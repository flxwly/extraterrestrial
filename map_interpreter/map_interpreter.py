import os.path
import random
import xml.etree.ElementTree as ET
from sys import setrecursionlimit

import cv2
from PIL import Image, ImageDraw

setrecursionlimit(10000)

# 1 is max detail
detail = 0.5

# for old CoSpace Versions
FieldA = "../../../../../store/media/Rescue/Map/Sec/Design/FieldA"
FieldB = "../../../../../store/media/Rescue/Map/Sec/Design/FieldB"
FieldFD = ET.parse("../../../../../store/media/Rescue/Map/Sec/Design/Field.FD")

cospace_version = "2.6.2"

# for CoSpace 2.6.2+
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


def color_switch(pixel):
    # format: b, g, r
    switcher = {
        tuple([0, 0, 0]): 1,  # wall
        tuple([151, 186, 221]): 1,
        tuple([131, 177, 244]): 1,
        tuple([0, 255, 255]): 0,  # trap (only use the blue in the middle)
        tuple([151, 85, 47]): 2,
        tuple([166, 166, 166]): 3,  # swamp
        tuple([0, 153, 255]): 4,  # deposit
        tuple([0, 102, 255]): 4,
        tuple([240, 176, 0]): 5  # water
    }
    return switcher.get(pixel, 0)


def remove_dif_pixels(pixels, val):
    """A method to check if all pixels in Pixels have the value val. It returns all different pixels"""

    different_pixels = []

    #  loop through every pixel in pixels
    for struct in pixels:
        for pixel in struct:
            # value check
            if abs(pixel.val) != val:
                #  remove pixel right away
                struct.remove(pixel)
                print("removed")
                different_pixels.append(pixel)

    return different_pixels


