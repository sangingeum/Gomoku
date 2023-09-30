#include "Gomoku.hpp"
Gomoku::Gomoku()
{
	reset();
}
// true: White, false: Black
bool Gomoku::getTurn() const {
	return m_turn;
}
uint8_t Gomoku::getLastWhiteAction() const {
	return m_lastWhiteAction;
}
uint8_t Gomoku::getLastBlackAction() const {
	return m_lastBlackAction;
}

void Gomoku::reset() {
	for (auto& piece : m_board)
		piece = 0;
	// 0  == Empty
	// -1 == Black
	// 1 == White
	m_turn = false; // black first
	m_lastBlackAction = PIECE_NUM >> 1;
	m_lastWhiteAction = m_lastBlackAction;
}

bool Gomoku::putPiece(uint8_t index) {
	if (m_turn && m_board[index] == 0) {
		m_board[index] = 1;
		m_turn = false;
		m_lastWhiteAction = index;
	}
	else if ((!m_turn) && m_board[index] == 0) {
		m_board[index] = -1;
		m_turn = true;
		m_lastBlackAction = index;
	}
	else {
		return false;
	}
	return true;
}

// 0 == not over, 1 == white win, -1 == black win
int Gomoku::isOver(uint8_t index) const {
	return isOver(m_board, index);
}

uint8_t Gomoku::minimax(int depth) const {
	return minimax(m_board, m_turn, depth, getLastBlackAction(), getLastWhiteAction());
}

int Gomoku::evaluate() const {
	return evaluate(m_board);
}


uint8_t Gomoku::minimax(Board board, bool max, int depth, uint8_t index1, uint8_t index2) {
	std::vector<uint8_t> nextMoves = getNextMoves(board, index1, index2);
	int alpha = std::numeric_limits<int>::min();
	int beta = std::numeric_limits<int>::max();
	static std::random_device device;
	static std::mt19937 gen(device());
	std::shuffle(nextMoves.begin(), nextMoves.end(), gen);
	uint8_t bestMove = 250;
	if (max) {
		for (const auto& move : nextMoves) {
			board[move] = 1;
			int value = minimaxHelper(board, !max, alpha, beta, depth - 1, move);
			if (alpha < value) {
				alpha = value;
				bestMove = move;
			}
			board[move] = 0;
			if (alpha >= beta)
				break;
		}
	}
	else {
		for (const auto& move : nextMoves) {
			board[move] = -1;
			int value = minimaxHelper(board, !max, alpha, beta, depth - 1, move);
			if (beta > value) {
				beta = value;
				bestMove = move;
			}
			board[move] = 0;
			if (alpha >= beta)
				break;
		}
	}
	return bestMove;
}

