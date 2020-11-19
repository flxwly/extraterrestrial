#ifndef EXTRATERRESTRIAL_DEBUGTOOLS_HPP
#define EXTRATERRESTRIAL_DEBUGTOOLS_HPP

#include <SFML/Graphics.hpp>


class DebugTool {
public:
	explicit DebugTool(const std::string& label = "console");

	void addMap(const std::string &label, std::vector<std::vector<int>> *map);

	int removeMap(const std::string &label);

	void clearMaps();

	void addRobotPosition(const std::string &label, std::pair<int *, int *> pos);

	int removeRobotPosition(const std::string &label);

	void clearRobotPositions();

	void update();

	void draw();

	void render();

private:
	const unsigned int maxSizeX_ = 1080, maxSizeY_ = 720;

	sf::RenderWindow window_ = sf::RenderWindow(sf::VideoMode(360, 270), "Debug_Console");
	unsigned int sizeX_ = 360, sizeY_ = 270;

	float scaleX_ = 10, scaleY_ = 10;

	std::vector<std::pair<std::string, std::vector<std::vector<int>> *>> maps_;
	std::vector<std::pair<std::string, std::pair<int *, int *>>> robotPositions_;

	bool changedContents = true;
};


#endif //EXTRATERRESTRIAL_DEBUGTOOLS_HPP