class ImageArray:
    """Completely reworked and commented"""

    def __init__(self, _dir):
        """A class to collect one single map as Pixels by converting _dir/Background.bmp"""

        global detail

        # The array all pixels are stored in
        self.img_arr = []

        # The dimensions of the map
        self.width = 0
        self.height = 0

        # if _dir doesn't exist the user has to change the input files
        if os.path.isdir(_dir):

            # resize the image for less detail but less ram usage and duration
            t_img = cv2.resize(cv2.imread(_dir + "/Background.bmp"), None, fx=detail, fy=detail,
                               interpolation=cv2.INTER_NEAREST)

            # convert the Image into pixels
            self.img_arr = [[Pixel(self, i, j, t_img[j][i]) for i in range(len(t_img[j]))] for j in
                            range(len(t_img))]  # TODO: Add support for more specific files

            # set the dimensions
            self.width = len(self.img_arr[0])
            self.height = len(self.img_arr)
        else:
            # Error
            exit("Path does not exist")

    def flood_fill(self, i, j, old_val, new_val, store=False):
        """This method is used to fill a region of Pixels with same value.
            It can also be used to just store them in an array by setting an arr"""

        #  Normal recursive 8-way floodfill
        if not store:
            #  Check if pixel.val equals the starting val
            if self.img_arr[j][i].val == old_val:

                #  Replace the value
                self.img_arr[j][i].val = new_val

                #  invoke on all neighboring pixels with same parameters but different coords
                for n in range(j - 1 if j - 1 >= 0 else 0, j + 2 if j + 1 < self.height else self.height):
                    for m in range(i - 1 if i - 1 >= 0 else 0, i + 2 if i + 1 < self.width else self.width):
                        #  store=False is standard and therefore doesn't have to be set
                        self.flood_fill(m, n, old_val, new_val)

        #  non recursive 8-way flood-fill
        else:

            #  q (short for queue) used for a non recursiv flood fill
            q = [self.img_arr[j][i]]

            #  To avoid setting values go back
            stored = []

            while len(q) > 0:

                #  for each neighbor (8x8)
                for n in range(q[0].y - 1 if q[0].y - 1 >= 0 else 0,
                               q[0].y + 2 if q[0].y + 1 < self.height else self.height):
                    for m in range(q[0].x - 1 if q[0].x - 1 >= 0 else 0,
                                   q[0].x + 2 if q[0].x + 1 < self.width else self.width):

                        #  p is the pixel that might be added
                        p = self.img_arr[n][m]
                        #  print("Checking if %s is valid" % n)

                        #  check if n is or was in q to prevent double checks
                        if p in q or p in stored:
                            # print("\t invalid (already look at it)")
                            continue

                        #  check if the values match
                        if p.val != old_val:
                            # print("\t invalid (value doesn't match)")
                            continue

                        #  The pixels value is updated
                        p.val = new_val

                        #  this pixel can be added to the queue
                        q.append(p)

                #  append to store array and remove from queue

                stored.append(q[0])
                q.pop(0)

            return stored
        return None

    def get_adjacent(self, i, j):
        """Function to get all adjacent Pixels of the same type"""

        #  Use the flood_fill function with store=true to
        old_val = self.img_arr[j][i].val
        return self.flood_fill(i, j, old_val, old_val, store=True)

    def expand(self, i, j, r):
        """Function to expand the type of every pixel of the same type around i, j by r"""

        # Collect all adjacent pixels
        pixels = self.get_adjacent(i, j)

        # Array for storing all new added Pixels
        changed_pixels = []

        # Expand for every collected pixel
        for p in pixels:

            # set val in radius r
            for n in range(p.y - r if p.y - r >= 0 else 0, p.y + r + 1 if p.y + r < self.height else self.height):
                for m in range(p.x - r if p.x - r >= 0 else 0, p.x + r + 1 if p.x + r < self.width else self.width):

                    # disable expanding over walls
                    if abs(self.img_arr[n][m].val) != 1:
                        self.img_arr[n][m].val = p.val
                        changed_pixels.append(self.img_arr[n][m])

        return changed_pixels

    def expand_all(self, val, r, pixels=None):
        """This function expands every pixel of a certain type by r"""
        if pixels is None:
            pixels = self.collect_pixels(val)

        # Array for storing all new added Pixels
        changed_pixels = []

        # expand for every struct in pixels
        for struct in pixels:
            changed_pixels.append(self.expand(struct[0].x, struct[0].y, r))

        return changed_pixels

    def collect_pixels(self, val):
        """This function returns every pixel of type val in self.img_arr in form of structs"""
        """NOTE: This function does not work on deactivated pixels and only collects acivated pixels"""

        pixels = []

        disabled_val = -val

        # loop through the complete array
        for j in range(self.height):
            for i in range(self.width):
                if self.img_arr[j][i].val == val:

                    # get all adjacent pixels to put them into a struct
                    pixels.append(self.get_adjacent(i, j))

                    # disable all pixels for further inspections
                    for p in pixels[len(pixels) - 1]:
                        p.val = disabled_val

        # activate disabled pixels again
        for struct in pixels:
            for p in struct:
                p.val = val

        return pixels

    def check_pattern(self, pixel, patterns=None, val=1):
        """Function to check if the pattern matches.
            There are default patterns and the default value the pattern is referring to is 1"""

        #  Use default pattern if no extra pattern is provided
        if patterns is None:
            patterns = [[1, 0, 1, 1], [1, 0, 0, 0]]

        #  used to store a 2x2 square of pixels that later is compared to the pattern
        squares = []

        # check the diganoal neighbors if there outside. If theyre outside add a Pixel with no val
        steps = ((1, 1), (1, -1), (-1, 1), (-1, -1))
        for step in steps:
            x = pixel.x + step[0]
            y = pixel.y + step[1]

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

        #  check for matches
        matches = []
        for square in squares:

            # Since the patterns is a int(bool) array use val to compare
            vals = [int(abs(p.val) == abs(val)) for p in square]

            # Compare the patterns
            if vals in patterns:
                matches.append(square)

        return matches

    def avg(self, i, j):
        """This function calculates the average point of all adjacent Pixels to the Pixel at i, j"""

        # Get adjacent pixels
        pixels = self.get_adjacent(i, j)

        # Calculate the average
        if len(pixels) > 0:
            x = 0
            y = 0
            for n in pixels:
                x += n.x
                y += n.y

            # also return the pixels if they need to be deactivated
            return [int(x / len(pixels)), int(y / len(pixels)), pixels]

        return None

    def __str__(self):
        s = ""
        for row in self.img_arr:
            s += str(row) + "\n"
        return s


