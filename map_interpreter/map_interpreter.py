import random
import xml.etree.ElementTree as ET
from os import path

from PIL import Image, ImageDraw
from cv2 import resize, imread, INTER_NEAREST
from sys import setrecursionlimit, exit

setrecursionlimit(10000)

# 1 is max detail
detail = 0.5

# for old CoSpace Versions
FieldA = "C:/CoSpaceRobot Studio/RCJ-2021/RSC/Field/FieldA"
FieldB = "C:/CoSpaceRobot Studio/RCJ-2021/RSC/Field/FieldB"
FieldFD = "C:/CoSpaceRobot Studio/RCJ-2021/RSC/Field"


def is_left(p0, p1, p2):
    return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y) > 0


def is_in_area(p_, area):
    p = Point(p_.field, p_.virtual_x, p_.virtual_y)

    wn = 0
    n = len(area)

    poly = area.copy()
    poly.append(area[0])

    for i in range(n):
        if poly[i].y <= p.y:
            if poly[i + 1].y > p.y:
                if is_left(poly[i], poly[i + 1], p):
                    wn += 1
        else:
            if poly[i + 1].y <= p.y:
                if not is_left(poly[i], poly[i + 1], p):
                    wn -= 1
    return wn != 0


def convert_arr_to_area_vector_string(arr):
    if len(arr) == 0:
        return "{};"
    return "{\n\t" + str(arr).replace("[", "{").replace("]", "}").replace(" ", "").replace("{{{", "{{") \
        .replace("}},", "}}),\n\t").replace("{{", "Area({{").replace("}}}", "}})}") + ";"


def collectible_type_switch(obj_type):
    # format: b, g, r
    switcher = {
        "Object_Red": 0,
        "Object_Green": 1,
        "Object_Black": 2
    }
    return switcher.get(obj_type, 0)


def index_switch(index):
    switcher = {
        0: "walls",
        1: "traps",
        2: "swamps",
        3: "deposits",
        4: "waters"
    }
    return switcher.get(index, 0)


def find_color_points(world, field):
    arr = []

    field_obj_collection = world.find("FieldObjects")
    field_objs = field_obj_collection.findall("FOBJ")

    for fobj in field_objs:
        obj_type = fobj.find("ObjType").text
        if obj_type in ["Object_Black", "Object_Red", "Object_Green"]:
            obj_type = collectible_type_switch(obj_type)
            cx = float(fobj.find("CX").text)
            cy = float(fobj.find("CY").text)
            arr.append(Collectible(obj_type, cx, cy, field))

    return arr


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


#####################
#       points      #
#####################

class Point:

    def __init__(self, field, x, y):
        """A simple Point in a 2D grid"""
        # coords
        self.x = x
        self.y = y

        #  The object where this Point is saved in
        self.field = field

    def __repr__(self):
        """Returns Point as typical C++ pair coord"""
        return "{%s,%s}" % (self.x, self.field.height - self.y)

    def __str__(self):
        return "[%s | %s]" % (self.x, self.field.height - self.y)


def color_switch(color):
    # format: b, g, r
    switcher = {
        tuple([0, 0, 0]): 1,  # wall
        tuple([151, 186, 221]): 1,
        tuple([131, 177, 244]): 1,
        tuple([0, 255, 255]): 0,  # trap (only use the blue in the middle)
        tuple([151, 85, 47]): 2,
        tuple([196, 114, 68]): 2,
        tuple([166, 166, 166]): 3,  # swamp
        tuple([0, 153, 255]): 4,  # deposit
        tuple([0, 102, 255]): 4,
        tuple([240, 176, 0]): 5  # water
    }
    return switcher.get(color, 0)


class Pixel(Point):
    """A type of point used to represent the actual image"""

    def __init__(self, field, x, y, color):
        super().__init__(field, x, y)

        #  value of the Pixel (easier to handle than rgb colors)
        self.val = color_switch(tuple([color[0], color[1], color[2]]))

    def __str__(self):
        """Returns Pixel as coord including it's val"""

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


