#include "DebugTools.hpp"

DebugTool::DebugTool(const std::string& label) : window{ sf::VideoMode(360, 270), "Debug_Console" } {
	window.setTitle(label);
}

//############### Add / remove map features ##############################
// add / remove maps
void DebugTool::addMap(const std::string& label, std::vector<std::vector<int>> *map) {
    maps_.emplace_back(label, map);

    // changed something. recalculate dimensions etc.
	changedContents = true;
}

int DebugTool::removeMap(const std::string& label) {
    for (unsigned int i = 0; i < maps_.size(); i++) {
        if (maps_[i].first == label) {
            maps_.erase(maps_.begin() + i);

	        // changed something. recalculate dimensions etc.
	        changedContents = true;

            return 1;
        }
    }
    return 0;
}
// add / remove robot positions
void DebugTool::addRobotPosition(const std::string &label, std::pair<int *, int *> pos) {
    robotPositions_.emplace_back(label, pos);
	// changed something. recalculate dimensions etc.

	changedContents = true;

}

int DebugTool::removeRobotPosition(const std::string &label) {
    for (unsigned int i = 0; i < robotPositions_.size(); i++) {
        if (robotPositions_[i].first == label) {
	        robotPositions_.erase(robotPositions_.begin() + i);

	        // changed something. recalculate dimensions etc.
	        changedContents = true;

	        return 1;
        }
    }
    return 0;
}


/// update DebugTool Vars
void DebugTool::update() {
	if (!changedContents)
		return;

	unsigned int mapsX = 0;
	for (const auto& map : maps_) {
		if (map.second->size() + 2> mapsX)
			mapsX = map.second->size() + 2;
	}

	// 1 pixel spacing between each map + one at the end
	unsigned int mapsY = maps_.size() + 1;
	for (const auto& map : maps_) {
		// the height of one map
		mapsY += map.second[0].size();
	}

	unsigned int robotPositionsX = 0;
	for (const auto& pos : robotPositions_) {
		if (pos.first.size() * 3 > robotPositionsX)
			robotPositionsX = pos.first.size() * 3;
	}
	// for the coordinates
	robotPositionsX += 9 * 3;
	unsigned int robotPositionsY = robotPositions_.size() * 6;

	unsigned int biggestX = robotPositionsX + mapsX;
	unsigned int biggestY = (robotPositionsY > mapsY) ? robotPositionsY : mapsY;

	sizeX_ = std::min(biggestX, static_cast<unsigned int>(maxSizeX_));
	sizeY_ = std::min(biggestY, static_cast<unsigned int>(maxSizeY_));
	window.setSize({ sizeX_, sizeY_ });
}


//################ Write map to window buffer ##########################################

void DebugTool::draw() {
	if (changedContents)
		update();

    float mapsY = 10.0f * static_cast<float>(maps_.size());
    for (const auto& map : maps_) {
        mapsY += static_cast<float>(map.second[0].size());
    }

    float move_y = 0;

    for(const auto& map : maps_) {

        sf::RectangleShape block({1, 1});
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
	                    block.setFillColor({ 166, 166, 166 });
	                break;
                case 4:
	                block.setSize({ 4, 4 });
	                block.setFillColor({ 255, 153, 0 });
	                break;
                default:
	                block.setFillColor(sf::Color::Red);
	                break;
                }
	            block.setPosition(static_cast<float>(i), static_cast<float>(j));
	            window.draw(block);
            }
        }
        move_y += 10;
    }

	render();
}

void DebugTool::render() {
	window.display();
}
