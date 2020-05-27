#ifndef MAPDATA_HPP
#define MAPDATA_HPP

#include <vector>
#include <iostream>

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

    int getPointCount(int pColor);

    int findPoint(std::pair<int, int> p, int pColor);

    int erasePoint(std::pair<int, int> p, int pColor);

    int erasePoint(int index, int pColor);

    std::pair<int, int> getPoint(int index, int pColor);

    std::vector<std::pair<int, int>> getDepositAreas();

    int availableColors();

private:
    std::vector<std::pair<int, int>> *getPtrToArr(int pColor);

    std::vector<std::pair<int, int>> DepositAreas;
    std::vector<std::pair<int, int>> RedPoints;
    std::vector<std::pair<int, int>> GreenPoints;
    std::vector<std::pair<int, int>> BlackPoints;
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