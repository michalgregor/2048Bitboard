#include "GameBoard.h"
#include <iostream>
#include <string>

#include "LegalPlayer.h"

int main() {
	LegalPlayer player;

	GameBoard board;
	std::cout << "score: " << board.getScore() << "\n\n";
	std::cout << board << std::endl;

	while(!board.isGameOver()) {
		auto move = player.selectAction(board);      
		board = board.next(move);
        auto score = board.getScore();
		std::cout << "score: " << score << "\n\n";		
	}
}

