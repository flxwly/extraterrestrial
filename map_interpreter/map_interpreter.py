import os.path
import xml.etree.ElementTree as ET

import numpy.linalg as la
from numpy import arctan2, dot, cross

import cv2
from PIL import Image, ImageDraw
from sys import setrecursionlimit

setrecursionlimit(10000)

# for new CoSpace Versions
FieldA = "../../../../../store/media/Rescue/Map/Sec/Design/FieldA"
FieldB = "../../../../../store/media/Rescue/Map/Sec/Design/FieldB"
FieldFD = ET.parse("../../../../../store/media/Rescue/Map/Sec/Design/Field.FD")

cospace_version = "2.6.2"

# for CoSpace 2.6.2
if cospace_version == "2.6.2":
    FieldA = "../../../../../store/media/CS.C/RSC/Map/Design/FieldA"
    FieldB = "../../../../../store/media/CS.C/RSC/Map/Design/FieldB"
    FieldFD = ET.parse("../../../../../store/media/CS.C/RSC/Map/Design/Field.FD")

FieldFD = FieldFD.getroot()


def find_color_points(world, worldnr):
    arr = []
    for field_obj in world:
        if field_obj.tag == "FieldObjects":
            for obj in field_obj:
                is_object = False
                for attribute in obj:
                    if attribute.tag == "ObjType":
                        if attribute.text in ["Object_Black", "Object_Red", "Object_Green"]:
                            is_object = True
                            break
                        else:
                            break
                if is_object:
                    t = ""
                    cx = 0
                    cy = 0
                    for attribute in obj:
                        if attribute.tag == "ObjType":
                            t = attribute.text
                        if attribute.tag == "CX":
                            cx = attribute.text
                        if attribute.tag == "CY":
                            cy = attribute.text
                    arr.append(Collectible(t, cx, cy, worldnr))

    return arr


def get_line(x1, y1, x2, y2):
    points = []
    issteep = abs(y2 - y1) > abs(x2 - x1)
    if issteep:
        x1, y1 = y1, x1
        x2, y2 = y2, x2
    rev = False
    if x1 > x2:
        x1, x2 = x2, x1
        y1, y2 = y2, y1
        rev = True
    deltax = x2 - x1
    deltay = abs(y2 - y1)
    error = int(deltax / 2)
    y = y1
    ystep = None
    if y1 < y2:
        ystep = 1
    else:
        ystep = -1
    for x in range(x1, x2 + 1):
        if issteep:
            points.append((y, x))
        else:
            points.append((x, y))
        error -= deltay
        if error < 0:
            y += ystep
            error += deltax
    # Reverse the list if the coordinates were reversed
    if rev:
        points.reverse()
    return points


def py_ang(v1, v2):
    """ Returns the angle in radians between vectors 'v1' and 'v2'    """
    cosang = dot(v1, v2)
    sinang = la.norm(cross(v1, v2))
    return arctan2(sinang, cosang)


def color_switch(pixel):
    # format: b, g, r
    switcher = {
        tuple([0, 0, 0]): 1,  # wall
        tuple([151, 186, 221]): 1,
        tuple([131, 177, 244]): 1,
        tuple([0, 255, 255]): 2,  # trap
        tuple([166, 166, 166]): 3,  # swamp
        tuple([0, 153, 255]): 4,  # deposit
        tuple([0, 102, 255]): 4,
        tuple([240, 176, 0]): 5  # water
    }
    return switcher.get(pixel, 0)


