#ifndef HUMAN_PLAYER_H
#define HUMAN_PLAYER_H

#include "GameBoard.h"
#include "ncurses_stream.h"

class HumanPlayer {
private:
	ncurses_stream _stream;

public:
	GameBoard::GameAction selectAction(const GameBoard& /*gameState*/) {
		while(1) {
			int cmd = getch();

			switch(cmd) {
			case KEY_UP:
				std::cout << "up\n";
				return GameBoard::UP;
			break;
			case KEY_DOWN:
				return GameBoard::DOWN;
			break;
			case KEY_LEFT:
				return GameBoard::LEFT;
			break;
			case KEY_RIGHT:
				return GameBoard::RIGHT;
			break;
			default:
				std::cout << "default\n";
			break;
			}
		}
	}

public:
	HumanPlayer(): _stream(std::cout) {
		initscr();
		keypad(stdscr, true);
	}

	~HumanPlayer() {
		std::cout << "game over. press any key to exit..." << "\n";
		getch();
		endwin();
	}
};

#endif // HUMAN_PLAYER_H

