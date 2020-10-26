//
// Created by flxwly on 29.05.2020.
//
#ifndef EXTRATERRESTRIAL_DEBUGTOOLS_HPP
#define EXTRATERRESTRIAL_DEBUGTOOLS_HPP

#include <SFML/Graphics.hpp>


class DebugTool {
public:
    DebugTool(sf::RenderWindow *_window);

    void add_map(const std::string &label, std::vector<std::vector<int>> *map);

    int remove_map(const std::string &label);

    void add_r_pos(const std::string &label, std::pair<int *, int *> pos);

    int remove_r_pos(const std::string &label);

    void draw();

private:
    sf::RenderWindow *window;
    unsigned int size_x, size_y;

    std::vector<std::pair<std::string, std::vector<std::vector<int>> *>> maps;
    std::vector<std::pair<std::string, std::pair<int *, int *>>> r_positions;

    // create the window

};


#endif //EXTRATERRESTRIAL_DEBUGTOOLS_HPP
