#include "SFMLWindow.hpp"


DebugWindow::DebugWindow(Robot *robot) : Bot(robot) {

	Font.loadFromFile("%FONT_FOLDER%/unbutton.ttf");

}

void DebugWindow::GameDebug(unsigned int ID) {

	Field *map = maps[ID];

	sf::RenderWindow window(sf::VideoMode(1080, 840), "Map" + std::to_string(ID));
	runningWindows.push_back(&window);


	while (isRunning[ID]) {

		// Event handling
		sf::Event event;
		while (window.pollEvent(event)) {
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed) {
				window.close();
				isRunning[ID] = false;
				return;
			}
		}

		// clear screen
		window.clear(sf::Color::Black);

		// Vars for drawing on map
		sf::Vector2u size = window.getSize();
		sf::Vector2f scale = {static_cast<float>(size.x) / static_cast<float>(map->getSize().x),
		                      static_cast<float>(size.y) / static_cast<float>(map->getSize().y)};
		sf::RectangleShape block(sf::Vector2f(scale.x, scale.y));

		//##########//
		//  print   //
		//##########//

		// Map label
		sf::Text label("Map", Font);
		label.setPosition(10, 5);
		label.setCharacterSize(30);
		label.setFillColor(sf::Color::Black);
		label.setStyle(sf::Text::Regular);
		window.draw(label);

		// Map:
		block.setSize({scale.x * 2, scale.y * 2});
		block.setFillColor({0, 255, 0});
		for (auto &walls : map->getMapObjects({0})) {
			for (auto &edge : walls.getEdges()) {
				sf::Vector2f p1(static_cast<float>(edge.p1.x * scale.x),
				                static_cast<float>(edge.p1.y * scale.y));
				sf::Vector2f p2(static_cast<float>(edge.p2.x * scale.x),
				                static_cast<float>(edge.p2.y * scale.y));

				sf::Vertex area[] = {p1, p2};
				area->color = sf::Color(0, 255, 0, 80);

				window.draw(area, 2, sf::Lines);
			}
		}


		// Path
		block.setSize(sf::Vector2f(scale.x * 3, scale.y * 3));
		block.setOrigin(scale.x * 1.5f, scale.y * 1.5f);
		sf::VertexArray path_lines(sf::Lines);
		for (unsigned int i = 0; i < Bot->completePath.size(); i++) {

			Path path = Bot->completePath[i];
			sf::Uint8 rval = i * 80 + 80;
			block.setFillColor({rval, 0, 255});


			for (unsigned int j = 0; j < path.points.size(); j++) {

				PVector point = path.points[j];

				path_lines.append({sf::Vector2f(static_cast<float> (point.x) * scale.x,
				                                static_cast<float> (point.y) * scale.y),
				                   sf::Color::Red});
				block.setPosition(static_cast<float> (point.x) * scale.x,
				                  static_cast<float> (point.y) * scale.y);
				if (j == path.points.size() - 1) {
					block.setFillColor({200, 100, 0});
				}
				window.draw(block);
			}
			block.setFillColor({160, 0, 255});       // purple
		}

		window.draw(path_lines);

		// Position
		block.setSize(sf::Vector2f(scale.x * 4, scale.y * 4));
		block.setOrigin(scale.x * 2, scale.y * 2);
		block.setFillColor({140, 30, 0});             // dark red / brown
		block.setPosition(static_cast<float> (PositionX) * scale.x,
		                  static_cast<float> (PositionY) * scale.y);
		window.draw(block);

		window.display();
	}

	window.close();

}

void DebugWindow::startDebugging(Field *map) {

	maps.push_back(map);
	isRunning.push_back(true);

	runningLoops.emplace_back(&DebugWindow::GameDebug, this, IDs.size());
	IDs.push_back(IDs.size());
}

void DebugWindow::stopDebugging(unsigned int ID) {

	unsigned int index = std::find(IDs.begin(), IDs.end(), ID) - IDs.begin();

	if (index < IDs.size()) {

		// make sure the window gets closed
		isRunning[index] = false;
		runningLoops[index].join();


		// remove it from the vectors
		IDs.erase(IDs.begin() + index);
		maps.erase(maps.begin() + index);
		runningLoops.erase(runningLoops.begin() + index);
		runningWindows.erase(runningWindows.begin() + index);
		isRunning.erase(isRunning.begin() + index);
	}
}

void DebugWindow::updateLoop() {

	std::vector<unsigned int> canBeDeleted;

	for (unsigned int i = 0; i < IDs.size(); i++) {
		if (!isRunning[i]) {
			canBeDeleted.push_back(IDs[i]);
		}
	}

	for (auto ID : canBeDeleted) {
		stopDebugging(ID);
	}

}