class ImageArray:
    def __init__(self, _dir):
        self.img_arr = []

        self.width = 0
        self.height = 0

        if os.path.isdir(_dir):

            t_img = cv2.resize(cv2.imread(_dir + "/Background.bmp"), None, fx=0.25, fy=0.25,
                               interpolation=cv2.INTER_NEAREST)

            self.img_arr = [[Pixel(self, i, j, t_img[j][i]) for i in range(len(t_img[j]))] for j in
                            range(len(t_img))]  # TODO: Add support for more specific files
            self.width = len(self.img_arr[0])
            self.height = len(self.img_arr)
        else:
            print("Path does not exist")

    def __str__(self):
        s = ""
        for row in self.img_arr:
            s += str(row) + "\n"
        return s

    def flood_fill(self, i, j, old_val, new_val=-1, activate=False, flags=None, arr=None):
        if self.img_arr[j][i].val == old_val != 0:
            if arr is not None:
                arr.append(self.img_arr[j][i])
            self.img_arr[j][i].val = new_val
            for n in range(j - 1, j + 2):
                for m in range(i - 1, i + 2):
                    if 0 <= m < self.width and 0 <= n < self.height:  # boundary check
                        self.flood_fill(m, n, old_val, new_val, arr=arr)

    def get_adjacent(self, i, j):
        pixels = []
        old_val = self.img_arr[j][i].val
        self.flood_fill(i, j, old_val, arr=pixels)
        self.flood_fill(i, j, -1, old_val)

        return pixels

    def expand(self, i, j, r):
        pixels = []
        old_val = self.img_arr[j][i].initial_val
        self.flood_fill(i, j, old_val, new_val=-21, arr=pixels)

        for p in pixels:
            for n in range(p.y - r, p.y + r):
                for m in range(p.x - r, p.x + r):
                    if 0 <= m < self.width and 0 <= n < self.height:
                        if self.img_arr[n][m].val != -21:
                            self.img_arr[n][m].initial_val = old_val
                            self.img_arr[n][m].val = old_val
        self.flood_fill(i, j, -21, new_val=old_val)

    def expand_all(self, val, r):
        pixels = self.collect_pixels(val)

        for p in pixels:
            for j in range(p.y - r, p.y + r):
                for i in range(p.x - r, p.x + r):
                    if 0 <= i < self.width and 0 <= j < self.height:
                        self.img_arr[j][i].val = val

    def collect_pixels(self, val):
        pixels = []
        for j in range(self.height):
            for i in range(self.width):
                if self.img_arr[j][i].val == val:
                    pixels.append(self.img_arr[j][i])
        return pixels

    def check_pattern(self, pixel, patterns=None, val=1):
        if patterns is None:
            patterns = [[1, 0, 1, 1], [1, 0, 0, 0]]
        squares = []

        for i in range(-1, 2):
            for j in range(-1, 2):
                if i is not 0 and j is not 0:
                    x = pixel.x + i
                    y = pixel.y + j
                    #  if everything is inside the boundarys everything is good
                    if self.width > x >= 0 and self.height > y >= 0:
                        squares.append((pixel, self.img_arr[y][x], self.img_arr[pixel.y][x], self.img_arr[y][pixel.x]))

                    #  if x is outside and y is inside pixels that would be out of bounds are generated
                    elif (self.width <= x or x < 0) and self.height > y >= 0:
                        squares.append(
                            (pixel, Pixel(None, x, y, [255, 255, 255]), Pixel(None, x, pixel.y, [255, 255, 255]),
                             self.img_arr[y][pixel.x]))

                    #  if y is outside and x is inside pixels that would be out of bounds are generated
                    elif self.width > x >= 0 and (self.height <= y or y < 0):
                        squares.append(
                            (pixel, Pixel(None, x, y, [255, 255, 255]), Pixel(None, pixel.x, y, [255, 255, 255]),
                             self.img_arr[pixel.y][x]))

                    #  if x and y are outside pixels that would be out of bounds are generated
                    else:
                        squares.append(
                            (pixel, Pixel(None, x, y, [255, 255, 255]), Pixel(None, pixel.x, y, [255, 255, 255]),
                             Pixel(None, x, pixel.y, [255, 255, 255])))

        matches = []
        for square in squares:
            vals = [int(abs(p.initial_val) == val) for p in square]
            if vals in patterns:
                matches.append(square)

        return matches

    def avg(self, i, j, val):
        pixels = []
        old_val = self.img_arr[j][i].val
        self.flood_fill(i, j, old_val, new_val=val, arr=pixels)
        if len(pixels) > 0:
            x = 0
            y = 0
            for n in pixels:
                x += n.x
                y += n.y
            #  self.flood_fill(i, j, -1, new_val=old_val)
            return self.img_arr[int(y / len(pixels))][int(x / len(pixels))]
        return None