class Boundary(Point):
    """A type of Point to represent corners. Each corner is bound to a wall boundary and vise versa"""

    def __init__(self, field, match: []):
        super().__init__(field, match[0].x, match[0].y)

        self.match = match
        self.connections = []  # max 2

    def find_connections(self, corners):

        # NOTE: (x, y) rotated around (0, 0) by 90° (clockwise) becomes (y, -x)

        #  value (color)
        val = self.field.img_arr[self.y][self.x].val

        connections = (self.match[0], self.match[0])

        direction_x, direction_y = self.match[2].x - self.x, self.match[2].y - self.y

        side = -1  # 1 = hug right side, -1 = hug left side

        if self.match[1].x - self.x != self.match[1].y - self.y:
            side = 1

        for connection in connections:

            x, y = self.x, self.y
            connection_found = False

            while not connection_found:

                # coords in front of the current position
                front_x, front_y = x + direction_x, y + direction_y

                # the pixel in front of the last checked pixel --> pixel that is now checked
                p = self.field.get_pixel(front_x, front_y)
                if p and p.val == val:  # if p is a valid next step

                    # check if p is a corner
                    for corner in corners:
                        if corner is not self:
                            if p is corner.match[0]:
                                self.connections.append(corner)
                                connection_found = True
                                break

                    # clockwise turn if side == 1 otherwise counter clockwise
                    # the pixel on one side of the current pixel
                    direction_x, direction_y = side * direction_y, -side * direction_x

                    x, y = front_x, front_y

                else:
                    #  counter-clockwise turn (if side = 1)
                    direction_x, direction_y = -side * direction_y, side * direction_x

            #  for the other connection
            direction_x, direction_y = self.match[3].x - self.x, self.match[3].y - self.y
            side *= -1

    def show(self, draw, scale, show_connections=True,
             hue=90, sat=255, lum=255):
        """This function displays the corner in the given draw object"""
        random.seed()
        visible_hue = random.randint(0, 255)
        boundary_hue = random.randint(100, 360)

        x_off = scale + int(scale / 2)
        y_off = scale + int(scale / 2)

        #  The upper left and bottom right corner coords of the corner itself
        coord = (
            self.x * scale - scale / 2 + x_off, self.y * scale - scale / 2 + y_off,
            self.x * scale + scale / 2 + x_off, self.y * scale + scale / 2 + y_off)

        #  draw a rectangle at the position. Color = green if reachable; red if not
        draw.rectangle(coord, width=scale,
                       fill=(hue, sat, lum))
        #  draw a number above the corner to display it's position in the ordered array

        draw.text((self.x * scale + x_off, self.y * scale - y_off), str(len(self.connections)), stroke_fill=2,
                  stroke_width=2)

        #  line to boundaries
        if show_connections:
            for connection in self.connections:
                coord = (
                    self.x * scale + x_off, self.y * scale + + y_off,
                    connection.x * scale + x_off, connection.y * scale + + y_off)
                draw.line(coord, fill=(boundary_hue, 255, 255, 50))

            #  line to boundaries
            if show_connections:
                for match in self.match:
                    coord = (
                        self.x * scale + x_off, self.y * scale + + y_off,
                        match.x * scale + x_off, match.y * scale + + y_off)
                    draw.line(coord, fill=(boundary_hue, 255, 255, 100))

    def __str__(self):
        """Returns corner as coord including how many other corners this corner can see. For Debugging"""

        return "Boundary at [%s | %s]" % (self.x, self.field.height - self.y)


class Collectible(Point):
    def __init__(self, t, x, y, field):
        super().__init__(field, float(x), float(y))
        """A type of Point to represent Collectible points"""

        # x and y are real coordinates in the simulator. Not the coordinates used by the robot
        #  Virtual coordinates are the ones used by the robot
        #  world 1 is 90 cm bigger in both directions (x, y)
        self.virtual_x = round(-100 * float(x) + field.width / 2)
        self.virtual_y = round(100 * float(y) + field.height / 2)

        #  The color (red = 0; cyan/green = 1; black = 2)
        self.t = t

        self.is_worth_double = False

    def __str__(self):
        """Returns Collectible as coord including it's color. For Debugging"""
        return "Collectible at [%s | %s] with color %s" % (self.x, self.y, self.t)

    def __repr__(self):
        """Returns Collectible as typical C++ pair coord"""
        return "{{%s,%s},%s,%s}" % (self.virtual_x, self.virtual_y, self.t, self.is_worth_double)


###########################
#       FieldObject       #
###########################

