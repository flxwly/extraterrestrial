import xml.etree.ElementTree as ET
from os import path

import array
import math
import numpy as np
from PIL import Image, ImageDraw
from cv2 import resize, imread, INTER_NEAREST, INTER_AREA, INTER_LINEAR_EXACT
from sys import setrecursionlimit, exit

setrecursionlimit(10000)

use_map_directory = True

TestField = "./map/test/Background.bmp"

# Standard CoSpace Directory
FieldA = "C:/CoSpaceRobot Studio/RCJ-2021/RSC/Field/FieldA/Background.bmp"
FieldB = "C:/CoSpaceRobot Studio/RCJ-2021/RSC/Field/FieldB/Background.bmp"
FieldFD = "C:/CoSpaceRobot Studio/RCJ-2021/RSC/Field"

# Maptiles
empty = 'E'
wall = 'W'
trap = 'T'
swamp = 'S'
temp_wall = 'w'
unknown = 'U'

if use_map_directory:
    # map directory
    FieldA = "./map/map0/Background.bmp"
    FieldB = "./map/map1/Background.bmp"
    FieldFD = "./map/Field.FD"


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


def rev_color_switch(color):
    # format: b, g, r
    switcher = {
        1: tuple([131, 177, 244]),
        2: tuple([151, 85, 47]),
        3: tuple([166, 166, 166]),  # swamp
        4: tuple([0, 153, 255]),  # deposit
        5: tuple([240, 176, 0])  # water
    }
    return switcher.get(color, tuple([0, 0, 0]))


def char_switch(num):
    # format: b, g, r
    switcher = {
        0: empty,
        1: wall,
        2: trap,
        3: swamp,  # swamp
        4: empty,  # deposit
        5: empty  # water
    }
    return switcher.get(num, unknown)


def collectible_color_switch(color):
    switcher = {
        0: tuple([255, 0, 0]),
        1: tuple([0, 255, 255]),
        2: tuple([255, 255, 100]),  # "black" points
    }
    return switcher.get(color, tuple([0, 0, 0]))


def round_dec(x, n):
    return math.floor(x * pow(10, n)) / pow(10, n)


class Collectible:
    def __init__(self, color: int, x: float, y: float, field):
        """A type of Point to represent Collectible points"""

        self.field = field

        self.x = x
        self.y = y

        # x and y are real coordinates in the simulator. Not the coordinates used by the robot
        #  Virtual coordinates are the ones used by the robot
        #  world 1 is 90 cm bigger in both directions (x, y)

        self.virtual_x = -100 * x * 2 * field.scale_x + (field.width / 2)
        self.virtual_y = 100 * y * 2 * field.scale_y + (field.height / 2)

        #  The color (red = 0; cyan/green = 1; black = 2)
        self.color = color

        self.is_worth_double = False

    def __str__(self):
        """Returns Collectible as coord including it's color. For Debugging"""
        return "Collectible at [%s | %s] with color %s" % (self.virtual_x, self.virtual_y, self.color)

    def __repr__(self):
        """Returns Collectible as typical C++ pair coord"""
        return "{{" + f"{round_dec(self.virtual_x, 2)}, {round_dec(self.virtual_y, 2)}" + "}," + \
               f"{self.color}, {self.is_worth_double}" + "}"


###########################
#       FieldObject       #
###########################

def avg(pixels: [int]):
    """This function calculates the average point of all adjacent Pixels to the Pixel at i, j"""

    # Calculate the average
    if len(pixels) > 0:
        x = 0
        y = 0
        for n in pixels:
            x += n[0]
            y += n[1]

        # also return the pixels if they need to be deactivated
        return [int(x / len(pixels)), int(y / len(pixels))]

    return None


