import xml.etree.ElementTree as ET
from os import path

import cv2

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


def average_point(point_arr, h):
    x = 0
    y = 0
    count = 1
    for n in point_arr:
        x += n[0]
        y += h - n[1]
        count += 1
    return [int(x / count), int(y / count)]


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
        return -1

    if (r == 221 and g == 186 and b == 151) or (r == 0 and g == 0 and b == 0):
        return 1  # wall
    if r == 255 and g == 0 and b == 0:
        return 1.1  # at_wall
    if r == 255 and g == 255 and b == 0:
        return 2  # trap
    if r == 166 and g == 166 and b == 166:
        return 3  # swamp
    if (r == 255 and g == 153 and b == 0) or (r == 255 and g == 102 and b == 0):
        return 4  # deposit
    return -1


def game_string(world, i, j):
    return "GAME" + str(world) + "[" + str(i) + "][" + str(j) + "]"


def set_area_point_to_zero(i, j, imgarr, deposit_area_points):
    if get_obj_type(imgarr[j][i]) == 4:
        imgarr[j][i] = [-1, -1, -1]
        deposit_area_points.append([i, j])  # change it and add it
        for n in range(j - 1, j + 2):
            for m in range(i - 1, i + 2):
                if 0 <= m < len(imgarr[j]) and 0 <= n < len(imgarr):  # boundary check
                    set_area_point_to_zero(m, n, imgarr, deposit_area_points)


def expand_walls(img_arr, wall_arr, w, h):
    robot_r = 1
    new_walls = []
    for wall in wall_arr:
        for j in range(wall[1] - robot_r, wall[1] + robot_r):
            for i in range(wall[0] - robot_r, wall[0] + robot_r):
                if 0 <= i < w and 0 <= j < h:
                    if get_obj_type(img_arr[j][i]) not in [1, 1.1]:
                        new_walls.append([i, j])
                        img_arr[j][i] = [0, 0, -1]
    for new_wall in new_walls:
        wall_arr.append(new_wall)

    return wall_arr


def get_nodes(img_arr, wall_arr, w, h):
    nodes = []
    wall_steps = [[-1, 0], [0, -1], [0, 1], [1, 0]]
    for wall in wall_arr:
        surrounding_walls = []
        for step in wall_steps:
            x = wall[0] + step[0]
            y = wall[1] + step[1]
            if 0 <= x < w and 0 <= y < h:
                if get_obj_type(img_arr[y, x]) in [1, 1.1, 2]:
                    surrounding_walls.append([x, y])
            else:
                pass
                # surrounding_walls.append([x, y])
        if len(surrounding_walls) == 2:
            # only if not a straight line
            if surrounding_walls[0][0] - surrounding_walls[1][0] != 0 \
                    and surrounding_walls[0][1] - surrounding_walls[1][1] != 0:
                # x step of surrounding_walls[0] is 0
                if wall[0] - surrounding_walls[0][0] == 0:
                    temp = surrounding_walls[0]
                    surrounding_walls[0] = surrounding_walls[1]
                    surrounding_walls[1] = temp

                x_step = wall[0] - surrounding_walls[0][0]
                y_step = wall[1] - surrounding_walls[1][1]

                nodes.append([wall[0] + x_step, wall[1] + y_step])
    return nodes


def convert_background(_dir, width, height, worldnr):
    print("converting Background: " + str(worldnr))

    wall_coords = []
    wall_str = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "WALLS = "
    node_coords = []
    node_str = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "NODES = "
    trap_coords = []
    trap_str = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "TRAPS = "
    swamp_coords = []
    swamp_str = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "SWAMPS = "
    deposit_area_str = "std::vector<std::pair<int, int>> GAME" + str(worldnr) + "DEPOSITAREAS = "

    deposit_area_points = []
    deposit_areas = []

    img_arr = cv2.imread(_dir + "/Background.bmp")
    img_arr = cv2.resize(img_arr, None, fx=0.5, fy=0.5, interpolation=cv2.INTER_CUBIC)

    print("\tfirst convert")

    for j in range(height):
        for i in range(width):
            pixel = img_arr[j][i]

            if get_obj_type(pixel) == 1:
                wall_coords.append([i, j])
            elif get_obj_type(pixel) == 2:
                trap_coords.append([i, j])
            elif get_obj_type(pixel) == 3:
                swamp_coords.append([i, j])
            elif get_obj_type(pixel) == 4:
                set_area_point_to_zero(i, j, img_arr, deposit_area_points)
                deposit_areas.append(average_point(deposit_area_points, height))

    print("\tanother walls file exists")

    if path.exists(_dir + "/Obstacle.bmp"):
        wall_img = cv2.imread(_dir + "/Obstacle.bmp")
        wall_img = cv2.resize(wall_img, None, fx=0.5, fy=0.5, interpolation=cv2.INTER_CUBIC)
        for j in range(height):
            for i in range(width):
                pixel = wall_img[j][i]
                if get_obj_type(pixel) == 1:
                    wall_coords.append([i, j])

    print("\texpanding walls")
    wall_coords = expand_walls(img_arr, wall_coords, width, height)
    print("\tgetting nodes")
    node_coords = get_nodes(img_arr, wall_coords, width, height)

    print("convert to str")
    deposit_area_str += str(deposit_areas).replace("[", "{").replace("]", "}") + ";"
    wall_str += str(wall_coords).replace("[", "{").replace("]", "}").replace(" ", "") + ";"
    trap_str += str(trap_coords).replace("[", "{").replace("]", "}").replace(" ", "") + ";"
    swamp_str += str(swamp_coords).replace("[", "{").replace("]", "}").replace(" ", "") + ";"
    node_str += str(node_coords).replace("[", "{").replace("]", "}").replace(" ", "") + ";"

    filecontent = "/*walls*/ " + wall_str + "\n/*traps*/ " + trap_str + "\n/*swamps*/ " + swamp_str + "\n/*deposit*/ " + \
                  deposit_area_str + "\n\n"  # + "\n/*nodes*/ " + node_str + "\n\n"

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

    print("getting objects... (this might take some time)")

    f = open("mapDataCPP", "r")
    cpp_data = f.read() + "\n\n"
    f.close()

    field_a = convert_background(FieldA, 240, 180, 0)
    field_a_points = write_points_to_file(world_1_points, 0)
    field_b = convert_background(FieldB, 360, 270, 1)
    field_b_points = write_points_to_file(world_2_points, 1)
    print("writing to file...")
    f = open(filename, "w+")
    f.write(cpp_data + field_a + field_a_points + field_b + field_b_points)
    f.close()

    print("finished")


if __name__ == '__main__':
    main()