class FieldObject:

    def __init__(self, _dir):
        """A class to collect one single map as Pixels by converting _dir/Background.bmp"""

        global detail

        # The array all pixels are stored in
        self.img_arr = []

        # The dimensions of the map
        self.width, self.height = 0, 0

        # if _dir doesn't exist the user has to change the input files
        if path.isdir(_dir):

            # resize the image for less detail but less ram usage and duration
            t_img = None
            if path.isfile(_dir + "\Background(complete).bmp"):
                t_img = resize(imread(_dir + "/Background(complete).bmp"), None, fx=detail, fy=detail,
                               interpolation=INTER_NEAREST)
            else:
                t_img = resize(imread(_dir + "/Background.bmp"), None, fx=detail, fy=detail,
                               interpolation=INTER_NEAREST)

            # img = Image.fromarray(t_img)
            # img.show("orig")

            # convert the Image into pixels
            self.img_arr = [[Pixel(self, i, j, t_img[j][i]) for i in range(len(t_img[j]))] for j in
                            range(len(t_img))]

            # set the dimensions
            self.width = len(self.img_arr[0])
            self.height = len(self.img_arr)
        else:
            # Error
            print("Path \"" + _dir + "\" does not exist")

    def get_pixel(self, x, y):
        if 0 <= x < self.width and 0 <= y < self.height:
            return self.img_arr[y][x]
        return None

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

    def avg(self, pixels):
        """This function calculates the average point of all adjacent Pixels to the Pixel at i, j"""

        # Calculate the average
        if len(pixels) > 0:
            x = 0
            y = 0
            for n in pixels:
                x += n.x
                y += n.y

            # also return the pixels if they need to be deactivated
            return [int(x / len(pixels)), int(y / len(pixels))]

        return None

    def __str__(self):
        s = ""
        for row in self.img_arr:
            s += str(row) + "\n"
        return s


def get_corners(field: FieldObject, pixels: [Pixel]):
    corners = []

    #  collect all possible corners by pattern recognition
    for pixel in pixels:
        matches = field.check_pattern(pixel, val=pixel.val)
        for match in matches:
            corners.append(Boundary(field, match))

        #  collect all corners that are unescessary
    corners_to_delete = []
    checking_neighbour_steps = [[1, 1], [-1, 1]]
    for corner in corners:
        for step in checking_neighbour_steps:
            neighbour_count = 0
            for c_corner in corners:
                if c_corner.x == corner.x + step[0] and c_corner.y == corner.y + step[1]:
                    neighbour_count += 1
                elif c_corner.x == corner.x - step[0] and c_corner.y == corner.y - step[1]:
                    neighbour_count += 1

                if neighbour_count >= 2:
                    corners_to_delete.append(corner)

    #  delete them
    for corner in corners_to_delete:
        if corner in corners:
            corners.remove(corner)

    for corner in corners:
        corner.find_connections(corners)

    return corners


def order_corners(corners):
    """Returns an array of corners in order, so that "neighboring" corners in the array represent a wall connection"""

    ordered = []

    #  corners are already ordered by structs
    for corner_struct in corners:

        #  print("ordering cornerstruct: " + str(corner_struct))

        #  open_corners are all unsorted corners
        open_corners = corner_struct

        #  corners should be ordered in structs afterwards too
        ordered.append([])

        #  n is the corner that is currently checked
        n = open_corners[0]

        #  the first corner that is checked is alays at the front
        ordered[len(ordered) - 1].append(n)
        open_corners.pop(0)

        #  as long as there are corners to be sorted
        while len(open_corners) > 0:

            # print("len of open_corners: " + str(len(open_corners)))
            # print("len of boundaries: " + str(len(n.connections)))

            #  loop over the wall_connections and get the connection that is not already ordered
            for boundary_connection in n.connections:
                #  there should always be at least one boundary_connection in open_corners
                if boundary_connection in open_corners:
                    # print(boundary_connection)
                    #  the boundary_connection in open_corners is the new corner that has to be checked.
                    n = boundary_connection

            #  add last add boundary to ordered and remove it from open_corners
            open_corners.remove(n)
            ordered[len(ordered) - 1].append(n)

            # print("open corners (after remove(n)): " + str(open_corners))

    return ordered


#################
#    MapData    #
#################


