#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "system.h"
#include <ostream>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <cmath>

// We could use boost_multiprecision to generalize this to support greater
// values -- if we do want to achieve the 65536 tile.
//
// In that case it would be best to switch to the larger integer type only
// once the 32768 tile has been achieved -- so as to keep computations up to
// that point fast. However, that might be quite difficult to accomplish.
//
//#include <boost/multiprecision/cpp_int.hpp>

/* The fundamental trick: the 4x4 board is represented as a 64-bit word,
 * with each board square packed into a single 4-bit nibble.
 * 
 * The maximum possible board value that can be supported is 32768 (2^15), but
 * this is a minor limitation as achieving 65536 is highly unlikely under normal circumstances.
 * 
 * The space and computation savings from using this representation should be significant.
 * 
 * The nibble shift can be computed as (r,c) -> shift (4*r + c). That is, (0,0) is the LSB.
 */

class AuxTableBase {
public:
	typedef uint64_t board_t;
	typedef uint16_t row_t;

private:
	/**
	 * We can perform state lookups one row at a time by using arrays with
	 * 65536 entries.
	 *
	 * Move tables. Each row or compressed column is mapped to (oldrow^newrow)
	 * assuming row/col 0. Thus, the value is 0 if there is no move, and
	 * otherwise equals a value that can easily be xor'ed into the current
	 * board state to update the board.
	 *
	 * The non-const versions of the tables are private so that they do not
	 * accidentally get corrupted.
	**/
	static row_t _row_left_table[65536];
	static row_t _row_right_table[65536];
	static board_t _col_up_table[65536];
	static board_t _col_down_table[65536];
	static float _score_table[65536];

	//! A helper member used to initialize all the tables.
	static bool table_initializer;

private:
	static void init_tables();

public:
	static const row_t* row_left_table;
	static const row_t* row_right_table;
	static const board_t* col_up_table;
	static const board_t* col_down_table;
	static const float* score_table;

	static const board_t ROW_MASK;
	static const board_t COL_MASK;

	static constexpr unsigned int ROWS = 4;
	static constexpr unsigned int COLS = 4;

public:
	static inline board_t unpack_col(row_t row) {
		board_t tmp = row;
		return (tmp | (tmp << 12ULL) | (tmp << 24ULL) | (tmp << 36ULL)) & COL_MASK;
	}

	static inline row_t reverse_row(row_t row) {
		return (row >> 12) | ((row >> 4) & 0x00F0)  | ((row << 4) & 0x0F00) | (row << 12);
	}

public:
	~AuxTableBase();
};

class BoardMethods: public AuxTableBase {
protected:
	static float score_helper(board_t board, const float* table) {
		return table[(board >>  0) & ROW_MASK] +
		       table[(board >> 16) & ROW_MASK] +
		       table[(board >> 32) & ROW_MASK] +
		       table[(board >> 48) & ROW_MASK];
	}

public:
	/**
	 * Transpose rows/columns in a board:
	 *   0123       048c
	 *   4567  -->  159d
	 *   89ab       26ae
	 *   cdef       37bf
	**/
	static inline board_t transpose_board(board_t x) {
		board_t a1 = x & 0xF0F00F0FF0F00F0FULL;
		board_t a2 = x & 0x0000F0F00000F0F0ULL;
		board_t a3 = x & 0x0F0F00000F0F0000ULL;
		board_t a = a1 | (a2 << 12) | (a3 >> 12);
		board_t b1 = a & 0xFF00FF0000FF00FFULL;
		board_t b2 = a & 0x00FF00FF00000000ULL;
		board_t b3 = a & 0x00000000FF00FF00ULL;
		return b1 | (b2 >> 24) | (b3 << 24);
	}

	// Count the number of empty positions (= zero nibbles) in a board.
	// Precondition: the board cannot be fully empty.
	static int count_empty(board_t x);

