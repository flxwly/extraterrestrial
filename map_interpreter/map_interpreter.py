import xml.etree.ElementTree as ET
from os import path

import cv2

FieldA = "../../../../../store/media/Rescue/Map/Sec/Design/FieldA"
FieldB = "../../../../../store/media/Rescue/Map/Sec/Design/FieldB"
FieldFD = ET.parse("../../../../../store/media/Rescue/Map/Sec/Design/Field.FD")
FieldFD = FieldFD.getroot()


class Point:
    def __init__(self, t, x, y, world):
        self.t = t
        if world == 0:
            self.x = 100 * float(x) + 270 / 2
            self.y = 100 * float(y) + 180 / 2
        elif world == 1:
            self.x = 100 * float(x) + 360 / 2
            self.y = 100 * float(y) + 270 / 2

    def __str__(self):
        return str(self.t) + ": " + str(self.x) + " | " + str(self.y)


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
                    arr.append(Point(t, cx, cy, worldnr))

    return arr


def get_obj_type(pixel):
    if len(pixel) > 2:
        b = pixel[0]
        g = pixel[1]
        r = pixel[2]
    else:
        return "invalid"

    if r == 221 and g == 186 and b == 151:
        return "wall"
    if r == 0 and g == 0 and b == 0:
        return "black"
    if r == 166 and g == 166 and b == 166:
        return "swamp"
    if r == 255 and g == 255 and b == 0:
        return "trap"
    if r == 255 and g == 153 and b == 0:
        return "deposit"
    return "invalid"


def game_string(world, i, j):
    return "GAME" + str(world) + "[" + str(i) + "][" + str(j) + "]"


def set_area_point_to_zero(i, j, imgarr, zerod):
    if get_obj_type(imgarr[j][i]) == "deposit":
        imgarr[j][i] = [0, 0, 0]
        zerod.append([i, j])  # change it and add it
        # print(str(i) + " | " + str(j))
        for n in range(j - 1, j + 2):
            for m in range(i - 1, i + 2):
                if 0 <= m < len(imgarr[j]) and 0 <= n < len(imgarr):  # boundary check
                    set_area_point_to_zero(m, n, imgarr, zerod)


def convert_background(_dir, width, height, worldnr):
    walls = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "WALLS = {"
    traps = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "TRAPS = {"
    swamps = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "SWAMPS = {"
    depositareastr = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "DEPOSITAREAS = "

    zerod = []
    depositareas = []

    imgarr = cv2.imread(_dir + "/Background.bmp")
    imgarr = cv2.resize(imgarr, None, fx=0.5, fy=0.5, interpolation=cv2.INTER_CUBIC)

    for j in range(height):
        for i in range(width):
            pixel = imgarr[j][i]
            if get_obj_type(pixel) == "wall":
                walls += "{" + str(i) + ", " + str(j) + "}, "
            if get_obj_type(pixel) == "trap":
                traps += "{" + str(i) + ", " + str(j) + "}, "
            if get_obj_type(pixel) == "swamp":
                swamps += "{" + str(i) + ", " + str(j) + "}, "
            if get_obj_type(pixel) == "deposit":
                set_area_point_to_zero(i, j, imgarr, zerod)
                x = 0
                y = 0
                count = 1
                for n in zerod:
                    x += n[0]
                    y += height - n[1]
                    count += 1
                zerod = []
                x = int(x / count)
                y = int(y / count)
                depositareas.append([x, y])

    if path.exists(_dir + "/Obstacle.bmp"):
        wall_img = cv2.imread(_dir + "/Obstacle.bmp")
        wall_img = cv2.resize(wall_img, None, fx=0.5, fy=0.5, interpolation=cv2.INTER_CUBIC)
        for j in range(height):
            for i in range(width):
                pixel = wall_img[j][i]
                if get_obj_type(pixel) == "black":
                    walls += "{" + str(i) + ", " + str(j) + "}, "

    depositareastr += str(depositareas).replace("[", "{").replace("]", "}") + ";"
    walls = (walls + "}").replace(", }", "};")
    traps = (traps + "}").replace(", }", "};")
    swamps = (swamps + "}").replace(", }", "};")
    swamps = swamps.replace("= {}", "= {};")

    filecontent = "/*walls*/ " + walls + "\n/*traps*/ " + traps + "\n/*swamps*/ " + swamps + "\n/*deposit*/ " + \
                  depositareastr + "\n\n"
    return filecontent


def write_points_to_file(points, worldnr):
    redpoints = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "REDPOINTS = {"
    greenpoints = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "GREENPOINTS = {"
    blackpoints = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "BLACKPOINTS = {"

    for point in points:
        if point.t == "Object_Red":
            redpoints += "{" + str(int(point.x)) + ", " + str(int(point.y)) + "}, "
        if point.t == "Object_Green":
            greenpoints += "{" + str(int(point.x)) + ", " + str(int(point.y)) + "}, "
        if point.t == "Object_Black":
            blackpoints += "{" + str(int(point.x)) + ", " + str(int(point.y)) + "}, "

    redpoints = (redpoints + "}").replace(", }", "};")
    greenpoints = (greenpoints + "}").replace(", }", "};")
    blackpoints = (blackpoints + "}").replace(", }", "};")

    filecontent = redpoints + "\n" + greenpoints + "\n" + blackpoints + "\n\n\n"
    return filecontent


def main():
    print("setting up...")
    world_1_points = []
    world_2_points = []
    for child in FieldFD:
        if child.tag == "World1":
            world_1_points = find_color_points(child, 0)
        if child.tag == "World2":
            world_2_points = find_color_points(child, 1)
    filename = "../code/MapData.cpp"

    print("writing to file... (this might take some time)")

    f = open("mapDataCPP", "r")
    cpp_data = f.read() + "\n\n"
    f.close()

    f = open(filename, "w+")
    field_a = convert_background(FieldA, 240, 180, 0)
    field_a_points = write_points_to_file(world_1_points, 0)
    field_b = convert_background(FieldB, 360, 270, 1)
    field_b_points = write_points_to_file(world_2_points, 1)
    f.write(cpp_data + field_a + field_a_points + field_b + field_b_points)
    f.close()

    print("finished")


if __name__ == '__main__':
    main()
    print("press any key to close the window:")
    input()
