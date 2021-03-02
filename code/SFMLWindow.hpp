#ifndef CsBot_SFMLWINDOW_HPP
#define CsBot_SFMLWINDOW_HPP


#include <SFML/Graphics.hpp>
#include "Robot.hpp"

class DebugWindow {
public:
	DebugWindow() = delete;
	explicit DebugWindow(Robot *robot);

	sf::Font Font;

	Robot *Bot;

	void updateLoop();

	void startDebugging(Field *map);
	void stopDebugging(unsigned int ID);

	void GameDebug(unsigned int mapIndex);

	std::vector<unsigned int> IDs;
	std::vector<Field *> maps;
	std::vector<std::thread> runningLoops;
	std::vector<sf::RenderWindow *> runningWindows;
	std::vector<bool> isRunning;
};

#endif //CsBot_SFMLWINDOW_HPP