def get_nodes(field, struct):
    nodes = []

    #  collect all possible nodes by pattern recognition
    for obj in struct:
        matches = field.check_pattern(obj, val=obj.val)
        for match in matches:
            nodes.append(Node(field, match[1].x, match[1].y, [match[0].x - match[1].x, match[0].y - match[1].y],
                              reachable=match[1].field is not None))
            # print("nodes after adding %s: %s" % (match[1], nodes))

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

    return nodes


# TODO: Find bug in this function that creates an endless loop when trying to order trap-nodes
#  This is needed to give the pathfinder of the robot the possibility to also watch for traps
def order_nodes(nodes):
    """Returns an array of nodes in order, so that "neighboring" nodes in the array represent a wall connection"""

    ordered = []

    #  nodes are already ordered by structs
    for node_struct in nodes:

        #  open_nodes are all unsorted nodes
        open_nodes = node_struct

        #  nodes should be ordered in structs afterwards too
        ordered.append([])

        #  n is the node that is currently checked
        n = open_nodes[0]

        #  the first node that is checked is alays at the front
        ordered[len(ordered) - 1].append(n)
        open_nodes.pop(0)

        #  as long as there are nodes to be sorted
        while len(open_nodes) > 0:

            # print("len of open_nodes: " + str(len(open_nodes)))
            # print("len of boundaries: " + str(len(n.boundary_connections)))

            #  loop over the wall_connections and get the connection that is not already ordered
            for boundary_connection in n.boundary_connections:
                #  there should always be at least one boundary_connection in open_nodes
                if boundary_connection in open_nodes:
                    #  the boundary_connection in open_nodes is the new node that has to be checked.
                    n = boundary_connection

            #  add last add boundary to ordered and remove it from open_nodes
            open_nodes.remove(n)
            ordered[len(ordered) - 1].append(n)

    return ordered


def draw_polygons(draw, polygons, scale, starting_hue=0, sat=255, lum=255, hue_dif=40):
    """draw the boundaries of each polygon in polygons"""

    x_off = scale
    y_off = scale

    # j is to differentiate each polygon from each other
    j = 0
    for polygon in polygons:
        for k in range(0, len(polygon)):
            p1 = polygon[k]
            p2 = polygon[(k + 1) % (len(polygon))]
            for p in get_line(p1.x, p1.y, p2.x, p2.y):
                coord = (
                    p[0] * scale - scale / 2 + x_off, p[1] * scale - scale / 2 + y_off,
                    p[0] * scale + scale / 2 + x_off, p[1] * scale + scale / 2 + y_off)
                draw.rectangle(coord, width=scale, fill=(starting_hue + j * hue_dif, sat, lum))

            coord = (
                p1.x * scale - scale / 2 + x_off, p1.y * scale - scale / 2 + y_off,
                p1.x * scale + scale / 2 + x_off, p1.y * scale + scale / 2 + y_off)
            draw.rectangle(coord, width=scale, fill=(2 * starting_hue + j * hue_dif, sat + 50, lum))

        j += 1