// Given a board and the lastAction position, checks if the game is over
// Returns an integet between -1 and 1
// -1 : black win, 0: not over, 1: white win
int Gomoku::isOver(const Board& board, uint8_t index) {
	int8_t type = board[index];
	uint8_t col = index % SQRT_PIECE_NUM, row = index / SQRT_PIECE_NUM;
	uint8_t right = SQRT_PIECE_NUM - col - 1, left = SQRT_PIECE_NUM;
	// horizontal
	int count = 1;
	for (int i = 1; (i + col) < SQRT_PIECE_NUM; ++i) {
		if (board[index + i] == type) {
			++count;
		}
		else {
			break;
		}
	}
	for (int i = -1; (i + col) >= 0; --i) {
		if (board[index + i] == type) {
			++count;
		}
		else {
			break;
		}
	}
	if (count >= 5)
		return type;
	// vertical
	count = 1;
	for (int i = 1; (i + row) < SQRT_PIECE_NUM; ++i) {
		if (board[index + i * SQRT_PIECE_NUM] == type) {
			++count;
		}
		else {
			break;
		}
	}
	for (int i = -1; (i + row) >= 0; --i) {
		if (board[index + i * SQRT_PIECE_NUM] == type) {
			++count;
		}
		else {
			break;
		}
	}
	if (count >= 5)
		return type;
	// diagonal (right-down, left-up)
	count = 1;
	int adder = SQRT_PIECE_NUM + 1;
	int sum = index + adder;
	for (int i = 0; (i < right) && (sum < PIECE_NUM); ++i) {
		if (board[sum] == type) {
			++count;
		}
		else {
			break;
		}
		sum += adder;
	}
	sum = index - adder;
	for (int i = 0; (i < left) && (sum >= 0); ++i) {
		if (board[sum] == type) {
			++count;
		}
		else {
			break;
		}
		sum -= adder;
	}
	if (count >= 5)
		return type;
	// diagonal 2 (right-up, left-down)
	count = 1;
	adder = SQRT_PIECE_NUM - 1;
	sum = index + adder;
	for (int i = 0; (i < right) && (sum < PIECE_NUM); ++i) {
		if (board[sum] == type) {
			++count;
		}
		else {
			break;
		}
		sum += adder;
	}
	sum = index - adder;
	for (int i = 0; (i < left) && (sum >= 0); ++i) {
		if (board[sum] == type) {
			++count;
		}
		else {
			break;
		}
		sum -= adder;
	}
	if (count >= 5)
		return type;

	return 0;
}

// Return every empty positions
std::vector<uint8_t> Gomoku::getNextMoves(const Board& board) {
	std::vector<uint8_t> moves;
	moves.reserve(PIECE_NUM); // May allocate unnecessary memory
	for (uint8_t i = 0; i < PIECE_NUM; ++i) {
		if (board[i] == 0) {
			moves.push_back(i);
		}
	}
	return moves;
}

// Given two positions and a board, return positions near them
std::vector<uint8_t> Gomoku::getNextMoves(const Board& board, uint8_t index1, uint8_t index2, uint8_t maxLen) {
	std::vector<uint8_t> moves;
	uint8_t row1 = index1 / SQRT_PIECE_NUM, col1 = index1 % SQRT_PIECE_NUM;
	uint8_t row2 = index2 / SQRT_PIECE_NUM, col2 = index2 % SQRT_PIECE_NUM;
	moves.reserve(PIECE_NUM); // May allocate unnecessary memory
	uint8_t offset = 0;
	for (uint8_t i = 0; i < SQRT_PIECE_NUM; ++i) {
		for (uint8_t j = 0; j < SQRT_PIECE_NUM; ++j) {
			if ((board[j + offset] == 0) && ((abs(row1 - i) + abs(col1 - j) <= maxLen) || (abs(row2 - i) + abs(col2 - j) <= maxLen))) {
				moves.push_back(j + offset);
			}
		}
		offset += SQRT_PIECE_NUM;
	}
	if (moves.empty())
		return getNextMoves(board);
	return moves;
}

// Evaluate the given board
int Gomoku::evaluate(const Board& board) {
	return evaluateHelper(board, 1) + evaluateHelper(board, -1);
}

int Gomoku::minimaxHelper(Board& board, bool max, int alpha, int beta, int depth, uint8_t lastAction) {
	int over = isOver(board, lastAction);
	if (over)
		return over == 1 ? 100000 : -100000;
	if (depth <= 0)
		return evaluate(board);

	auto nextMoves = getNextMoves(board);
	static std::random_device device;
	static std::mt19937 gen(device());
	std::shuffle(nextMoves.begin(), nextMoves.end(), gen);
	if (max) {
		for (auto move : nextMoves) {
			board[move] = 1;
			alpha = std::max(alpha, int(minimaxHelper(board, !max, alpha, beta, depth - 1, move)));
			board[move] = 0;
			if (alpha >= beta)
				break;
		}
		return alpha;
	}
	else {
		for (auto move : nextMoves) {
			board[move] = -1;
			beta = std::min(beta, int(minimaxHelper(board, !max, alpha, beta, depth - 1, move)));
			board[move] = 0;
			if (alpha >= beta)
				break;
		}
		return beta;
	}
}

