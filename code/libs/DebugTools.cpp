//
// Created by flxwly on 29.05.2020.
//
#include "DebugTools.hpp"

DebugTool::DebugTool(sf::RenderWindow *_window) {
    window = _window;
    if (window != nullptr) {
        size_x = window->getSize().x;
        size_y = window->getSize().y;
    }
}

//############### Add / remove map features ##############################

// add / remove maps
void DebugTool::add_map(const std::string& label, std::vector<std::vector<int>> *map) {
    maps.emplace_back(label, map);
}
int DebugTool::remove_map(const std::string& label) {
    for (unsigned int i = 0; i < maps.size(); i++) {
        if (maps[i].first == label) {
            std::swap(maps[i], maps.back());
            maps.pop_back();
            return 1;
        }
    }
    return 0;
}

// add / remove robot positions
void DebugTool::add_r_pos(const std::string &label, std::pair<int *, int *> pos) {
    r_positions.emplace_back(label, pos);
}
int DebugTool::remove_r_pos(const std::string &label) {
    for (unsigned int i = 0; i < r_positions.size(); i++) {
        if (r_positions[i].first == label) {
            std::swap(r_positions[i], r_positions.back());
            r_positions.pop_back();
            return 1;
        }
    }
    return 0;
}

//################ Print the map ##########################################

void DebugTool::draw() {
    if (window == nullptr) {
        return; // window is not defined yet
    }
    sf::Font font;

    float mapsY = 10.0f * static_cast<float>(maps.size());
    for (const auto& map : maps) {
        mapsY += static_cast<float>(map.second[0].size());
    }

    float move_y = 0;

    for(const auto& map : maps) {
        float scaleX = static_cast<float> (window->getSize().x) / static_cast<float> (map.second->size()) / 2;
        float scaleY = static_cast<float> (window->getSize().y) / mapsY;

        sf::Text label;
        label.setString(map.first);
        label.setPosition(10, move_y);
        window->draw(label);

        sf::RectangleShape block(sf::Vector2f(scaleX, scaleY));
        for (unsigned int i = 0; i < map.second->size(); i++) {
            for (unsigned int j = 0; j < map.second[i].size(); j++) {

                switch (map.second->at(i).at(j)) {
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
                    case 4:
                        block.setSize(sf::Vector2f(scaleX * 4, scaleY * 4));
                        block.setFillColor({255, 153, 0});
		                break;
                    default:
                        block.setFillColor(sf::Color::Red);
                        break;
                }
                block.setPosition(static_cast<float> (i) * scaleX, static_cast<float> (j) * scaleY + move_y);
                window->draw(block);
            }
        }
        move_y += 10;
    }

    window->display();

}
