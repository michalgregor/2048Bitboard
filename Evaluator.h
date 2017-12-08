#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "GameBoard.h"

class ScoreEvaluator {
public:
	float evaluate(const GameBoard& board) const {
		return board.getScore();
	}
};

class HeuristicEvaluator {
private:
	static float _heur_score_table[65536];
	static bool table_initializer;

protected:
	static void init_table();

public:
    static const float* heur_score_table;
	static constexpr float SCORE_LOST_PENALTY = 200000.0f;
	static constexpr float SCORE_MONOTONICITY_POWER = 4.0f;
	static constexpr float SCORE_MONOTONICITY_WEIGHT = 47.0f;
	static constexpr float SCORE_SUM_POWER = 3.5f;
	static constexpr float SCORE_SUM_WEIGHT = 11.0f;
	static constexpr float SCORE_MERGES_WEIGHT = 700.0f;
	static constexpr float SCORE_EMPTY_WEIGHT = 270.0f;

public:
	float evaluate(const GameBoard& board) const {
        return BoardMethods::score_helper(
                    board.getBoardState(), heur_score_table
               ) + BoardMethods::score_helper(
                    board.transpose().getBoardState(), heur_score_table
               );
    }

public:
	~HeuristicEvaluator() {
        //! Makes sure that table_initializer is initialized and init_tables called.
    	(void) table_initializer;
    }
};


#endif // EVALUATOR_H

