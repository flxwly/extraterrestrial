#define CsBot_AI_H //DO NOT delete this line
#define CsBot_AI_C  //DO NOT delete this line
#ifndef CSBOT_REAL

// Libs
#include "libs/PPSettings.hpp"
#include "libs/CoSpaceFunctions.hpp"
#include "libs/CommonFunctions.hpp"
#include "libs/ColorRecognition.hpp"

// Classes
#include "Pathfinder.hpp"
#include "MapData.hpp"
#include "Robot.hpp"

#include <iostream>
#include <fileapi.h>

#endif


#ifdef SFML

#include <SFML/Graphics.hpp>

sf::RenderWindow *CC = nullptr;
sf::Font *unbutton = nullptr;
#endif

Field *GAME0 = nullptr;

Field *GAME1 = nullptr;

Robot *Bot = nullptr;

void Setup() {
    system("cls");

    // TODO: Move static objects back to static space so the objects get initialized upon load
    // ----------- Initialisation of static objects -------------------- //

    static Field Game0(270, 180, GAME0WALLS, GAME0TRAPS, GAME0SWAMPS, GAME0WATERS, GAME0DEPOSITS,
                       GAME0WALLNODES, GAME0TRAPNODES, GAME0SWAMPNODES, GAME0COLLECTIBLES);
    GAME0 = &Game0;

    static Field Game1(360, 270, GAME1WALLS, GAME1TRAPS, GAME1SWAMPS, GAME1WATERS, GAME1DEPOSITS,
                       GAME1WALLNODES, GAME1TRAPNODES, GAME1SWAMPNODES, GAME1COLLECTIBLES);
    GAME1 = &Game1;

    static Robot bot(&PositionX, &PositionY, &Compass, &SuperObj_Num, &SuperObj_X, &SuperObj_Y,
                     &US_Left, &US_Front, &US_Right,
                     &WheelLeft, &WheelRight, &LED_1, &Teleport, &Time, GAME0, GAME1);
    Bot = &bot;


#ifdef SFML
    static sf::RenderWindow window(sf::VideoMode(480, 360), "Debug_Console");
    CC = &window;
    CC->setFramerateLimit(60);

    static sf::Font font;
    if (font.loadFromFile(
            "C:/Users/flxwl/Microsoft Robotics Dev Studio 4/CS/User/Rescue/CsBot/fonts/arial.ttf")) {
        unbutton = &font;
    }

#endif
}

#ifdef COLOR_LOGGING
void colorLogging() {

    std::cout << "------ RGB -------" << std::endl;

    std::cout << "\tLeft: " << CSLeft_R << ":" << CSLeft_G << ":" << CSLeft_B << std::endl;
    std::cout << "\tRight: " << CSRight_R << ":" << CSRight_G << ":" << CSRight_B << std::endl;

    std::cout << "------ HSL -------" << std::endl;

    std::cout << "\tLeft: " << hueL << ":" << satL << ":" << lumL << std::endl;
    std::cout << "\tRight: " << hueR << ":" << satR << ":" << lumR << std::endl;

    std::cout << "------ matching colors ------" << std::endl;

    std::cout << "\tLeft:";

    if (isRedLeft()) {
        std::cout << " Red";
    }
    if (isCyanLeft()) {
        std::cout << " Cyan";
    }
    if (isBlackLeft()) {
        std::cout << " Black";
    }
    if (isYellowLeft()) {
        std::cout << " Yellow";
    }
    if (isOrangeLeft()) {
        std::cout << " Orange";
    }
    if (isSwampLeft()) {
        std::cout << " Swamp";
    }
    if (isSuperObjLeft()) {
        std::cout << " Super Object";
    }

    std::cout << std::endl << "\tRight:";

    if (isRedRight()) {
        std::cout << " Red";
    }
    if (isCyanRight()) {
        std::cout << " Cyan";
    }
    if (isBlackRight()) {
        std::cout << " Black";
    }
    if (isYellowRight()) {
        std::cout << " Yellow";
    }
    if (isOrangeRight()) {
        std::cout << " Orange";
    }
    if (isSwampRight()) {
        std::cout << " Swamp";
    }
    if (isSuperObjRight()) {
        std::cout << " Super Object";
    }

    std::cout << std::endl;

}
#endif

