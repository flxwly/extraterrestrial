#define CsBot_AI_H //DO NOT delete this line
#define CsBot_AI_C  //DO NOT delete this line
#ifndef CSBOT_REAL

#define ROBOT_RAD 6

#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <SFML/Graphics.hpp>

using namespace std;

#include "CoSpaceFunctions.hpp"
#include "CommonFunctions.hpp"
#include "ColorRecognition.hpp"


// Classes
#include "Pathfinder.hpp"
#include "MapData.hpp"
#include "DebugTools.hpp"
#include "Robot.hpp"
#include "Point.hpp"

#endif

void updateHSL() {
    DEBUG_MESSAGE("Updating HSL...\n", 3);

    hueR = rgb2h(CSRight_R, CSRight_G, CSRight_B);
    hueL = rgb2h(CSLeft_R, CSLeft_G, CSLeft_B);
    DEBUG_MESSAGE("\t Hue Right: " + to_string(hueR) + " | Hue Left: " + to_string(hueL) + "\n", 3);

    satR = rgb2s(CSRight_R, CSRight_G, CSRight_B);
    satL = rgb2s(CSLeft_R, CSLeft_G, CSLeft_B);
    DEBUG_MESSAGE("\t Sat Right: " + to_string(satR) + " | Sat Left: " + to_string(satL) + "\n", 3);

    lumR = rgb2l(CSRight_R, CSRight_G, CSRight_B);
    lumL = rgb2l(CSLeft_R, CSLeft_G, CSLeft_B);
    DEBUG_MESSAGE("\t Lum Right: " + to_string(lumR) + " | Lum Left: " + to_string(lumL) + "\n", 3);

    DEBUG_MESSAGE("finished updating HSL \n", 3);
}

MapData GAME0(240, 180, &GAME0REDPOINTS, &GAME0GREENPOINTS, &GAME0BLACKPOINTS, &GAME0DEPOSITAREAS, &GAME0WALLS,
              &GAME0TRAPS, &GAME0SWAMPS);

MapData GAME1(360, 270, &GAME1REDPOINTS, &GAME1GREENPOINTS, &GAME1BLACKPOINTS, &GAME1DEPOSITAREAS, &GAME1WALLS,
              &GAME1TRAPS, &GAME1SWAMPS);
Pathfinder PathfinderGame0(GAME0.Map);

Pathfinder PathfinderGame1(GAME1.Map);

sf::RenderWindow *CC = nullptr;
sf::Font *unbutton = nullptr;

Robot Bot(&PositionX, &PositionY, &Compass, &SuperObj_Num, &SuperObj_X, &SuperObj_Y,
          &CSRight_R, &CSRight_G, &CSRight_B, &CSLeft_R, &CSLeft_G, &CSLeft_B,
          &US_Left, &US_Front, &US_Right,
          &WheelLeft, &WheelRight, &LED_1, &Teleport, &Time,
          &GAME0, &GAME1, &PathfinderGame0, &PathfinderGame1);

void Setup() {
    system("cls");

    DEBUG_MESSAGE("Init... \n", 0);

    static sf::RenderWindow window(sf::VideoMode(1080, 840), "Debug_Console");
    CC = &window;
    CC->setFramerateLimit(10);
    static sf::Font font;
    if (font.loadFromFile("../fonts/unbutton.ttf")) {
        unbutton = &font;
    } else if (font.loadFromFile("unbutton.ttf")) {
        unbutton = &font;
    } else if (font.loadFromFile(
            "C:/Users/flxwly/Microsoft Robotics Dev Studio 4/CS.C/User/RSC/extraterrestrial/unbutton.ttf")) {
        unbutton = &font;
    }

    if (unbutton != nullptr) {
        std::cout << "loaded font" << std::endl;
    }


    updateHSL();
    DEBUG_MESSAGE("\tGame0...", 0);


    DEBUG_MESSAGE("\t finished\n", 0);

    DEBUG_MESSAGE("\tGame1...", 0);

    /*std::vector<std::vector<int>> pMap;
    for (int i = 0; i < PathfinderGame1.map.size(); i++) {
        std::vector<int> _v;
        pMap.push_back(_v);
        for (int j = 0; j < PathfinderGame1.map[i].size(); j++) {
            auto node = PathfinderGame1.map[i][j];
            if (node.isWall) {
                pMap[i].push_back(1);
            } else if (node.isTrap) {
                pMap[i].push_back(2);
            } else if (node.isSwamp) {
                pMap[i].push_back(3);
            } else {
                pMap[i].push_back(0);
            }
        }
    }

    Debug.addMap(pMap);
    */


    DEBUG_MESSAGE("\t finished\n", 0);

    DEBUG_MESSAGE("finished initialization\n", 0);

    // run the program as long as the window is open

}