def get_nodes(field, struct):
    nodes = []

    #  collect all possible nodes by pattern recognition
    for obj in struct:
        matches = field.check_pattern(obj, val=obj.initial_val)
        for match in matches:
            nodes.append(Node(field, match[1].x, match[1].y, [match[0].x - match[1].x, match[0].y - match[1].y],
                              reachable=match[1].field is not None))
            print(nodes[len(nodes) - 1])

    #  collect all nodes that are unescessary
    nodes_to_delete = []
    checking_neighbour_steps = [[1, 1], [-1, 1]]
    for node in nodes:
        for step in checking_neighbour_steps:
            neighbour_count = 0
            for c_node in nodes:
                if c_node.x == node.x + step[0] and c_node.y == node.y + step[1]:
                    neighbour_count += 1
                elif c_node.x == node.x - step[0] and c_node.y == node.y - step[1]:
                    neighbour_count += 1

                if neighbour_count >= 2:
                    nodes_to_delete.append(node)

    #  delete them
    for node in nodes_to_delete:
        if node in nodes:
            nodes.remove(node)

    #  get the wall connections for each node

    for node in nodes:
        node.find_connections(nodes)
        print(node.boundary_connections)

    print(nodes)

    return nodes


def order_nodes(nodes):
    """Returns an array of nodes in order, so that "neighboring" nodes in the array represent a wall connection"""
    ordered = []

    #  nodes are already ordered by structs
    for node_struct in nodes:

        #  open_nodes are all unsorted nodes
        open_nodes = node_struct
        print("1.1")

        #  nodes should be ordered into structs afterwards too
        ordered.append([])

        #  n is the node that is currently checked
        n = open_nodes[0]
        open_nodes.pop(0)

        #  as long as there are nodes to be sorted
        while len(open_nodes) > 0:
            print("1.0.1")
            #  loop over the wall_connections and get the connection that is not already ordered
            for connection in n.boundary_connections:
                if connection in open_nodes:
                    n = connection
                    open_nodes.remove(n)

                    #  add last collected node to ordered
                    ordered[len(ordered) - 1].append(n)

                    #  break the loop to only add one node at a time
                    break
                print("1.0.2")

    return ordered


