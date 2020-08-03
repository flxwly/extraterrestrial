import xml.etree.ElementTree as ET
from sys import setrecursionlimit

import cv2
import numpy as np
import numpy.linalg as la
from PIL import Image, ImageDraw

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
    cosang = np.dot(v1, v2)
    sinang = la.norm(np.cross(v1, v2))
    return np.arctan2(sinang, cosang)


def add_to_arr_switch(field, p, walls, traps, swamps, deposits):
    switcher = {
        1: walls.append(p),
        2: traps.append(p),
        3: swamps.append(p),
        4: deposits.append(field.avg(p.x, p.y)),
        5: print(p)
    }
    switcher.get(p.val, -1)


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
        t_img = cv2.resize(cv2.imread(_dir + "/Background.bmp"), None, fx=0.25, fy=0.25,
                           interpolation=cv2.INTER_NEAREST)

        self.img_arr = [[Pixel(self, i, j, t_img[j][i]) for i in range(len(t_img[j]))] for j in
                        range(len(t_img))]  # TODO: Add support for more specific files
        self.width = len(self.img_arr[0])
        self.height = len(self.img_arr)

    def __str__(self):
        s = ""
        for row in self.img_arr:
            s += str(row) + "\n"
        return s

    def flood_fill(self, i, j, old_val, new_val=-1, activate=False, flags=None, arr=None):
        if self.img_arr[j][i].val == old_val != 0:
            if arr is not None:
                arr.append(self.img_arr[j][i])
            self.img_arr[j][i].set_val(new_val)
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
        old_val = self.img_arr[j][i].val
        self.flood_fill(i, j, old_val, new_val=-21, arr=pixels)

        for p in pixels:
            for n in range(p.y - r, p.y + r):
                for m in range(p.x - r, p.x + r):
                    if 0 <= m < self.width and 0 <= n < self.height:
                        if self.img_arr[n][m].val != -1:
                            self.img_arr[n][m].set_val(old_val)
        self.flood_fill(i, j, -21, new_val=old_val)

    def expand_all(self, val, r):
        pixels = self.collect_pixels(val)

        for p in pixels:
            for j in range(p.y - r, p.y + r):
                for i in range(p.x - r, p.x + r):
                    if 0 <= i < self.width and 0 <= j < self.height:
                        self.img_arr[j][i].set_val(val)

    def collect_pixels(self, val):
        pixels = []
        for j in range(self.height):
            for i in range(self.width):
                if self.img_arr[j][i].val == val:
                    pixels.append(self.img_arr[j][i])
        return pixels

    def check_pattern(self, pixel, patterns=None):
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
            vals = [int(p.is_wall) for p in square]
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


def get_nodes(field, wall_piece):
    walls = wall_piece
    corner_walls = []
    nodes = []

    #  collect all possible nodes by pattern recognition
    for wall in walls:
        matches = field.check_pattern(wall)
        for match in matches:
            nodes.append(Node(field, match[1].x, match[1].y, [match[0].x - match[1].x, match[0].y - match[1].y],
                              reachable=match[1].arr is not None))

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
        node.find_wall_connections(nodes)

    return nodes


class MapData:
    def __init__(self, img_dirs, fd_dirs):
        print("Creating MapData-Object...")

        self.walls = []
        self.nodes = []
        self.traps = []
        self.swamps = []
        self.deposit_areas = []

        self.img_arrs = []
        for _dir in img_dirs:
            print("Creating ImageArray-Object for %s" % _dir)
            self.img_arrs.append(ImageArray(_dir))
            img_arr = self.img_arrs[len(self.img_arrs) - 1]

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
                    elif p.val == 3:
                        swamps.append([])
                        img_arr.flood_fill(i, j, 3, -3, arr=swamps[len(swamps) - 1])
                        print("\t\tAdded swamp-struct")
                    elif p.val == 2:
                        print("\t\tAdded traps-struct")
                        traps.append([])
                        img_arr.flood_fill(i, j, 2, -2, arr=traps[len(traps) - 1])
                    elif p.val == 4:
                        print("\t\tAdded deposit_area")
                        deposit_areas.append(img_arr.avg(i, j, -4))

            print("\n\tCollecting nodes...")
            for wall_piece in walls:
                nodes.append(get_nodes(img_arr, wall_piece))

            walls = []
            ordered_nodes = []

            for node_struct in nodes:
                ordered_nodes.append([])
                open_nodes = node_struct
                node = open_nodes[0]
                open_nodes.pop(0)
                while len(open_nodes) > 0:
                    ordered_nodes[len(ordered_nodes) - 1].append(node)
                    for i in range(0, len(node.wall_connections)):
                        if node.wall_connections[i] in open_nodes:
                            node = node.wall_connections[i]
                            open_nodes.remove(node)
                            break

            for node_struct in ordered_nodes:
                walls.append([])
                for node in node_struct:
                    walls[len(walls) - 1].append(node.adjacend_wall)

            nodes = ordered_nodes

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
                    if p.is_wall:
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
    def __init__(self, x, y):
        self.x = x
        self.y = y