	static inline int max_rank(board_t board) {
		int maxrank = 0;
		while(board) {
		    maxrank = std::max(maxrank, int(board & 0xf));
		    board >>= 4;
		}
		return maxrank;
	}

	static inline int count_distinct_tiles(board_t board) {
		uint16_t bitset = 0;
		while (board) {
		    bitset |= 1<<(board & 0xf);
		    board >>= 4;
		}

		// Don't count empty tiles.
		bitset >>= 1;

		int count = 0;
		while (bitset) {
		    bitset &= bitset - 1;
		    count++;
		}
		return count;
	}

	static board_t draw_tile() {
		return (unif_random(10) < 9) ? 1 : 2;
	}

	static board_t insert_tile_rand(board_t board, board_t tile) {
		int index = unif_random(count_empty(board));
		board_t tmp = board;
		while (true) {
		    while ((tmp & 0xf) != 0) {
		        tmp >>= 4;
		        tile <<= 4;
		    }
		    if (index == 0) break;
		    --index;
		    tmp >>= 4;
		    tile <<= 4;
		}
		return board | tile;
	}

	static board_t make_init_board() {
	    board_t board = draw_tile() << (4 * unif_random(16));
		return insert_tile_rand(board, draw_tile());
	}

public:
	static float score_board(board_t board) {
		return score_helper(board, score_table);
	}

	static inline board_t execute_up(board_t board) {
		board_t ret = board;
		board_t t = transpose_board(board);
		ret ^= col_up_table[(t >>  0) & ROW_MASK] <<  0;
		ret ^= col_up_table[(t >> 16) & ROW_MASK] <<  4;
		ret ^= col_up_table[(t >> 32) & ROW_MASK] <<  8;
		ret ^= col_up_table[(t >> 48) & ROW_MASK] << 12;
		return ret;
	}

	static inline board_t execute_down(board_t board) {
		board_t ret = board;
		board_t t = transpose_board(board);
		ret ^= col_down_table[(t >>  0) & ROW_MASK] <<  0;
		ret ^= col_down_table[(t >> 16) & ROW_MASK] <<  4;
		ret ^= col_down_table[(t >> 32) & ROW_MASK] <<  8;
		ret ^= col_down_table[(t >> 48) & ROW_MASK] << 12;
		return ret;
	}

	static inline board_t execute_left(board_t board) {
		board_t ret = board;
		ret ^= board_t(row_left_table[(board >>  0) & ROW_MASK]) <<  0;
		ret ^= board_t(row_left_table[(board >> 16) & ROW_MASK]) << 16;
		ret ^= board_t(row_left_table[(board >> 32) & ROW_MASK]) << 32;
		ret ^= board_t(row_left_table[(board >> 48) & ROW_MASK]) << 48;
		return ret;
	}

	static inline board_t execute_right(board_t board) {
		board_t ret = board;
		ret ^= board_t(row_right_table[(board >>  0) & ROW_MASK]) <<  0;
		ret ^= board_t(row_right_table[(board >> 16) & ROW_MASK]) << 16;
		ret ^= board_t(row_right_table[(board >> 32) & ROW_MASK]) << 32;
		ret ^= board_t(row_right_table[(board >> 48) & ROW_MASK]) << 48;
		return ret;
	}

	static inline board_t set_element(board_t board, unsigned int row,
		unsigned int col, unsigned int val)
	{
		if((val & (val - 1)) != 0) throw std::runtime_error("The value must be zero or power of two.");
		if(val > 32768) throw std::runtime_error("The value cannot be greater than 32768.");
	
		auto shift = (col + row * COLS) * 4;
		board &= ~(board_t(0xf) << shift);

		board_t powerVal = std::log2(val);
		board |= powerVal << shift;

		return board;
	}

	static inline board_t get_element(board_t board, unsigned int row, unsigned int col) {
		auto numel = col + row * COLS;
		uint8_t powerVal = (board >> numel*4) & 0xf;
		return (powerVal == 0) ? 0 : 1 << powerVal;
	}
};

