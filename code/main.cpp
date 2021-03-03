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

    static Robot bot(&PositionX, &PositionY, &Compass, &SuperObj_X, &SuperObj_Y,
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

    std::cout << "\tLeft: " << Bot->leftColor.h << ":" << Bot->leftColor.s << ":" << Bot->leftColor.l << std::endl;
    std::cout << "\tRight: " << Bot->rightColor.h << ":" << Bot->rightColor.s << ":" << Bot->rightColor.l << std::endl;

    std::cout << "------ matching colors ------" << std::endl;

    std::cout << "\tLeft:";

    if (isRed(Bot->leftColor)) {
        std::cout << " Red";
    }
    if (isCyan(Bot->leftColor)) {
        std::cout << " Cyan";
    }
    if (isBlack(Bot->leftColor)) {
        std::cout << " Black";
    }
    if (isYellow(Bot->leftColor)) {
        std::cout << " Yellow";
    }
    if (isOrange(Bot->leftColor)) {
        std::cout << " Orange";
    }
    if (isSwamp(Bot->leftColor)) {
        std::cout << " Swamp";
    }
    if (isSuperObj(Bot->leftColor)) {
        std::cout << " Super Object";
    }

    std::cout << std::endl << "\tRight:";

    if (isRed(Bot->rightColor)) {
        std::cout << " Red";
    }
    if (isCyan(Bot->rightColor)) {
        std::cout << " Cyan";
    }
    if (isBlack(Bot->rightColor)) {
        std::cout << " Black";
    }
    if (isYellow(Bot->rightColor)) {
        std::cout << " Yellow";
    }
    if (isOrange(Bot->rightColor)) {
        std::cout << " Orange";
    }
    if (isSwamp(Bot->rightColor)) {
        std::cout << " Swamp";
    }
    if (isSuperObj(Bot->rightColor)) {
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

    /*// Pathfinder (Nodes):
    label.setCharacterSize(10);
    block.setSize(sf::Vector2f(4, 4));
    block.setOrigin((2), 2);
    block.setFillColor({0, 255, 0});
    for (auto &node : Bot->pathfinder1.map) {
        sf::Vector2f p1(static_cast<float>(node.pos.x),
                        static_cast<float>(node.pos.y));

        *//*sf::VertexArray connectedNodes(sf::Lines);
        for (auto neighbour : node.neighbours) {
            sf::Vector2f p2(static_cast<float>(neighbour.first->pos.x),
                            static_cast<float>(neighbour.first->pos.y));
            connectedNodes.append(p1);
            connectedNodes.append(p2);
        }
        CC->draw(connectedNodes);
        *//*
        block.setPosition(p1);
        CC->draw(block);
    }*/

    // Map:
    block.setSize({4, 4});
    block.setOrigin((2), 2);
    block.setFillColor({0, 255, 0});
    for (auto &walls : Bot->map1->getMapObjects({0})) {

        sf::VertexArray area(sf::LineStrip);

        for (auto &corner : walls.getCorners()) {
            area.append(sf::Vector2f(static_cast<float>(corner.x),
                                     static_cast<float>(corner.y)));
        }
        area.append(sf::Vector2f(static_cast<float>(walls.getCorners()[0].x),
                                 static_cast<float>(walls.getCorners()[0].y)));

        CC->draw(area);
    }

    // Collectibles
    block.setSize(sf::Vector2f(4, 4));
    block.setOrigin(2, 2);
    for (auto &collecible : Bot->map1->getCollectibles({0, 1, 2})) {

        if (collecible.state != 2) {
            block.setPosition(static_cast<float>(collecible.pos.x), static_cast<float>(collecible.pos.y));

            switch (collecible.state) {
                case 0:
                    block.setOutlineColor(sf::Color::Magenta);
                case 1:
                    block.setOutlineColor(sf::Color::Yellow);
            }

            switch (collecible.color) {
                case 0:
                    block.setFillColor(sf::Color::Red);
                    break;
                case 1:
                    block.setFillColor(sf::Color::Cyan);
                    break;
                default:
                    block.setFillColor(sf::Color::White);
                    break;
            }

            CC->draw(block);
        }
    }


    // Path
    block.setSize(sf::Vector2f(6, 6));
    block.setOrigin(3, 3);
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

    // Position from Simulator
    block.setSize(sf::Vector2f(8, 8));
    block.setOrigin((4), 4);
    block.setFillColor({140, 30, 0});             // dark red / brown
    block.setPosition(static_cast<float>(PositionX),
                      static_cast<float>(PositionY));
    CC->draw(block);

    PVector newPos = Bot->pos + Bot->getVelocity(1000);

    sf::Vertex line[] = {sf::Vertex(block.getPosition()), sf::Vertex(sf::Vector2f(
            static_cast<float>(newPos.x), static_cast<float>(newPos.y)))};
    CC->draw(line, 2, sf::Lines);

    // Position from calculation
    block.setSize(sf::Vector2f(4, 4));
    block.setOrigin(2, 2);
    block.setFillColor({0, 200, 200});             // dark red / brown
    block.setPosition(static_cast<float>(Bot->pos.x),
                      static_cast<float>(Bot->pos.y));
    CC->draw(block);

    /*// Position from calculation
    block.setSize(sf::Vector2f(4, 4));
    block.setOrigin(2, 2);
    block.setFillColor({200, 0, 200});             // dark red / brown
    block.setPosition(static_cast<float>(Bot->nextTarget.x),
                      static_cast<float>(Bot->nextTarget.y));
    CC->draw(block);*/
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