/*
 * ///_________________________________GAME0________________________________________///
 *
 * ///__________________________________________________________________________________///
*/

void Game0() {
    updateHSL();
    Bot.game_0_loop();
}

/*
 * ///_________________________________GAME1________________________________________///
 * TODO Make Pathfinding work
 *  - Get new Waypoint if old one is not needed anymore
 *  - follow route
 *      - choose best point (in  to)
 *      - remember time of visited points.
 * TODO Hunt Superobject
 * TODO
 * ///__________________________________________________________________________________///
*/

void Game1_Debug() {
    sf::Event event;
    while (CC->pollEvent(event)) {
        // "close requested" event: we close the window
        if (event.type == sf::Event::Closed)
            CC->close();
    }

    CC->clear(sf::Color::Black);

    if (unbutton == nullptr || CC == nullptr) {
        std::cout << "something went wrong" << std::endl;
        return;
    }


    float scaleX = static_cast<float> (CC->getSize().x) / static_cast<float> (GAME1.MapX);
    float scaleY = static_cast<float> (CC->getSize().y) / static_cast<float>(GAME1.MapY);

    sf::Text label("Map", *unbutton);
    label.setPosition(10, 5);
    label.setCharacterSize(30);
    label.setStyle(sf::Text::Regular);
    CC->draw(label);

    // Map
    sf::RectangleShape block(sf::Vector2f(scaleX, scaleY));
    for (unsigned int i = 0; i < GAME1.MapX; i++) {
        for (unsigned int j = 0; j < GAME1.MapY; j++) {

            switch (GAME1.Map[i][j]) {
                case 0:
                    block.setFillColor(sf::Color::White);
                    break;
                case 1:
                    block.setFillColor(sf::Color::Black);
                    break;
                case 2:
                    block.setFillColor(sf::Color::Yellow);
                    break;
                case 3:
                    block.setFillColor({166, 166, 166});
                    break;
                default:
                    block.setFillColor(sf::Color::Red);
                    break;
            }
            block.setPosition(static_cast<float> (i) * scaleX, static_cast<float> (GAME1.MapY - j) * scaleY);
            CC->draw(block);
        }
    }

    // Deposit Area
    block.setSize(sf::Vector2f(scaleX * 10, scaleY * 10));
    block.setFillColor({255, 153, 0});
    for (auto deposit : GAME1.getDepositAreas()) {
        block.setPosition(static_cast<float> (deposit.first) * scaleX, static_cast<float> (GAME1.MapY - deposit.second) * scaleY);
        CC->draw(block);
    }

    // Path
    block.setSize(sf::Vector2f(scaleX * 3, scaleY * 3));
    block.setFillColor({155, 153, 255});
    for (const auto& path : Bot.complete_path) {
        for (auto point : path) {
            block.setPosition(static_cast<float> (point.first) * scaleX, static_cast<float> (GAME1.MapY - point.second) * scaleY);
            CC->draw(block);
        }
    }
    block.setSize(sf::Vector2f(scaleX * 4, scaleY * 4));
    block.setFillColor({155, 255, 255});
    block.setPosition(static_cast<float> (PositionX) * scaleX, static_cast<float> (GAME1.MapY - PositionY) * scaleY);
    CC->draw(block);

    // Points
    block.setSize(sf::Vector2f(scaleX * 5, scaleY * 5));
    for (auto point : GAME1.getAllPoints()) {
        if (point->state != 0) {
            block.setPosition(static_cast<float> (point->pos.first) * scaleX,
                              static_cast<float> (point->pos.second) * scaleY);
            switch (point->color) {
                case 0:
                    block.setFillColor({255, 0, 0});
                    break;
                case 1:
                    block.setFillColor({0, 255, 255});
                    break;
                case 2:
                    block.setFillColor({50, 255, 0});
                    break;
                default:
                    block.setFillColor({0, 0, 0});
                    break;
            }

            CC->draw(block);
        }
    }


    CC->display();
}

void Game1() {
    updateHSL();

    Game1_Debug();
    Bot.game_1_loop();

    // === Debug ===
}