class MapData:
    def __init__(self, img_dirs, fd_dir):
        """A Object to collect all other map objects and also convert them to string or to a picture"""
        print("Creating MapData-Object...")

        self.map_objects = []
        self.map_objects_nodes = []

        self.collectibles = []

        self.img_arrs = []  # collection of ImageArray objects

        """ In this loop the img_arrays are created and converted """
        for _dir in img_dirs:
            print("Creating ImageArray-Object for %s" % _dir)

            self.img_arrs.append(FieldObject(_dir))
            world_index = len(self.img_arrs) - 1
            img_arr = self.img_arrs[world_index]

            print("\tConverting ImageArray...")

            img_arr.expand_all(1, int(detail * 2))  # standard 16
            img_arr.expand_all(2, int(detail * 2))
            img_arr.expand_all(3, int(detail * 2))
            img_arr.expand_all(5, int(detail * 2))

            temp_map_objects = [
                [],  # walls
                [],  # traps
                [],  # swamps
                [],  # deposits
                []  # waters
            ]

            temp_map_objects_nodes = [
                [],  # walls
                [],  # traps
                []  # swamps
            ]

            for j in range(img_arr.height):
                for i in range(img_arr.width):
                    p = img_arr.img_arr[j][i]
                    if p.val <= 0:
                        pass
                    else:
                        temp_map_objects[p.val - 1].append(img_arr.flood_fill(i, j, p.val, -p.val, store=True))

            #  activate all pixels again
            for j in range(img_arr.height):
                for i in range(img_arr.width):
                    img_arr.img_arr[j][i].val = abs(img_arr.img_arr[j][i].val)

            print("\tCreating Polygons...")

            for i in range(0, len(temp_map_objects)):
                if i != 3:
                    object_type = temp_map_objects[i]
                    corners = []
                    for object_piece in object_type:
                        corners.append(get_corners(img_arr, object_piece))
                    temp_map_objects[i] = order_corners(corners)
                else:
                    temp_map_objects[3] = [img_arr.avg(deposit) for deposit in temp_map_objects[3]]

            for i in [0, 1, 2]:
                for obj in temp_map_objects[i]:
                    for c in obj:
                        if c.match[1].field:
                            temp_map_objects_nodes[i].append(c.match[1])

            self.map_objects.append(temp_map_objects)
            self.map_objects_nodes.append(temp_map_objects_nodes)

            for i in range(5):
                if len(temp_map_objects[i]) == 0:
                    print("WARNING... No " + index_switch(i) + " found")
                if i < 3:
                    if len(temp_map_objects_nodes[i]) == 0:
                        print("WARNING... No " + index_switch(i) + " nodes found")

            print("\tfinished")

        if path.isfile(fd_dir + "/Field.FD"):
            tree = ET.parse(fd_dir + "/Field.FD")
            root = tree.getroot()
            w1 = root.find("World1")
            w2 = root.find("World2")
            self.collectibles.append(find_color_points(w1, self.img_arrs[0]))
            self.collectibles.append(find_color_points(w2, self.img_arrs[1]))

            for i in range(2):
                for collectible in self.collectibles[i]:
                    for water in self.map_objects[i][4]:
                        if is_in_area(collectible, water):
                            collectible.is_worth_double = True
                            break

                            # print(self.collectibles)
        else:
            print("File: " + fd_dir + "/Field.FD not found")

    def show(self, scale):

        x_off = scale
        y_off = scale

        i = 0
        for img_arr in self.img_arrs:
            im = Image.new('HSV', ((img_arr.width + 2) * scale, (img_arr.height + 2) * scale))
            draw = ImageDraw.Draw(im)

            # show the walls
            draw_polygons(draw, self.map_objects[i][0], scale, sat=0, lum=255)

            # show the traps
            draw_polygons(draw, self.map_objects[i][1], scale, starting_hue=40, hue_dif=0)

            # show the swamps
            draw_polygons(draw, self.map_objects[i][2], scale, sat=0, lum=50)

            # show the bonus_areas
            draw_polygons(draw, self.map_objects[i][4], scale, starting_hue=80, hue_dif=0)

            for deposit in self.map_objects[i][3]:
                coord = (
                    deposit[0] * scale - scale / 2 + x_off, deposit[1] * scale - scale / 2 + y_off,
                    deposit[0] * scale + scale / 2 + x_off, deposit[1] * scale + scale / 2 + y_off)
                draw.rectangle(coord, (200, 100, 100), (0, 0, 100))

            for collectible in self.collectibles[i]:
                coord = (
                    collectible.virtual_x * scale - scale / 2 + x_off, collectible.virtual_y * scale - scale / 2 + y_off,
                    collectible.virtual_x * scale + scale / 2 + x_off, collectible.virtual_y * scale + scale / 2 + y_off)
                if collectible.is_worth_double:
                    draw.rectangle(coord, (100, 100, 100), (100, 100, 100))
                else:
                    draw.rectangle(coord, (0, 100, 100), (0, 100, 100))

            im.show("Map%s" % i)
            im = im.convert(mode="RGB")
            im.save("debugging/Map%s.png" % i, "png")
            i += 1

    def convert(self, empty=False):
        file_content = ""
        i = 0
        for img_arr in self.img_arrs:
            wall_str = "const std::vector<Area>GAME%sWALLS = " % i  # {{{x1, y1}, {x2, y2}...}, {...}} (Polygon)
            trap_str = "const std::vector<Area>GAME%sTRAPS = " % i  # {{{x1, y1}, {x2, y2}...}, {...}} (Polygon)
            swamp_str = "const std::vector<Area>GAME%sSWAMPS = " % i  # {{x1, y1}, {x2, y2}...} (Polygon)
            bonus_area_str = "const std::vector<Area>GAME%sWATERS = " % i  # {{x1, y1}, {x2, y2}...} (Polygon)
            deposit_area_str = "const std::vector<PVector>GAME%sDEPOSITS = " % i  # {{x1, y1}, {x2, y2}...} (Single points)
            wall_nodes_str = "const std::vector<PVector>GAME%sWALLNODES = " % i  # {{x1, y1}, {x2, y2}...} (Single points)
            trap_nodes_str = "const std::vector<PVector>GAME%sTRAPNODES = " % i  # {{x1, y1}, {x2, y2}...} (Single points)
            swamp_nodes_str = "const std::vector<PVector>GAME%sSWAMPNODES = " % i  # {{x1, y1}, {x2, y2}...} (Single points)
            collectibles_str = "const std::vector<Collectible> GAME%sCOLLECTIBLES = " % i
            if not empty:
                wall_str += convert_arr_to_area_vector_string(self.map_objects[i][0])
                trap_str += convert_arr_to_area_vector_string(self.map_objects[i][1])
                swamp_str += convert_arr_to_area_vector_string(self.map_objects[i][2])
                deposit_area_str += str(self.map_objects[i][3]).replace("[", "{").replace("]", "}").replace(" ", "") + ";"
                bonus_area_str += convert_arr_to_area_vector_string(self.map_objects[i][4])

                wall_nodes_str += str(self.map_objects_nodes[i][0]).replace("[", "{").replace("]", "}").replace(" ",
                                                                                                                "") + ";"
                trap_nodes_str += str(self.map_objects_nodes[i][1]).replace("[", "{").replace("]", "}").replace(" ",
                                                                                                                "") + ";"
                swamp_nodes_str += str(self.map_objects_nodes[i][2]).replace("[", "{").replace("]", "}").replace(" ",
                                                                                                                 "") + ";"

                collectibles_str += str(self.collectibles[i]).replace("[", "{").replace("]", "}").replace(" ", "") + ";"
                collectibles_str = collectibles_str.replace("False", "false").replace("True", "true")
            else:
                wall_str += "{};"
                trap_str += "{};"
                swamp_str += "{};"
                bonus_area_str += "{};"
                deposit_area_str += "{};"
                wall_nodes_str += "{};"
                trap_nodes_str += "{};"
                swamp_nodes_str += "{};"
                collectibles_str += "{};"

            file_content += "//------------- Game%s_Objects --------------//\n\n" % i

            file_content += "\t\t/*walls*/\n" + wall_str + \
                            "\n\t\t/*traps*/\n" + trap_str + \
                            "\n\t\t/*swamps*/\n" + swamp_str + \
                            "\n\t\t/*Water*/\n" + bonus_area_str + \
                            "\n\t\t/*deposit*/\n" + deposit_area_str

            file_content += "\n\n\t//------ Nodes ------//\n"

            file_content += "\t\t/*wall_nodes*/\n" + wall_nodes_str + \
                            "\n\t\t/*trap_nodes*/\n" + trap_nodes_str + \
                            "\n\t\t/*swamp_nodes*/\n" + swamp_nodes_str + "\n\n"

            file_content += "\n\n\t//------ Collectibles ------//\n"

            file_content += "\t\t/*collectibles*/\n" + collectibles_str + "\n\n"

            i += 1

        return file_content

    def __str__(self):
        return self.convert()


def main():
    print("setting up...")

    # mapData = MapData(img_dirs=["debugging"], fd_dirs=FieldFD)
    mapData = MapData(img_dirs=[FieldA, FieldB], fd_dir=FieldFD)
    # mapData.show(10)


    begin = "\n\n\n" \
            "///   _______                _____          __\n" \
            "///  |   |   |.---.-..-----.|     \ .---.-.|  |_ .---.-.\n" \
            "///  |       ||  _  ||  _  ||  --  ||  _  ||   _||  _  |\n" \
            "///  |__|_|__||___._||   __||_____/ |___._||____||___._|\n" \
            "///                  |__|\n"

    with open("../code/MapData.cpp") as f:
        file = f.read().split(begin)

    mapData_str = begin + "\n #pragma region MapData\n\n\n\n" + mapData.convert(empty=True) + "\n #pragma endregion\n";
    if len(file) == 2:
        file[1] = mapData_str
    else:
        file.append(mapData_str)

    file_str = "".join(file)

    with open("../code/MapData.cpp", "w") as f:
        f.write(file_str)


if __name__ == '__main__':
    main()
    exit()
