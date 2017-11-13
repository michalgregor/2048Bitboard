#include "GameBoard.h"

AuxTableBase::row_t AuxTableBase::_row_left_table[65536];
AuxTableBase::row_t AuxTableBase::_row_right_table[65536];
AuxTableBase::board_t AuxTableBase::_col_up_table[65536];
AuxTableBase::board_t AuxTableBase::_col_down_table[65536];
float AuxTableBase::_score_table[65536];
bool AuxTableBase::table_initializer = (init_tables(), true);

const AuxTableBase::board_t AuxTableBase::ROW_MASK = 0xFFFFULL;
const AuxTableBase::board_t AuxTableBase::COL_MASK = 0x000F000F000F000FULL;
const AuxTableBase::row_t* AuxTableBase::row_left_table = AuxTableBase::_row_left_table;
const AuxTableBase::row_t* AuxTableBase::row_right_table = AuxTableBase::_row_right_table;
const AuxTableBase::board_t* AuxTableBase::col_up_table = AuxTableBase::_col_up_table;
const AuxTableBase::board_t* AuxTableBase::col_down_table = AuxTableBase::_col_down_table;
const float* AuxTableBase::score_table = AuxTableBase::_score_table;

void AuxTableBase::init_tables() {
    for (unsigned row = 0; row < 65536; ++row) {
        unsigned line[4] = {
                (row >>  0) & 0xf,
                (row >>  4) & 0xf,
                (row >>  8) & 0xf,
                (row >> 12) & 0xf
        };

        // Score
        float score = 0.0f;
        for(int i = 0; i < 4; ++i) {
            int rank = line[i];
            if (rank >= 2) {
                // the score is the total sum of the tile and all intermediate merged tiles
                score += (rank - 1) * (1 << rank);
            }
        }
        _score_table[row] = score;

        // execute a move to the left
        for (int i = 0; i < 3; ++i) {
            int j;
            for (j = i + 1; j < 4; ++j) {
                if (line[j] != 0) break;
            }
            if (j == 4) break; // no more tiles to the right

            if (line[i] == 0) {
                line[i] = line[j];
                line[j] = 0;
                i--; // retry this entry
            } else if (line[i] == line[j]) {
                if(line[i] != 0xf) {
                    /* Pretend that 32768 + 32768 = 32768 (representational limit). */
                    line[i]++;
                }
                line[j] = 0;
            }
        }

        row_t result = (line[0] <<  0) |
                       (line[1] <<  4) |
                       (line[2] <<  8) |
                       (line[3] << 12);
        row_t rev_result = reverse_row(result);
        unsigned rev_row = reverse_row(row);

        _row_left_table [    row] =                row  ^                result;
        _row_right_table[rev_row] =            rev_row  ^            rev_result;
        _col_up_table   [    row] = unpack_col(    row) ^ unpack_col(    result);
        _col_down_table [rev_row] = unpack_col(rev_row) ^ unpack_col(rev_result);
    }
}

AuxTableBase::~AuxTableBase() {
	//! Makes sure that table_initializer is initialized and init_tables called.
	(void) table_initializer;
}

int BoardMethods::count_empty(board_t x) {
    x |= (x >> 2) & 0x3333333333333333ULL;
    x |= (x >> 1);
    x = ~x & 0x1111111111111111ULL;
    // At this point each nibble is:
    //  0 if the original nibble was non-zero
    //  1 if the original nibble was zero
    // Next sum them all
    x += x >> 32;
    x += x >> 16;
    x += x >>  8;
    x += x >>  4; // this can overflow to the next nibble if there were 16 empty positions
    return x & 0xf;
}

//! Returns all possible next states for the specified action along with
//! their respective probabilities.
std::vector<std::pair<GameBoard, float> > GameBoard::allNexts(GameAction action) const {
	auto det_board = execute_deterministic_move(_board, action);
	if(det_board == _board) return {};

	auto empty = count_empty(det_board);
	std::vector<std::pair<GameBoard, float> > nexts(empty * 2);

	float prob4 = static_cast<float>(PROB4_TIMES_100) / 100.0f;
	float prob2 = 1 - prob4;
	prob4 /= empty;
	prob2 /= empty;

	for(unsigned int i = 0; i < empty*2; i+=2) {
		// tile 2
		nexts[i].first = insert_tile(det_board, 1, i);
		nexts[i].second = prob2;

		// tile 4
		nexts[i+1].first = insert_tile(det_board, 2, i);
		nexts[i+1].second = prob4;
	}

	return nexts;
}

std::ostream& operator<<(std::ostream& os, const GameBoard& obj) {
	auto board = obj.getBoardState();

    for(unsigned int i=0; i<4; i++) {
        for(unsigned int  j=0; j<4; j++) {
            uint8_t powerVal = (board) & 0xf;
			os << ((powerVal == 0) ? 0 : 1 << powerVal);
			os << "\t";
            board >>= 4;
        }
    	os << "\n";
    }
	os << "\n";

	return os;
}