class MapData:
    def __init__(self, img_dirs, fd_dirs):
        """A Object to collect all other map objects and also convert them to string or to a picture"""
        print("Creating MapData-Object...")

        self.nodes = []  # "waypoints" for pathfinding/pathplanning
        self.walls = []  # wall_structs <- defined by points as polygon
        self.traps = []  # trap_structs <- defined by points as polygon
        self.swamps = []  # swamp_structs <- defined by points as polygon
        self.deposit_areas = []  # deposit areas defined as points

        self.img_arrs = []  # collection of ImageArray objects

        #  for every dear a new ImageArray has to be created
        for _dir in img_dirs:
            print("Creating ImageArray-Object for %s" % _dir)

            self.img_arrs.append(ImageArray(_dir))
            img_arr = self.img_arrs[len(self.img_arrs) - 1]

            #  objects that will be collected from the ImageArray and later put
            #  as on entry into their respective array
            walls = []
            nodes = []
            traps = []
            swamps = []
            deposit_areas = []

            print("\tConverting ImageArray")
            for j in range(img_arr.height):
                for i in range(img_arr.width):
                    p = img_arr.img_arr[j][i]
                    if p.val == 0:
                        pass
                    elif p.val == 1:
                        walls.append([])
                        img_arr.expand(i, j, 4)
                        img_arr.flood_fill(i, j, 1, -1, arr=walls[len(walls) - 1])
                        print("\t\tAdded wall-struct")
                    elif p.val == 2:
                        traps.append([])
                        img_arr.flood_fill(i, j, 2, -2, arr=traps[len(traps) - 1])
                        print("\t\tAdded traps-struct")
                    elif p.val == 3:
                        swamps.append([])
                        img_arr.flood_fill(i, j, 3, -3, arr=swamps[len(swamps) - 1])
                        print("\t\tAdded swamp-struct")
                    elif p.val == 4:
                        deposit_areas.append(img_arr.avg(i, j, -4))
                        print("\t\tAdded deposit_area")

            print("\n\tCollecting Nodes...")


            print("\t\tWall/normal Nodes...")
            wall_nodes = []
            for wall_piece in walls:
                wall_nodes.append(get_nodes(img_arr, wall_piece))
            print("1")
            wall_nodes = order_nodes(wall_nodes)
            nodes = wall_nodes
            print("2")
            for struct in wall_nodes:
                print("3")
                walls.append([node.adjacend_wall for node in struct])

            print("\t\t Trap Nodes")
            trap_nodes = []
            for trap_piece in traps:
                trap_nodes.append(get_nodes(img_arr, trap_piece))
            trap_nodes = order_nodes(trap_nodes)
            for struct in trap_nodes:
                traps.append([node.adjacend_wall for node in struct])

            print("\t\t Swamp Nodes")
            swamp_nodes = []
            for swamp_piece in swamps:
                swamp_nodes.append(get_nodes(img_arr, swamp_piece))
            swamp_nodes = order_nodes(swamp_nodes)
            for struct in swamp_nodes:
                swamps.append([node.adjacend_wall for node in struct])

            all_nodes = []
            for node_struct in nodes:
                for node in node_struct:
                    all_nodes.append(node)

            for node in all_nodes:
                node.find_visibles(all_nodes)

            self.walls.append(walls)
            self.nodes.append(nodes)
            self.traps.append(traps)
            self.swamps.append(swamps)
            self.deposit_areas.append(deposit_areas)

    def show(self, scale):

        x_off = scale
        y_off = scale

        i = 0
        for img_arr in self.img_arrs:
            im = Image.new('HSV', ((img_arr.width + 2) * scale, (img_arr.height + 2) * scale))
            draw = ImageDraw.Draw(im)

            for row in img_arr.img_arr:
                for p in row:
                    if p.initial_val == 1:
                        coord = (
                            p.x * scale - scale / 2 + x_off, p.y * scale - scale / 2 + y_off,
                            p.x * scale + scale / 2 + x_off, p.y * scale + scale / 2 + y_off)
                        draw.rectangle(coord, width=scale, fill=(255, 0, 255))

            j = 0
            for node_struct in self.nodes[i]:
                for n in node_struct:
                    #  the node itself
                    coord = (
                        n.x * scale - scale / 2 + x_off, n.y * scale - scale / 2 + y_off,
                        n.x * scale + scale / 2 + x_off, n.y * scale + scale / 2 + y_off)
                    draw.rectangle(coord, width=scale,
                                   fill=(90 * int(not n.reachable), 255, 255))

                    #  line to adjacent wall
                    coord = (
                        n.x * scale + x_off, n.y * scale + + y_off,
                        (n.x + n.direction[0]) * scale + x_off, (n.y + n.direction[1]) * scale + y_off)
                    draw.line(coord, fill=(60, 255, 255))

                    #  line to visibles
                    for visible in n.wall_connections:
                        if visible is not None:
                            coord = (
                                n.x * scale + x_off, n.y * scale + + y_off,
                                visible.x * scale + x_off, visible.y * scale + + y_off)
                            draw.line(coord, fill=(j * 40, 255, 255, 50))
                        else:
                            print("Error")
                j += 1
                # points.append((n.x, n.y))
            im.show("Map%s" % i)
            i += 1

    def __str__(self):
        file_content = ""
        i = 0
        for img_arr in self.img_arrs:
            wall_str = "std::vector<Obstacle> GAME %s WALLS = " % i  # {{x1, y1}, {x2, y2}...} (Wall Polygon)
            trap_str = "std::vector<Trap> GAME %s TRAPS = " % i  # {{x1, y1}, {x2, y2}...} (Trap Polygon)
            swamp_str = "std::vector<std::pair<int, int>> GAME %s SWAMPS = " % i  # {{x1, y1}, {x2, y2}...} (Swamp Polygon)
            deposit_area_str = "std::vector<std::pair<int, int>> GAME %s DEPOSITAREAS = " % i  # {{x1, y1}, {x2, y2}...} (Single Deposit_Area points)
            node_str = "std::vector<Node> GAME %s Nodes = " % i  # {{x1, y1}, {x2, y2}...} (Node Objects)

            wall_str += str(self.walls[i])
            trap_str += str(self.traps[i])
            swamp_str += str(self.swamps[i])
            deposit_area_str += str(self.deposit_areas[i])
            node_str += str(self.nodes[i])

            file_content += "//------------- Game%s_Objects --------------//\n\n" % i

            file_content += "/*walls*/ " + wall_str + \
                            "\n/*traps*/ " + trap_str + \
                            "\n/*swamps*/ " + swamp_str + \
                            "\n\n/*deposit*/ " + deposit_area_str + \
                            "\n\n/*nodes*/ " + node_str + "\n\n"

            i += 1

        return file_content


