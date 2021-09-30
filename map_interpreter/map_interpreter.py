import random
import xml.etree.ElementTree as ET
from os import path

from PIL import Image, ImageDraw
from cv2 import resize, imread, INTER_NEAREST
from sys import setrecursionlimit, exit

setrecursionlimit(10000)

grid_size_x = 90
grid_size_y = 60


use_map_directory = True

TestField = "./map/test/Background.bmp"

# Standard CoSpace Directory
FieldA = "C:/CoSpaceRobot Studio/RCJ-2021/RSC/Field/FieldA/Background.bmp"
FieldB = "C:/CoSpaceRobot Studio/RCJ-2021/RSC/Field/FieldB/Background.bmp"
FieldFD = "C:/CoSpaceRobot Studio/RCJ-2021/RSC/Field"

if use_map_directory:
    # map directory
    FieldA = "./map/map0/Background.bmp"
    FieldB = "./map/map1/Background.bmp"
    FieldFD = "./map/Field.FD"


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

def collectible_type_switch(obj_type):
    # format: b, g, r
    switcher = {
        "Object_Red": 0,
        "Object_Green": 1,
        "Object_Black": 2
    }
    return switcher.get(obj_type, 0)


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

#####################
#       points      #
#####################

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


class Collectible():
    def __init__(self, t, x, y, field):
        """A type of Point to represent Collectible points"""

        self.field = field
        self.x = x
        self.y = y

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

    def __init__(self, _dir: str, name: str):
        """A class to collect one single map as Pixels by converting _dir/Background.bmp"""

        global grid_size_x, grid_size_y

        self.correct = False

        # The array all pixels are stored in
        self.img_arr = []

        self.name = name

        # The dimensions of the map
        self.width, self.height = 0, 0

        _dir = _dir if path.isfile(_dir) else _dir + "/Background.bmp"
        if path.isfile(_dir):

            img = imread(path.abspath(_dir))

            x_factor = len(img[0]) / grid_size_x
            y_factor = len(img) / grid_size_y

            # resize the image for less detail but less ram usage and duration
            t_img = resize(img, None, fx=x_factor, fy=y_factor,
                               interpolation=INTER_NEAREST)

            # img = Image.fromarray(t_img)
            # img.show("orig")

            # convert the Image into pixels
            self.img_arr = t_img

            # set the dimensions
            self.width = len(self.img_arr[0])
            self.height = len(self.img_arr)

            self.correct = True
        else:
            print("Can't find image file for \"" + self.name + "\"")

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

    def check_pattern(self, pixel: Pixel, patterns=None, val=1):
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

    def avg(self, pixels: [Pixel]):
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


