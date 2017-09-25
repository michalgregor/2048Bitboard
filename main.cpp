#include "GameBoard.h"
#include <iostream>
#include <string>

#include "HumanPlayer.h"
#include "LegalPlayer.h"

int main() {
    
//	HumanPlayer player;
	LegalPlayer player;

	for(unsigned int i = 0; i < 100000; i++) {

		GameBoard board;
//		std::cout << "score: " << board.getScore() << "\n\n";
//		std::cout << board << std::endl;

		while(!board.isGameOver()) {
			auto move = player.selectAction(board);
			board.move(move);

//			clear();
//			std::cout << board << std::endl;
//	
//			std::cout << "score: " << board.getScore() << "\n\n";
//			std::cout << board << std::endl;
		}

	}
}