class Point:
    def __init__(self, x, y, field):
        """A simple Point in a 2D grid"""
        self.x = x
        self.y = y

        #  The array the Point is saved in
        self.field = field

    def __repr__(self):
        """Returns Point as typical C++ pair coord"""
        return "{%s,%s}" % (self.x, self.field.height - self.y)


class Pixel(Point):
    def __init__(self, field, x, y, color):
        super().__init__(x, y, field)
        """A type of point used to represent the actual image"""

        #  value of the Pixel that never changes
        self.initial_val = color_switch(tuple([color[0], color[1], color[2]]))

        #  value that can be modified (for marking)
        self.val = self.initial_val

    def __str__(self):
        """Returns Pixel as coord including it's val. For Debugging"""
        return "Pixel at [%s | %s] has value %s" % (self.x, self.field.height - self.y, self.val)


class Node(Point):
    def __init__(self, field, x, y, direction, reachable=True):
        super().__init__(x, y, field)
        """A type of Point to represent Nodes. Each Node is bound to a wall boundary and vise versa"""

        #  The array a node is stored in
        self.field = field

        #  If the node is inside the arrays dimension (reachable == false -> field = None)
        self.reachable = reachable

        #  The direction from the node to it's boundary
        self.direction = direction

        #  The boundary a node is bound to
        self.boundary = field.img_arr[self.y + self.direction[1]][self.x + direction[0]]

        #  An array of Nodes that create a connection around a certain object
        self.boundary_connections = []  # max 2

        #  All other Nodes this Node can see (only for debugging)
        self.visible_nodes = []

    def can_see(self, point, excluded_init_vals=1):
        """Returns a boolean weather this Node can see a point. Works best if point is a Node"""

        #  A Node can't see itself or anything if it's out of bounds
        if self is point or not self.reachable:
            return False

        #  This only increases efficiency if point is a node
        if point not in self.visible_nodes:

            # Get the points on the line between the Node and a point
            points_on_line = get_line(self.x, self.y, point.x, point.y)
            for p in points_on_line:

                #  Check if any of the points "blocks" the vision
                if self.field.img_arr[p[1]][p[0]].initial_val in excluded_init_vals:
                    return False

        return True

    def find_visibles(self, nodes, excluded_init_vals=1):
        """Collects all visible Nodes"""
        for node in nodes:

            #  Use inbuild function to determine visibility
            if self.can_see(node, excluded_init_vals=excluded_init_vals):
                #  Add as visible to both Nodes
                node.visible_nodes.append(self)
                self.visible_nodes.append(node)

        #  idk why
        return self.visible_nodes

    def find_connections(self, node_struct):
        """Gets the two connection_nodes for a certain Node"""

        #  boundary value
        b_v = self.boundary.initial_val
        print(b_v)

        #  q (short for queue) used for a non recursiv flood fill
        q = [self.boundary]

        #  To not go back (could've also used node.val but it needs to be reversed afterwards)
        removed = []

        while len(q) > 0:
            #  for each direct neighbor
            steps = ((q[0].x + 1, q[0].y), (q[0].x - 1, q[0].y), (q[0].x, q[0].y + 1), (q[0].x, q[0].y - 1))
            for step in steps:

                #  Out of bounds check
                if 0 <= step[0] < self.field.width and 0 <= step[1] < self.field.height:

                    #  n is the pixel that might be added
                    p = self.field.img_arr[step[1]][step[0]]
                    #  print("Checking if %s is valid" % n)

                    #  check if n is or was in q to prevent double checks
                    already_seen = False
                    if p in q or p in removed:
                        #print("\t invalid (already look at it)")
                        continue

                    #  check if the values match
                    if p.initial_val != b_v:
                        #print("\t invalid (value doesn't match)")
                        continue

                    #  check if neighbor has a neighbor that has a different initial value
                    has_different_neighbor = False
                    for i in range(p.x - 1, p.x + 2):
                        for j in range(p.y - 1, p.y + 2):

                            #  a field outside of bounds is counted as a neighbor of other type
                            if 0 <= i < self.field.width and 0 <= j < self.field.height:
                                if self.field.img_arr[j][i].initial_val != b_v:
                                    has_different_neighbor = True
                                    break
                            else:
                                has_different_neighbor = True
                                break

                    if has_different_neighbor:
                        #print("\t invalid (has only wall neighbors)")
                        continue

                    #  check if wall is a valid connection to a node
                    is_node_connection = False
                    for node in node_struct:
                        if p is node.boundary:
                            self.boundary_connections.append(node)
                            print("found boundary: %s" % node)
                            is_node_connection = True

                    if is_node_connection:
                        #print("\t invalid (is node connection)")
                        continue

                    #  this pixel can be added to the queue
                    q.append(p)

            removed.append(q[0])
            q.pop(0)
            #  print(q)

    def __str__(self):
        """Returns Node as coord including how many other nodes this node can see. For Debugging"""
        return "Node at [%s | %s] has %s visible Nodes" % (self.x, self.field.height - self.y, len(self.visible_nodes))