class GameBoard: public BoardMethods {
public:
	using AuxTableBase::board_t;
	using AuxTableBase::row_t;

	enum GameAction: uint8_t {
		None = 0,
		UP = 1,
		DOWN = 2,
		LEFT = 3,
		RIGHT = 4
	};

private:
	board_t _board;

	// cache
	mutable board_t _up = 0;
	mutable board_t _down = 0;
	mutable board_t _left = 0;
	mutable board_t _right = 0;
	mutable std::vector<GameAction> _legals;

protected:
	void clearCache() const {
		_up = 0; _down = 0; _left = 0; _right = 0;
		_legals.clear(); _legals.reserve(4);
	}

protected:
	inline board_t execute_deterministic_move(GameAction action) const {
		switch(action) {
		case UP:
			if(!_up) _up = execute_up(_board);
			return _up;
		break;
		case DOWN:
			if(!_down) _down = execute_down(_board);
			return _down;
		break;
		case LEFT:
			if(!_left) _left = execute_left(_board);
			return _left;
		break;
		case RIGHT:
			if(!_right) _right = execute_right(_board);
			return _right;
		break;
		case None:
		break;
		default:
			throw std::runtime_error("Unknown action " + std::to_string(action) + ".");
		}

		// this should never be reached, it is just to suppress warnings
		return _board;
	}

public:
	inline int emptyCount() const {return count_empty(_board);}
	inline int maxRank() const {return max_rank(_board);}
	inline int distinctTilesCount() const {return count_distinct_tiles(_board);}
	inline GameBoard transpose() const {return transpose_board(_board);}

	inline void move(GameAction action) {
		auto new_board = execute_deterministic_move(action);
		if(new_board == _board) {
			/** Do we throw exceptions on illegal actions? **/
		} else {
			_board = insert_tile_rand(new_board, draw_tile());
			clearCache();
		}
	}

	inline float getScore() const {return score_board(_board);}

	inline board_t getBoardState() const {return _board;}
	inline void setBoardState(board_t boardState) {_board = boardState; clearCache();}

	//! Puts the board into its starting position, first clearning all bits and
	//! the randomly generating a new starting state.
	inline void initBoard() {_board = make_init_board(); clearCache();}

public:
	unsigned int rows() const {return ROWS;}
	unsigned int cols() const {return COLS;}

	inline unsigned int operator()(unsigned int row, unsigned int col) const {
		return get_element(_board, row, col);
	}

	unsigned int at(unsigned int row, unsigned int col) const {
		if(row >= rows() or col >= cols()) throw std::runtime_error("Index overflow.");
		return get_element(_board, row, col);
	}




	const std::vector<GameAction>& legalActions() const {
		if(_legals.size() or !_legals.capacity()) return _legals;

		if(!_up) _up = execute_up(_board);
		if(_board != _up) _legals.push_back(UP);

		if(!_down) _down = execute_down(_board);
		if(_board != _down) _legals.push_back(DOWN);

		if(!_left) _left = execute_left(_board);
		if(_board != _left) _legals.push_back(LEFT);

		if(!_right) _right = execute_right(_board);
		if(_board != _right) _legals.push_back(RIGHT);

		if(!_legals.size()) {
			_legals.clear();
			_legals.shrink_to_fit();
		}

		return _legals;
	}
	
	bool isGameOver() const {
		return legalActions().empty();
	}





public:
	GameBoard& operator=(const GameBoard& obj) {
		_board = obj._board;



		clearCache();



		return *this;
	}

	GameBoard(const GameBoard& obj): _board(obj._board), _legals() {
		_legals.reserve(4);
	}



	GameBoard(board_t state): _board(state), _legals() {
		_legals.reserve(4);
	}

	GameBoard(): _board(make_init_board()), _legals() {
		_legals.reserve(4);
	}
};

std::ostream& operator<<(std::ostream& os, const GameBoard& obj);

#endif // GAMEBOARD_H

