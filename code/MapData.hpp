#ifndef MAPDATA_HPP
#define MAPDATA_HPP

#include <vector>
#include <iostream>
#include <array>
#include "Point.hpp"

class MapData {
public:
	int MapX;
	int MapY;
	std::vector<std::vector<int>> Map;
	MapData(int mapX, int mapY,
            std::vector<std::pair<int, int>> *redPoints,
            std::vector<std::pair<int, int>> *greenPoints,
            std::vector<std::pair<int, int>> *blackPoints,
            std::vector<std::pair<int, int>> *deposits,
            std::vector<std::pair<int, int>> *walls,
            std::vector<std::pair<int, int>> *traps,
            std::vector<std::pair<int, int>> *swamps);

    int findPoint(std::pair<int, int> p, int pColor);
    int erasePoint(std::pair<int, int> p, int pColor);
    int erasePoint(int index, int pColor);

    // TODO: int findApproxPoint(std::pair<int, int> p, int pColor);
    int getPointCount(int pColor);
    std::vector<Point*> getAllPoints();
    Point *getClosestPoint(std::pair<int, int> pos);
    std::pair<int, int> getPoint(int index, int pColor);
    std::vector<std::pair<int, int>> getDepositAreas();
    std::vector<std::pair<int, int>> getNodes();

    std::pair<std::vector<Point *>, std::pair<int, int>> get_path(std::array<int, 3> loaded_objects, int loaded_objects_num, std::pair<int, int> pos);


    int availableColors();

private:
    std::vector<Point> *getPtrToArr(int pColor);

    std::vector<std::pair<int, int>> Nodes;
    std::vector<std::pair<int, int>> DepositAreas;
    std::vector<Point> RedPoints;
    std::vector<Point> GreenPoints;
    std::vector<Point> BlackPoints;
    std::vector<Point *> AllPoints;
};

/*walls*/ extern std::vector<std::pair<int, int>> GAME0WALLS;
/*traps*/ extern std::vector<std::pair<int, int>> GAME0TRAPS;
/*swamps*/ extern std::vector<std::pair<int, int>> GAME0SWAMPS;
/*deposit*/ extern std::vector<std::pair<int, int>> GAME0DEPOSITAREAS;

extern std::vector<std::pair<int, int>> GAME0REDPOINTS;
extern std::vector<std::pair<int, int>> GAME0GREENPOINTS;
extern std::vector<std::pair<int, int>> GAME0BLACKPOINTS;


/*walls*/ extern std::vector<std::pair<int, int>> GAME1WALLS;
/*traps*/ extern std::vector<std::pair<int, int>> GAME1TRAPS;
/*swamps*/ extern std::vector<std::pair<int, int>> GAME1SWAMPS;
/*deposit*/ extern std::vector<std::pair<int, int>> GAME1DEPOSITAREAS;

extern std::vector<std::pair<int, int>> GAME1REDPOINTS;
extern std::vector<std::pair<int, int>> GAME1GREENPOINTS;
extern std::vector<std::pair<int, int>> GAME1BLACKPOINTS;

#endif // !MAPDATA_HPP