class Collectible(Point):
    def __init__(self, t, x, y, world):
        super().__init__(float(x), float(y), None)
        """A type of Point to represent Collectible points"""

        # x and y are real coordinates in the simulator. Not the coordinates used by the robot
        #  Virtual coordinates are the ones used by the robot
        #  world 1 is 90 cm bigger in both directions (x, y)
        self.virtual_x = (270 + 90 * world) / 2 - 100 * float(x)
        self.virtual_y = 100 * float(y) + (180 + 90 * world) / 2

        #  The color (red = 0; cyan/green = 1; black = 2)
        self.t = t

    def __str__(self):
        """Returns Collectible as coord including it's color. For Debugging"""
        return "Collectible at [%s | %s] with color %s" % (self.x, self.y, self.t)


def convert_background(field, worldnr):  # TODO: Add to field.__init__ class
    print("converting Background: " + str(worldnr))

    walls = []
    wall_str = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "WALLS = "
    traps = []
    trap_str = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "TRAPS = "
    swamps = []
    swamp_str = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "SWAMPS = "
    deposit_areas = []
    deposit_area_str = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "DEPOSITAREAS = "

    print("\tconvert")

    for j in range(field.height):
        for i in range(field.width):
            p = field.img_arr[j][i]
            if p.val == 0:
                pass
            elif p.val == 1:
                walls.append([])
                field.flood_fill(i, j, 1, -1, arr=walls[len(walls) - 1])
                print("Added wall struct")
            elif p.val == 3:
                swamps.append(p)
            elif p.val == 2:
                traps.append(p)
            elif p.val == 4:
                deposit_areas.append(field.avg(i, j, -4))

    for wall_piece in walls:
        field.nodes.append(get_nodes(field, wall_piece))

    all_nodes = []
    for node_struct in field.nodes:
        for node in node_struct:
            all_nodes.append(node)

    for node_struct in field.nodes:
        for node in node_struct:
            node.find_visibles(all_nodes)
            node.find_wall_connections(node_struct)

    print("convert to str")
    wall_str += str(walls).replace("[", "{").replace("]", "}").replace(" ", "") + ";"
    trap_str += str(traps).replace("[", "{").replace("]", "}").replace(" ", "") + ";"
    swamp_str += str(swamps).replace("[", "{").replace("]", "}").replace(" ", "") + ";"
    deposit_area_str += str(deposit_areas).replace("[", "{").replace("]", "}") + ";"

    filecontent = "/*walls*/ " + wall_str + "\n/*traps*/ " + trap_str + "\n/*swamps*/ " + swamp_str + "\n/*deposit*/ " + \
                  deposit_area_str + "\n\n"  # + "\n/*nodes*/ " + node_str + "\n\n"

    return filecontent


