#include "GameBoard.h"
#include <iostream>
#include <string>

#include "Evaluator.h"

#include "HumanPlayer.h"
#include "LegalPlayer.h"
#include "ExpectiMax.h"

int main() {
    
//	HumanPlayer player;
//	LegalPlayer player;
//	ExpectiMax<ScoreEvaluator> player(ScoreEvaluator{});
	ExpectiMax<HeuristicEvaluator> player(HeuristicEvaluator{});

	GameBoard board;
	std::cout << "score: " << board.getScore() << "\n\n";
	std::cout << board << std::endl;

	while(!board.isGameOver()) {
		auto move = player.selectAction(board);
		board = board.next(move);

		clear();
		std::cout << board << std::endl;
		std::cout << "score: " << board.getScore() << "\n\n";		
	}
}