/*
 * ///_________________________________GAME0________________________________________///
 *
 * ///__________________________________________________________________________________///
*/

void Game0() {
    Bot->updateLoop();
    Bot->game0Loop();
}

/*
 * ///_________________________________GAME1________________________________________///
 *
 * ///__________________________________________________________________________________///
*/

void Game1Debug() {

#ifdef SFML
    //##########//
    //  Setup   //
    //##########//

    // check nullptr to prevent crash.
    if (unbutton == nullptr || CC == nullptr) {
        ERROR_MESSAGE("Something went wrong. Window or font is not initialized correctly.")
        return;
    }

    // Event handling
    sf::Event event{};
    while (CC->pollEvent(event)) {
        // "close requested" event: we close the window
        if (event.type == sf::Event::Closed)
            CC->close();

        if (event.type == sf::Event::Resized)
            CC->setActive();
    }

    // clear screen
    CC->clear(sf::Color::Black);

    sf::RectangleShape block(sf::Vector2f(2, 2));

    //##########//
    //  print   //
    //##########//

    // Map label
    sf::Text label("Map", *unbutton);
    label.setPosition(10, 5);
    label.setCharacterSize(30);
    label.setFillColor(sf::Color::White);
    label.setStyle(sf::Text::Regular);
    CC->draw(label);

    // Pathfinder (Nodes):
    label.setCharacterSize(10);
    block.setSize(sf::Vector2f(4, 4));
    block.setOrigin((2), 2);
    block.setFillColor({0, 255, 0});
    for (auto &node : Bot->pathfinder1_.map) {
        sf::Vector2f p1(static_cast<float>(node.pos.x),
                        static_cast<float>(node.pos.y));
        block.setPosition(p1);

        CC->draw(block);
    }

    // Map:
    block.setSize({4, 4});
    block.setOrigin((2), 2);
    block.setFillColor({0, 255, 0});
    for (auto &walls : Bot->map1_->getMapObjects({0})) {

        sf::VertexArray area(sf::LineStrip);

        for (auto &corner : walls.getCorners()) {
            area.append(sf::Vector2f(static_cast<float>(corner.x),
                                     static_cast<float>(corner.y)));
        }
        area.append(sf::Vector2f(static_cast<float>(walls.getCorners()[0].x),
                                 static_cast<float>(walls.getCorners()[0].y)));

        CC->draw(area);
    }


    // Path
    block.setSize(sf::Vector2f(6, 6));
    block.setOrigin((3), 3);
    sf::VertexArray path_lines(sf::LineStrip);
    path_lines.append(sf::Vector2f(PositionX, PositionY));
    for (unsigned int i = 0; i < Bot->completePath.size(); i++) {

        Path path = Bot->completePath[i];
        sf::Uint8 rval = i * 80 + 80;
        block.setFillColor({rval, 0, 255});

        for (auto point : path.points) {
            path_lines.append({sf::Vector2f(static_cast<float>(point.x),
                                            static_cast<float>(point.y)), sf::Color::Red});
            block.setPosition(static_cast<float>(point.x),
                              static_cast<float>(point.y));
            if (point == path.getLast()) {
                block.setFillColor({200, 100, 100});
            }
            CC->draw(block);
        }
        block.setFillColor({160, 0, 255});       // purple
    }

    CC->draw(path_lines);

    // Position
    block.setSize(sf::Vector2f(8, 8));
    block.setOrigin((4), 4);
    block.setFillColor({140, 30, 0});             // dark red / brown
    block.setPosition(static_cast<float>(PositionX),
                      static_cast<float>(PositionY));
    CC->draw(block);

    // Position
    block.setSize(sf::Vector2f(4, 4));
    block.setOrigin(2, 2);
    block.setFillColor({0, 200, 200});             // dark red / brown
    block.setPosition(static_cast<float>(Bot->aPos_.x),
                      static_cast<float>(Bot->aPos_.y));
    CC->draw(block);

    CC->display();
#endif

#ifdef COLOR_LOGGING
    colorLogging();
#endif

}


void Game1() {
    Bot->updateLoop();
    Bot->game1Loop();

    Game1Debug();
}