class MapData:
    def __init__(self, img_dirs, fd_dirs):
        """A Object to collect all other map objects and also convert them to string or to a picture"""
        print("Creating MapData-Object...")

        self.wall_nodes = []  # "waypoints" for pathfinding/pathplanning
        self.walls = []  # wall_structs <- defined by points as polygon
        self.trap_nodes = []
        self.traps = []  # trap_structs <- defined by points as polygon
        self.swamp_nodes = []
        self.swamps = []  # swamp_structs <- defined by points as polygon
        self.deposit_areas = []  # deposit areas <- defined as points
        self.bonus_area_nodes = []
        self.bonus_areas = []  # Bonus_areas <- defined by points as polygon

        self.img_arrs = []  # collection of ImageArray objects

        """ In this loop the img_arrays are created and converted """
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
            bonus_areas = []

            print("\tConverting ImageArray")

            img_arr.expand_all(1, int(detail * 16))
            img_arr.expand_all(2, int(detail * 16))
            img_arr.expand_all(3, int(detail * 2))
            img_arr.expand_all(5, int(detail * 2))

            for j in range(img_arr.height):
                for i in range(img_arr.width):
                    p = img_arr.img_arr[j][i]
                    if p.val == 0:
                        pass
                    elif p.val == 1:
                        walls.append(img_arr.flood_fill(i, j, 1, -1, store=True))
                        print("\t\tAdded wall-struct")
                    elif p.val == 2:
                        traps.append(img_arr.flood_fill(i, j, 2, -2, store=True))
                        print("\t\tAdded traps-struct")
                    elif p.val == 3:
                        swamps.append(img_arr.flood_fill(i, j, 3, -3, store=True))
                        print("\t\tAdded swamp-struct")
                    elif p.val == 4:
                        avg = img_arr.avg(i, j)

                        # deactivate pixels
                        for p in avg[2]:
                            p.val = -4

                        deposit_areas.append(img_arr.img_arr[avg[1]][avg[0]])
                        print("\t\tAdded deposit_area")
                    elif p.val == 5:
                        bonus_areas.append(img_arr.flood_fill(i, j, 5, -5, store=True))
                        print("\t\tAdded wall-struct")

            #  activate all pixels again
            for j in range(img_arr.height):
                for i in range(img_arr.width):
                    img_arr.img_arr[j][i].val = abs(img_arr.img_arr[j][i].val)

            print("\n\tCollecting Nodes...")

            print("\t\tWall/normal Nodes...")
            wall_nodes = []
            for wall_piece in walls:
                wall_nodes.append(get_nodes(img_arr, wall_piece))
            wall_nodes = order_nodes(wall_nodes)
            walls = []
            for struct in wall_nodes:
                walls.append([node.boundary for node in struct])

            print("\t\t Trap Nodes")
            trap_nodes = []
            for trap_piece in traps:
                trap_nodes.append(get_nodes(img_arr, trap_piece))
            trap_nodes = order_nodes(trap_nodes)
            traps = []
            for struct in trap_nodes:
                traps.append([node.boundary for node in struct])

            print("\t\t Swamp Nodes")
            swamp_nodes = []
            for swamp_piece in swamps:
                swamp_nodes.append(get_nodes(img_arr, swamp_piece))
            swamp_nodes = order_nodes(swamp_nodes)
            swamps = []
            for struct in swamp_nodes:
                swamps.append([node.boundary for node in struct])

            print("\t\t Bonus-Area Nodes")
            bonus_area_nodes = []
            for bonus_piece in bonus_areas:
                bonus_area_nodes.append(get_nodes(img_arr, bonus_piece))
            bonus_area_nodes = order_nodes(bonus_area_nodes)
            bonus_areas = []
            for struct in bonus_area_nodes:
                bonus_areas.append([node.boundary for node in struct])

            all_nodes = []
            for node_struct in nodes:
                for node in node_struct:
                    all_nodes.append(node)

            for node in all_nodes:
                node.find_visibles(all_nodes)

            self.walls.append(walls)
            self.wall_nodes.append(wall_nodes)
            self.traps.append(traps)
            self.trap_nodes.append(trap_nodes)
            self.swamps.append(swamps)
            self.swamp_nodes.append(swamp_nodes)
            self.deposit_areas.append(deposit_areas)
            self.bonus_areas.append(bonus_areas)
            self.bonus_area_nodes.append(bonus_area_nodes)

            print("\tfinished")

    def show(self, scale):

        x_off = scale
        y_off = scale

        i = 0
        for img_arr in self.img_arrs:
            im = Image.new('HSV', ((img_arr.width + 2) * scale, (img_arr.height + 2) * scale))
            draw = ImageDraw.Draw(im)

            # show the nodes
            j = 0
            for node_struct in self.wall_nodes[i]:
                for n in node_struct:
                    n.show(draw, scale, show_visibles=False)

            # show the walls
            draw_polygons(draw, self.walls[i], scale, sat=0, lum=255)

            # show the traps
            draw_polygons(draw, self.traps[i], scale, starting_hue=40, hue_dif=0)

            # show the swamps
            draw_polygons(draw, self.swamps[i], scale, sat=0, lum=50)

            # show the bonus_areas
            draw_polygons(draw, self.bonus_areas[i], scale, starting_hue=80, hue_dif=0)

            im.show("Map%s" % i)
            i += 1

    def __str__(self):
        file_content = ""
        file_content += "/**     -----------     **/\n" \
                        "/**                     **/\n" \
                        "/**       MapData       **/\n" \
                        "/**                     **/\n" \
                        "/**     -----------     **/\n\n"


        i = 0
        for img_arr in self.img_arrs:
            wall_str = "std::vector<Area>GAME%sWALLS = " % i  # {{{x1, y1}, {x2, y2}...}, {...}} (Wall Polygon)
            node_str = "std::vector<Point>GAME%sNODES = " % i  # {{x1, y1}, {x2, y2}...} (Node Objects)
            trap_str = "std::vector<Area>GAME%sTRAPS = " % i  # {{{x1, y1}, {x2, y2}...}, {...}} (Trap Polygon)
            swamp_str = "std::vector<Area>GAME%sSWAMPS = " % i  # {{x1, y1}, {x2, y2}...} (Swamp Polygon)
            bonus_area_str = "std::vector<Area>GAME%sWATERS = " % i  # {{x1, y1}, {x2, y2}...} (Water Polygon)
            deposit_area_str = "std::vector<Point>GAME%sDEPOSITS = " % i  # {{x1, y1}, {x2, y2}...} (# Single Deposit_Area points)

            wall_str += str(self.walls[i]).replace("[", "{").replace("]", "}").replace(" ", "") \
                            .replace("}},", "}},\n\t") + ";"
            node_str += str(self.wall_nodes[i]).replace("[", "{").replace("]", "}").replace(" ", "") \
                            .replace("}},", "}},\n\t") + ";"
            trap_str += str(self.traps[i]).replace("[", "{").replace("]", "}").replace(" ", "") \
                            .replace("}},", "}},\n\t") + ";"
            swamp_str += str(self.swamps[i]).replace("[", "{").replace("]", "}").replace(" ", "") \
                             .replace("}},", "}},\n\t") + ";"
            bonus_area_str += str(self.bonus_areas[i]).replace("[", "{").replace("]", "}").replace(" ", "") \
                                  .replace("}},", "}},\n\t") + ";"
            deposit_area_str += str(self.deposit_areas[i]).replace("[", "{").replace("]", "}").replace(" ", "") + ";"

            file_content += "//------------- Game%s_Objects --------------//\n\n" % i

            file_content += "\t\t/*walls*/\n" + wall_str + \
                            "\n\t\t/*nodes*/\n" + node_str + \
                            "\n\t\t/*traps*/\n" + trap_str + \
                            "\n\t\t/*swamps*/\n" + swamp_str + \
                            "\n\t\t/*Water*/\n" + bonus_area_str + \
                            "\n\t\t/*deposit*/\n" + deposit_area_str + "\n\n"

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
    """A type of point used to represent the actual image"""

    def __init__(self, field, x, y, color):
        super().__init__(x, y, field)

        #  value of the Pixel that is never changed
        self.initial_val = color_switch(tuple([color[0], color[1], color[2]]))

        #  value that can be modified (for marking)
        self.val = self.initial_val

    def __str__(self):
        """Returns Pixel as coord including it's val. For Debugging"""

        return "Pixel at [%s | %s] has value %s" % (self.x, self.field.height - self.y, self.val)