class FieldObject:
    def __init__(self, _dir: str, name: str, grid_size: [int, int]):
        """A class to collect one single map as Pixels by converting _dir/Background.bmp"""

        self.map = []

        self.name = name

        # The dimensions of the map
        self.width, self.height = 0, 0
        self.scale_x, self.scale_y = 0, 0

        _dir = _dir if path.isfile(_dir) else _dir + "/Background.bmp"
        if path.isfile(_dir):

            img = imread(path.abspath(_dir))

            self.scale_x = grid_size[0] / len(img[0])
            self.scale_y = grid_size[1] / len(img)

            # resize the image for less detail but less ram usage and duration
            img = resize(img, None, fx=self.scale_x, fy=self.scale_y, interpolation=INTER_NEAREST)

            self.map = [[color_switch(tuple([img[j][i][0], img[j][i][1], img[j][i][2]])) for i in range(len(img[j]))]
                        for j in range(len(img))]

            self.width = len(self.map[0])
            self.height = len(self.map)

            self.expand_all(1, 5)  # standard 16
            self.expand_all(2, 6)
            self.expand_all(3, 6)




        else:
            print("Can't find image file for \"" + self.name + "\"")

    def flood_fill(self, i, j, old_val, new_val, store=False):
        """This method is used to fill a region of Pixels with same value.
            It can also be used to just store them in an array by setting an arr"""

        #  Normal recursive 8-way floodfill
        if not store:
            #  Check if pixel.val equals the starting val
            if self.map[j][i] == old_val:

                #  Replace the value
                self.map[j][i] = new_val

                #  invoke on all neighboring pixels with same parameters but different coords
                for n in range(j - 1 if j - 1 >= 0 else 0, j + 2 if j + 1 < self.height else self.height):
                    for m in range(i - 1 if i - 1 >= 0 else 0, i + 2 if i + 1 < self.width else self.width):
                        #  store=False is standard and therefore doesn't have to be set
                        self.flood_fill(m, n, old_val, new_val)

        #  non recursive 8-way flood-fill
        else:

            #  q (short for queue) used for a non recursiv flood fill
            q = [[i, j]]

            #  To avoid setting values go back
            stored = []

            while len(q) > 0:

                #  for each neighbor (8x8)
                for n in range(q[0][1] - 1 if q[0][1] - 1 >= 0 else 0,
                               q[0][1] + 2 if q[0][1] + 1 < self.height else self.height):
                    for m in range(q[0][0] - 1 if q[0][0] - 1 >= 0 else 0,
                                   q[0][0] + 2 if q[0][0] + 1 < self.width else self.width):

                        #  p is the pixel that might be added
                        p = [m, n]
                        #  print("Checking if %s is valid" % n)

                        #  check if n is or was in q to prevent double checks
                        if p in q or p in stored:
                            # print("\t invalid (already look at it)")
                            continue

                        #  check if the values match
                        if self.map[n][m] != old_val:
                            # print("\t invalid (value doesn't match)")
                            continue

                        #  The pixels value is updated
                        self.map[n][m] = new_val

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
        old_val = self.map[j][i]
        return self.flood_fill(i, j, old_val, old_val, store=True)

    def expand_all(self, val, r):
        """This function expands every pixel of a certain type by r"""

        pixels_to_expand = []

        for j in range(self.height):
            for i in range(self.width):
                if self.map[j][i] == val:
                    pixels_to_expand.extend(self.flood_fill(i, j, val, -val, store=True))

        for p in pixels_to_expand:
            # set val in radius r
            for n in range(p[1] - r if p[1] - r >= 0 else 0, p[1] + r + 1 if p[1] + r < self.height else self.height):
                for m in range(p[0] - r if p[0] - r >= 0 else 0, p[0] + r + 1 if p[0] + r < self.width else self.width):
                    if math.sqrt((n - p[1]) * (n - p[1]) + (m - p[0]) * (m - p[0])) <= r:

                        # disable expanding over walls
                        if self.map[n][m] != 1:
                            self.map[n][m] = val

    def __str__(self):
        s = ""
        for row in self.map:
            s += str(row) + "\n"
        return s