class Pixel(Point):
    def __init__(self, arr, x, y, color):
        super().__init__(x, y)

        self.arr = arr
        self.val = color_switch(tuple([color[0], color[1], color[2]]))
        self.is_wall = self.val == 1
        #  possible flags: marked, ignored
        self.flags = {"active": False,
                      "marked": False,
                      "ignored": False}

    def set_val(self, val):
        self.val = val
        self.is_wall = abs(val) == 1

    def __repr__(self):
        return "[%s,%s]" % (self.x, self.arr.height - self.y)

    def __str__(self):
        return "[%s | %s]: %s" % (self.x, self.arr.height - self.y, self.val)


class Node(Point):
    def __init__(self, field, x, y, direction, reachable=True):
        super().__init__(x, y)

        self.reachable = reachable
        self.field = field
        self.direction = direction
        self.adjacend_wall = field.img_arr[self.y + self.direction[1]][self.x + direction[0]]
        self.visibles = []
        self.wall_connections = []  # max 2

    def __iter__(self):
        return [self.x, self.y]

    def __repr__(self):
        return "[%s,%s]" % (self.x, self.field.height - self.y)

    def __str__(self):
        return "Node at [%s | %s]: %s" % (self.x, self.field.height - self.y, len(self.visibles))

    def can_see(self, node):
        if self is node or not self.reachable:
            return False
        visible = True
        if node not in self.visibles:
            points = get_line(self.x, self.y, node.x, node.y)
            for p in points:
                if 0 <= p[0] < self.field.width and 0 <= p[1] < self.field.height:
                    if self.field.img_arr[p[1]][p[0]].val == 1 or self.field.img_arr[p[1]][p[0]].val == -1:
                        visible = False
                        break
        return visible

    def find_visibles(self, nodes):
        for node in nodes:
            if self.can_see(node):
                node.visibles.append(self)
                self.visibles.append(node)

    def find_wall_connections(self, node_struct):

        #  print("finding wall_connections of: %s" % self)

        q = [self.adjacend_wall]
        removed = []

        while len(q) > 0:
            #  for each direct neighbor
            steps = ((q[0].x + 1, q[0].y), (q[0].x - 1, q[0].y), (q[0].x, q[0].y + 1), (q[0].x, q[0].y - 1))
            for step in steps:

                #  Out of bounds check
                if 0 <= step[0] < self.field.width and 0 <= step[1] < self.field.height:

                    #  n is the pixel that might be added
                    n = self.field.img_arr[step[1]][step[0]]
                    #  print("Checking if %s is valid" % n)

                    #  check if n is in q to prevent double checks
                    already_seen = False
                    if n in q or n in removed:
                        already_seen = True

                    if already_seen:
                        #  print("\t invalid (already look at it)")
                        continue

                    if not n.is_wall:
                        #  print("\t invalid (is no wall)")
                        continue

                    #  check if neighbor has a non wall neighbor
                    has_only_wall_neighbor = True
                    for i in range(n.x - 1, n.x + 2):
                        for j in range(n.y - 1, n.y + 2):
                            if 0 <= i < self.field.width and 0 <= j < self.field.height:
                                if not self.field.img_arr[j][i].is_wall:
                                    has_only_wall_neighbor = False
                                    break
                            else:
                                has_only_wall_neighbor = False
                                break

                    #  Further investigation can be skipped
                    if has_only_wall_neighbor:
                        #  print("\t invalid (has only wall neighbors)")
                        continue

                    #  check if wall is a valid connection to a node
                    is_node_connection = False
                    for node in node_struct:
                        if n is node.adjacend_wall:
                            self.wall_connections.append(node)
                            is_node_connection = True

                    if is_node_connection:
                        #  print("\t invalid (is node connection)")
                        continue

                    #  print("\t...added")
                    q.append(n)

            removed.append(q[0])
            q.pop(0)
            #  print(q)


class Collectible(Point):
    def __init__(self, t, x, y, world):
        # x and y are real coordinates in the simulator. Not the coordinates used by the robot
        super().__init__(float(x), float(y))

        #  Virtual coordinates are the ones used by the robot
        #  world 1 is 90 cm bigger in both directions (x, y)
        self.virtual_x = (270 + 90 * world) / 2 - 100 * float(x)
        self.virtual_y = 100 * float(y) + (180 + 90 * world) / 2

        #  The color (red = 0; cyan/green = 1; black = 2)
        self.t = t

    def __str__(self):
        return str(self.t) + ": " + str(self.virtual_x) + " | " + str(self.virtual_y)


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