def get_line(x1, y1, x2, y2):
    """Returns all Points on a line from (x1, y1) to (x2, y2)"""

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


class Node(Point):
    """A type of Point to represent Nodes. Each Node is bound to a wall boundary and vise versa"""

    def __init__(self, field, x, y, direction, reachable=True):
        super().__init__(x, y, field)
        """Init"""

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

        self.pos_in_ordered = 0

    def can_see(self, point, excluded_vals=[1]):
        """Returns a boolean weather this Node can see a point. Works best if point is a Node"""

        #  A Node can't see itself
        if self is point:
            return False

        if point is Node:
            if not point.reachable:
                return False

        #  This only increases efficiency if point is a node
        if point not in self.visible_nodes:

            # Get the points on the line between the Node and a point
            points_on_line = get_line(self.x, self.y, point.x, point.y)
            for p in points_on_line:
                if 0 <= p[0] < self.field.width and 0 <= p[1] < self.field.height:
                    # Check if any of the points "blocks" the vision
                    if abs(self.field.img_arr[p[1]][p[0]].val) in excluded_vals:
                        return False

        return True

    def find_visibles(self, nodes, excluded_vals=[1]):
        """Collects all visible Nodes"""

        for node in nodes:

            #  Use inbuild function to determine visibility
            if self.can_see(node, excluded_vals=excluded_vals):
                #  Add as visible to both Nodes
                node.visible_nodes.append(self)
                self.visible_nodes.append(node)

        #  idk why
        return self.visible_nodes

    def find_connections(self, node_struct):
        """Gets the two connection_nodes for a certain Node"""
        """NOTE: This function only works correct if all pixels in the struct are active"""

        print("finding connections for %s" % self)

        #  boundary value
        b_v = self.boundary.val

        #  q (short for queue) used for a non recursiv flood fill
        q = [self.boundary]

        #  To not go back (could've also used node.val but it needs to be reversed afterwards)
        removed = []
        seen_nodes = []

        #  The Idea is that every pixel that has at least one neighbor pixel with
        #  a different value is a pixel on the edge of a struct. This allows to "walk" on the edge
        #  the first pixel which is also a boundary of some node then is the boundary_connection
        #  of this node
        while len(q) > 0 and len(self.boundary_connections) < 2:

            #  It is not good to step over diagonals since nodes can be directly next to each other.
            #  In a case like that doing a diagonal step would mean skipping one of the nodes
            steps = ((1, 0), (-1, 0), (0, 1), (0, -1))
            for step in steps:

                x = q[0].x + step[0]
                y = q[0].y + step[1]

                #  Even if nodes can be out of bounds, this loop does checks pixels so
                #  it is mandatory do a out of bounds check. (Pixels can never be out of bounds)
                if 0 <= x < self.field.width and 0 <= y < self.field.height:

                    #  p is the pixel that has to be checked for validility
                    p = self.field.img_arr[y][x]

                    #  check if wall is a valid connection to a node
                    is_node_connection = False
                    for node in node_struct:
                        if p is node.boundary:
                            print("%s might be a connection" % node)
                            if self.can_see(node, excluded_vals=[b_v]) and node not in seen_nodes:
                                print("%s is a connection" % node)
                                self.boundary_connections.append(node)
                                seen_nodes.append(node)
                                is_node_connection = True

                    if is_node_connection:
                        # print("\t invalid (is node connection)")
                        continue

                    #  check if n is or was in q to prevent double checks
                    #  This is to always walk into one direction.
                    #  Without it there would be an endless loop at some point
                    #  because there can be 2 neighboring pixels that both fulfill every condition
                    if p in q or p in removed:
                        # print("\t invalid (already look at it)")
                        continue

                    #  If the values of the neighbor and the current pixel don't match
                    #  the neighboring pixel can not be walked.
                    if p.val != b_v:
                        # print("\t invalid (value doesn't match)")
                        continue

                    #  check if neighbor has a neighbor that has a different initial value
                    #  A neighbor with a different value means that this pixel is on the outside
                    #  of the current struct
                    has_different_neighbor = False
                    for i in range(p.x - 1, p.x + 2):
                        if has_different_neighbor:
                            break
                        for j in range(p.y - 1, p.y + 2):

                            #  a pixel outside of bounds is counted as a neighbor of other type
                            #  because it basically is
                            if 0 <= i < self.field.width and 0 <= j < self.field.height:
                                if self.field.img_arr[j][i].val != b_v:
                                    has_different_neighbor = True
                                    break
                            else:
                                has_different_neighbor = True
                                break

                    if not has_different_neighbor:
                        # print("\t invalid (has only wall neighbors)")
                        continue

                    #  Every condition was fulfilled so this pixel can be added to the queue
                    q.append(p)

            #  After valid pixels have been appended the last checked pixel has to be removed
            removed.append(q[0])
            q.pop(0)

            """Problems: If 4 nodes surround a single pixel no Node will be added as connection
            """

    def show(self, draw, scale, show_visibles=True, show_boundary_connections=True,
             hue=90, sat=255, lum=255):
        """This function displays the node in the given draw object"""
        random.seed()
        visible_hue = random.randint(0, 255)
        boundary_hue = random.randint(100, 360)

        x_off = scale + int(scale / 2)
        y_off = scale + int(scale / 2)

        #  The upper left and bottom right corner coords of the node itself
        coord = (
            self.x * scale - scale / 2 + x_off, self.y * scale - scale / 2 + y_off,
            self.x * scale + scale / 2 + x_off, self.y * scale + scale / 2 + y_off)

        #  draw a rectangle at the position. Color = green if reachable; red if not
        draw.rectangle(coord, width=scale,
                       fill=(hue * int(self.reachable), sat, lum))

        #  draw a line to it's adjacent wall
        coord = (
            self.x * scale + x_off, self.y * scale + y_off,
            self.boundary.x * scale + x_off, self.boundary.y * scale + y_off)
        #  This line is always green
        draw.line(coord, fill=(60, 255, 255))

        #  draw a number above the node to display it's position in the ordered array

        draw.text((self.x * scale + x_off, self.y * scale - y_off), str(len(self.boundary_connections)), stroke_fill=2,
                  stroke_width=2)

        #  line to visibles
        if show_visibles:
            for visible in self.visible_nodes:
                coord = (
                    self.x * scale + x_off, self.y * scale + + y_off,
                    visible.x * scale + x_off, visible.y * scale + + y_off)
                draw.line(coord, fill=(visible_hue, 255, 255, 50))

        #  line to boundaries
        if show_boundary_connections:
            for boundary in self.boundary_connections:
                coord = (
                    self.x * scale + x_off, self.y * scale + + y_off,
                    boundary.x * scale + x_off, boundary.y * scale + + y_off)
                draw.line(coord, fill=(boundary_hue, 255, 255, 50))

        # points.append((n.x, n.y))

    def __str__(self):
        """Returns Node as coord including how many other nodes this node can see. For Debugging"""

        return "Node at [%s | %s] has %s visible Nodes" % (self.x, self.field.height - self.y, len(self.visible_nodes))


class Collectible(Point):
    def __init__(self, t, x, y, field):
        super().__init__(float(x), float(y), field)
        """A type of Point to represent Collectible points"""

        # x and y are real coordinates in the simulator. Not the coordinates used by the robot
        #  Virtual coordinates are the ones used by the robot
        #  world 1 is 90 cm bigger in both directions (x, y)
        self.virtual_x = -100 * float(x) + self.field.width / 2
        self.virtual_y = 100 * float(y) + self.field.height / 2

        #  The color (red = 0; cyan/green = 1; black = 2)
        self.t = t

    def __str__(self):
        """Returns Collectible as coord including it's color. For Debugging"""
        return "Collectible at [%s | %s] with color %s" % (self.x, self.y, self.t)

    def __repr__(self):
        """Returns Collectible as typical C++ pair coord"""
        return "{%s,%s}" % (self.virtual_x, self.virtual_y)


def main():
    print("setting up...")

    mapData = MapData(img_dirs=[FieldA, FieldB], fd_dirs=FieldFD)
    mapData.show(10)

    print(mapData)


if __name__ == '__main__':
    main()
    exit()