class MapData:
    def __init__(self, img_files: dict, fd_file: str):
        """A Object to collect all other map objects and also convert them to string or to a picture"""
        print("Creating MapData-Object...")

        self.collectibles = {}
        self.field_objects = {}
        self.deposits = {}

        """ In this loop the img_arrays are created and converted """
        for key in img_files.keys():
            file = img_files[key][0]
            print("Creating ImageArray-Object for %s" % file)

            self.field_objects[key] = FieldObject(file, key, img_files[key][1])

            self.deposits[key] = []

            for j in range(len(self.field_objects[key].map)):
                for i in range(len(self.field_objects[key].map[j])):
                    if self.field_objects[key].map[j][i] == 4:
                        self.deposits[key].append(avg(self.field_objects[key].flood_fill(i, j, 4, -4, store=True)))

            if path.isfile(fd_file):
                tree = ET.parse(fd_file)
                root = tree.getroot()
                world = root.find(key)
                if world:
                    self.collectibles[key] = find_color_points(world, self.field_objects[key])
                else:
                    self.collectibles[key] = []

                to_remove = []
                for c in self.collectibles[key]:
                    # print(self.field_objects[key].map[round(collectible.virtual_y)][round(collectible.virtual_x)])
                    if self.field_objects[key].map[round(c.virtual_y)][round(c.virtual_x)] == 1 or \
                            self.field_objects[key].map[round(c.virtual_y)][
                                round(c.virtual_x)] == 2:
                        # print("Remove" + str(collectible))
                        to_remove.append(c)

                    elif self.field_objects[key].map[round(c.virtual_y)][round(c.virtual_x)] == 5:
                        # print("Make double" + str(collectible))
                        c.is_worth_double = True

                for c in to_remove:
                    self.collectibles[key].remove(c)

            else:
                print("File: " + fd_file + " not found")


    def show(self, scale):
        for key, img in self.getImages(scale):
            img.show(f"{key}")

    def save(self, scale):
        for key, img in self.getImages(scale).items():
            img.save(f"debugging/{key}.png", "png")

    def getImages(self, scale):
        x_off = scale
        y_off = scale

        imgs = {}

        for key in self.field_objects.keys():
            field_object = self.field_objects[key]
            imgs[key] = Image.new('RGB', ((field_object.width + 2) * scale, (field_object.height + 2) * scale))
            draw = ImageDraw.Draw(imgs[key])

            for j in range(field_object.height):
                for i in range(field_object.width):
                    draw.rectangle((i * scale, j * scale, i * scale + x_off, j * scale + y_off),
                                   rev_color_switch(field_object.map[j][i]), (0, 0, 0))

            for c in self.collectibles[key]:
                draw.rectangle((c.virtual_x * scale, c.virtual_y * scale,
                                c.virtual_x * scale + x_off,
                                c.virtual_y * scale + y_off), collectible_color_switch(c.color),
                               (0, 255, 0) if c.is_worth_double else (0, 0, 0))

            imgs[key] = imgs[key].convert(mode="RGB")

        return imgs

    def convert(self, empty=False):
        cpp_content = ""
        hpp_content = ""
        for key in self.field_objects.keys():

            externs = f"extern const std::vector<PVector> {key}DEPOSITS;\n" \
                      f"extern const int {key}MAP_WIDTH;\n" \
                      f"extern const int {key}MAP_HEIGHT;\n" \
                      f"extern const std::string {key}MAP;\n" \
                      f"extern const std::vector<Collectible> {key}COLLECTIBLES;\n"

            deposit_str = f"const std::vector<PVector> {key}DEPOSITS = "
            map_str = f"const int {key}MAP_WIDTH = {self.field_objects[key].width}; \n" \
                      f"const int {key}MAP_HEIGHT = {self.field_objects[key].height}; \n" \
                      f"const std::string {key}MAP = \""
            collectibles_str = f"const std::vector<Collectible> {key}COLLECTIBLES = "
            if not empty:
                deposit_str += str(self.deposits[key]).replace("[", "{").replace("]", "}").replace(" ", "") + ";"

                # map
                for j in range(len(self.field_objects[key].map)):
                    for i in range(len(self.field_objects[key].map[j])):
                        map_str += char_switch(self.field_objects[key].map[j][i])
                map_str += "\";"

                collectibles_str += str(self.collectibles[key]).replace("[", "{").replace("]", "}").replace(" ",
                                                                                                            "") + ";"
                collectibles_str = collectibles_str.replace("False", "false").replace("True", "true")
            else:
                deposit_str += "{};"
                map_str += "\"\";"
                collectibles_str += "{};"

            hpp_content += f"//------------- {key}_Map --------------//\n\n"
            hpp_content += externs + "\n"

            cpp_content += f"//------------- {key}_Map --------------//\n\n"

            cpp_content += deposit_str + "\n"
            cpp_content += map_str + "\n"
            cpp_content += collectibles_str + "\n\n"

        return [hpp_content, cpp_content]

    def __str__(self):
        return "".join(self.convert())


def write(data: MapData):
    data_begin = "///   _______                _____          __\n" \
                 "///  |   |   |.---.-..-----.|     \ .---.-.|  |_ .---.-.\n" \
                 "///  |       ||  _  ||  _  ||  --  ||  _  ||   _||  _  |\n" \
                 "///  |__|_|__||___._||   __||_____/ |___._||____||___._|\n" \
                 "///                  |__|\n"

    header_part = ""
    code_part = ""

    with open("../code/MapData.hpp") as f:
        parts = f.read().split(data_begin)
        header_part = parts[0]

    data = data.convert()

    header_part += data_begin + "\n"
    header_part += f"#define MAP_EMPTY_TILE '{empty}'\n" \
                   f"#define MAP_WALL_TILE '{wall}'\n" \
                   f"#define MAP_TRAP_TILE '{trap}'\n" \
                   f"#define MAP_SWAMP_TILE '{swamp}'\n" \
                   f"#define MAP_TEMP_WALL_TILE '{temp_wall}'\n" \
                   f"#define MAP_UNKNOWN_TILE '{unknown}'\n\n"
    header_part += data[0] + "\n"
    header_part += "#endif // !MAPDATA_HPP"

    with open("../code/Data.cpp") as f:
        parts = f.read().split(data_begin)
        code_part = parts[0]

    code_part += data_begin + "\n"
    code_part += data[1] + "\n"

    with open("../code/MapData.hpp", "w+") as f:
        if header_part is not f.read():
            print("Writing header file")
            f.write(header_part)

    with open("../code/Data.cpp", "w+") as f:
        if code_part is not f.read():
            print("Writing code file")
            f.write(code_part)


def main():
    print("setting up...")

    # mapData = MapData(img_dirs=["debugging"], fd_dirs=FieldFD)
    mapData = MapData(img_files={"World1": [FieldA, [270, 180]], "World2": [FieldB, [360, 270]]}, fd_file=FieldFD)
    mapData.save(10)

    write(mapData)


if __name__ == '__main__':
    main()
    exit()