class MapData:
    def __init__(self, img_files: dict, fd_file: str):
        """A Object to collect all other map objects and also convert them to string or to a picture"""
        print("Creating MapData-Object...")

        self.map_objects = {}
        self.map_objects_nodes = {}
        self.collectibles = {}
        self.img_arrs = {}  # collection of ImageArray objects


        """ In this loop the img_arrays are created and converted """
        for key in img_files.keys():
            file = img_files[key]
            print("Creating ImageArray-Object for %s" % file)

            self.img_arrs[key] = FieldObject(file, key)

            img_arr = self.img_arrs[key]

            if not img_arr.correct:
                self.map_objects[key] = [
                    [],  # walls
                    [],  # traps
                    [],  # swamps
                    [],  # deposits
                    []  # waters
                ]

                self.map_objects_nodes[key] = [
                    [],  # walls
                    [],  # traps
                    []  # swamps
                ]

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

            self.map_objects[key] = temp_map_objects
            self.map_objects_nodes[key] = temp_map_objects_nodes

            for i in range(5):
                if len(temp_map_objects[i]) == 0:
                    print("WARNING... No " + index_switch(i) + " found")
                if i < 3:
                    if len(temp_map_objects_nodes[i]) == 0:
                        print("WARNING... No " + index_switch(i) + " nodes found")

            print("\tfinished")

        if path.isfile(fd_file):
            tree = ET.parse(fd_file)
            root = tree.getroot()
            for key in self.img_arrs.keys():
                world = root.find(key)
                if world:
                    self.collectibles[key] = find_color_points(world, self.img_arrs[key])
                else:
                    self.collectibles[key] = []

            for key in self.collectibles.keys():
                for collectible in self.collectibles[key]:
                    for water in self.map_objects[key][4]:
                        if is_in_area(collectible, water):
                            collectible.is_worth_double = True
                            break

                            # print(self.collectibles)
        else:
            print("File: " + fd_file + " not found")

    def show(self, scale):

        x_off = scale
        y_off = scale

        for key in self.img_arrs.keys():
            img_arr = self.img_arrs[key]
            im = Image.new('HSV', ((img_arr.width + 2) * scale, (img_arr.height + 2) * scale))
            draw = ImageDraw.Draw(im)

            # show the walls
            draw_polygons(draw, self.map_objects[key][0], scale, sat=0, lum=255)

            # show the traps
            draw_polygons(draw, self.map_objects[key][1], scale, starting_hue=40, hue_dif=0)

            # show the swamps
            draw_polygons(draw, self.map_objects[key][2], scale, sat=0, lum=50)

            # show the bonus_areas
            draw_polygons(draw, self.map_objects[key][4], scale, starting_hue=80, hue_dif=0)

            for deposit in self.map_objects[key][3]:
                coord = (
                    deposit[0] * scale - scale / 2 + x_off, deposit[1] * scale - scale / 2 + y_off,
                    deposit[0] * scale + scale / 2 + x_off, deposit[1] * scale + scale / 2 + y_off)
                draw.rectangle(coord, (200, 100, 100), (0, 0, 100))

            for collectible in self.collectibles[key]:
                coord = (
                    collectible.virtual_x * scale - scale / 2 + x_off, collectible.virtual_y * scale - scale / 2 + y_off,
                    collectible.virtual_x * scale + scale / 2 + x_off, collectible.virtual_y * scale + scale / 2 + y_off)
                if collectible.is_worth_double:
                    draw.rectangle(coord, (100, 100, 100), (100, 100, 100))
                else:
                    draw.rectangle(coord, (0, 100, 100), (0, 100, 100))

            im.show("%s" % key)
            im = im.convert(mode="RGB")
            im.save("debugging/%s.png" % key, "png")

    def convert(self, empty=False):
        file_content = ""
        for key in self.img_arrs.keys():
            wall_str = "const std::vector<Area>%sWALLS = " % key  # {{{x1, y1}, {x2, y2}...}, {...}} (Polygon)
            trap_str = "const std::vector<Area>%sTRAPS = " % key  # {{{x1, y1}, {x2, y2}...}, {...}} (Polygon)
            swamp_str = "const std::vector<Area>%sSWAMPS = " % key  # {{x1, y1}, {x2, y2}...} (Polygon)
            bonus_area_str = "const std::vector<Area>%sWATERS = " % key  # {{x1, y1}, {x2, y2}...} (Polygon)
            deposit_area_str = "const std::vector<PVector>%sDEPOSITS = " % key  # {{x1, y1}, {x2, y2}...} (Single points)
            wall_nodes_str = "const std::vector<PVector>%sWALLNODES = " % key  # {{x1, y1}, {x2, y2}...} (Single points)
            trap_nodes_str = "const std::vector<PVector>%sTRAPNODES = " % key  # {{x1, y1}, {x2, y2}...} (Single points)
            swamp_nodes_str = "const std::vector<PVector>%sSWAMPNODES = " % key  # {{x1, y1}, {x2, y2}...} (Single points)
            collectibles_str = "const std::vector<Collectible> %sCOLLECTIBLES = " % key
            if not empty:
                wall_str += convert_arr_to_area_vector_string(self.map_objects[key][0])
                trap_str += convert_arr_to_area_vector_string(self.map_objects[key][1])
                swamp_str += convert_arr_to_area_vector_string(self.map_objects[key][2])
                deposit_area_str += str(self.map_objects[key][3]).replace("[", "{").replace("]", "}").replace(" ", "") + ";"
                bonus_area_str += convert_arr_to_area_vector_string(self.map_objects[key][4])

                wall_nodes_str += str(self.map_objects_nodes[key][0]).replace("[", "{").replace("]", "}").replace(" ",
                                                                                                                "") + ";"
                trap_nodes_str += str(self.map_objects_nodes[key][1]).replace("[", "{").replace("]", "}").replace(" ",
                                                                                                                "") + ";"
                swamp_nodes_str += str(self.map_objects_nodes[key][2]).replace("[", "{").replace("]", "}").replace(" ",
                                                                                                                 "") + ";"

                collectibles_str += str(self.collectibles[key]).replace("[", "{").replace("]", "}").replace(" ", "") + ";"
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

            file_content += "//------------- %s_Objects --------------//\n\n" % key

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

        return file_content

    def __str__(self):
        return self.convert()


def main():
    print("setting up...")

    # mapData = MapData(img_dirs=["debugging"], fd_dirs=FieldFD)
    mapData = MapData(img_files={"GAME0": FieldA, "GAME1": FieldB}, fd_file=FieldFD)

    begin = "\n\n\n" \
            "///   _______                _____          __\n" \
            "///  |   |   |.---.-..-----.|     \ .---.-.|  |_ .---.-.\n" \
            "///  |       ||  _  ||  _  ||  --  ||  _  ||   _||  _  |\n" \
            "///  |__|_|__||___._||   __||_____/ |___._||____||___._|\n" \
            "///                  |__|\n"

    with open("../code/MapData.cpp") as f:
        file = f.read().split(begin)

    mapData_str = begin + "\n #pragma region MapData\n\n\n\n" + mapData.convert(empty=False) + "\n #pragma endregion\n";
    if len(file) == 2:
        file[1] = mapData_str
    else:
        file.append(mapData_str)

    file_str = "".join(file)

    with open("../code/MapData.cpp", "w") as f:
        f.write(file_str)
        pass


if __name__ == '__main__':
    main()
    exit()