int Gomoku::calculateScore(int8_t length, int8_t closedCount) {
	static int scoreBoard[6][3] = {
		{0, 0, 0},
		{0, 0, 0},
		{40, 30, 30},
		{400, 60, 60},
		{1500, 300, 300},
	};
	return scoreBoard[std::min(length, int8_t(5))][closedCount];
}

inline int Gomoku::scoreHelper(int8_t cur, int8_t type, int8_t& closedCounter, int8_t& stoneCounter) {
	int score = 0;
	if (cur == type) {
		++stoneCounter;
	}
	else if (cur == 0) {
		score += calculateScore(stoneCounter, closedCounter);
		closedCounter = 0;
		stoneCounter = 0;
	}
	else {
		++closedCounter;
		score += calculateScore(stoneCounter, closedCounter);
		closedCounter = 1;
		stoneCounter = 0;
	}
	return score;
}

inline int Gomoku::diagonalScoreHelper(const Board& board, uint8_t index, int8_t type, int adder, int8_t length) {
	int score = 0;
	int offset = 0;
	int8_t closedCounter = 1;
	int8_t stoneCounter = 0;
	for (int i = 0; i < length; ++i) {
		auto cur = board[index + offset];
		score += scoreHelper(cur, type, closedCounter, stoneCounter);
		offset += adder;
	}
	++closedCounter;
	score += calculateScore(stoneCounter, closedCounter);
	return score;
}

// Given a board and a type, returns the score of the `type` team
// type == 1 or -1
int Gomoku::evaluateHelper(const Board& board, int8_t type) {
	int score = 0;
	int offset = 0;
	// Horizontal
	for (int i = 0; i < SQRT_PIECE_NUM; ++i) {
		int8_t closedCounter = 1;
		int8_t stoneCounter = 0;
		for (int j = 0; j < SQRT_PIECE_NUM; ++j) {
			auto cur = board[j + offset];
			score += scoreHelper(cur, type, closedCounter, stoneCounter);
		}
		// consider the wall at the end
		++closedCounter;
		score += calculateScore(stoneCounter, closedCounter);
		// update offset
		offset += SQRT_PIECE_NUM;
	}
	// vertical
	for (int i = 0; i < SQRT_PIECE_NUM; ++i) {
		int8_t closedCounter = 1;
		int8_t stoneCounter = 0;
		int offset = 0;
		for (int j = 0; j < SQRT_PIECE_NUM; ++j) {
			auto cur = board[offset + i];
			score += scoreHelper(cur, type, closedCounter, stoneCounter);
			// update offset
			offset += SQRT_PIECE_NUM;
		}
		// consider the wall at the end
		++closedCounter;
		score += calculateScore(stoneCounter, closedCounter);
	}
	// diagonal 1 ¢Ù
	int adder = SQRT_PIECE_NUM + 1;
	for (int i = 0; i < SQRT_PIECE_NUM; ++i) {
		score += diagonalScoreHelper(board, i, type, adder, SQRT_PIECE_NUM - i);
	}
	for (int i = 1, index = SQRT_PIECE_NUM; i < SQRT_PIECE_NUM; ++i) {
		score += diagonalScoreHelper(board, index, type, adder, SQRT_PIECE_NUM - i);
		index += SQRT_PIECE_NUM;
	}
	// diagonal 2 ¢×
	adder = SQRT_PIECE_NUM - 1;
	for (int i = 0; i < SQRT_PIECE_NUM; ++i) {
		score += diagonalScoreHelper(board, i, type, adder, i + 1);
	}
	for (int i = 1, index = SQRT_PIECE_NUM - 1; i < SQRT_PIECE_NUM; ++i) {
		index += SQRT_PIECE_NUM;
		score += diagonalScoreHelper(board, index, type, adder, SQRT_PIECE_NUM - i);
	}
	return ((type == 1) ? score : -score);
}
