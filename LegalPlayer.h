#ifndef LEGAL_PLAYER_H
#define LEGAL_PLAYER_H

#include "GameBoard.h"
#include <vector>
#include <random>

class LegalPlayer {
public:
	GameBoard::GameAction selectAction(const GameBoard& gameState) {
		auto legals = gameState.legalActions();

		std::uniform_int_distribution<unsigned int> rand_action(0, legals.size()-1);
		auto action = rand_action(default_generator());
	
		return legals.at(action);
	}
};

#endif // LEGAL_PLAYER_H