def write_points_to_file(points, worldnr):
    redpoints = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "REDPOINTS = {"
    greenpoints = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "GREENPOINTS = {"
    blackpoints = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "BLACKPOINTS = {"

    for point in points:
        if point.t == "Object_Red":
            redpoints += "{" + str(int(point.virtual_x)) + ", " + str(int(point.virtual_y)) + "}, "
        if point.t == "Object_Green":
            greenpoints += "{" + str(int(point.virtual_x)) + ", " + str(int(point.virtual_y)) + "}, "
        if point.t == "Object_Black":
            blackpoints += "{" + str(int(point.virtual_x)) + ", " + str(int(point.virtual_y)) + "}, "

    redpoints = (redpoints + "}").replace(", }", "};")
    greenpoints = (greenpoints + "}").replace(", }", "};")
    blackpoints = (blackpoints + "}").replace(", }", "};")

    filecontent = redpoints + "\n" + greenpoints + "\n" + blackpoints + "\n\n\n"
    return filecontent


def main():
    print("setting up...")

    mapData = MapData(img_dirs=(FieldA, FieldB), fd_dirs=FieldFD)

    mapData.show(10)

    print(mapData)


def has_to_be_reworked():
    world_1_points = []
    world_2_points = []
    for child in FieldFD:
        if child.tag == "World1":
            world_1_points = find_color_points(child, 0)
        if child.tag == "World2":
            world_2_points = find_color_points(child, 1)
    filename = "../code/MapData.cpp"

    print("getting objects... (this might take some time)")

    f = open("mapDataCPP", "r")
    cpp_data = f.read() + "\n\n"
    f.close()

    field_a = ImageArray(FieldA)
    field_a.expand_all(1, 5)  # expand all walls by 5
    field_b = ImageArray(FieldB)
    field_b.expand_all(1, 5)  # expand all walls by 5

    field_a_str = convert_background(field_a, 0)
    field_a_points = write_points_to_file(world_1_points, 0)
    field_b_str = convert_background(field_b, 1)
    field_b_points = write_points_to_file(world_2_points, 1)
    #  print("writing to file...")
    #  f = open(filename, "w+")
    #  f.write(cpp_data + field_a_str + field_a_points + field_b_str + field_b_points)
    #  f.close()

    print("finished")


if __name__ == '__main__':
    main()
