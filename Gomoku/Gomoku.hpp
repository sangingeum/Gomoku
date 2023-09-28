#pragma once
#include <vector>
#include <array>
// 14*14
class Gomoku
{
private:
	using Board = typename std::array<int8_t, 196>;
	Board m_board;
	bool m_turn;
public:
	Gomoku()
	{
		reset();
	}
	void reset() {
		for (auto& piece : m_board)
			piece = 0; 
		// 0  == Empty
		// -1 == Black
		// 1 == White
		m_turn = false; // black first
	}
	bool putPiece(uint8_t index) {
		if (m_turn && m_board[index] == 0) {
			m_board[index] = 1;
			m_turn = false;
		}
		else if ((!m_turn) && m_board[index] == 0) {
			m_board[index] = -1;
			m_turn = true;
		}
		else {
			return false;
		}
		return true;
	}
	int isOver(uint8_t index) {
		int8_t type = m_board[index];
		uint8_t col = index % 14, row = index / 14;

		// 가로
		int count = 1;
		for (int i = 1; (i + col) < 14; ++i) {
			if (m_board[index + i] == type) {
				++count;
			}
			else {
				break;
			}
		}
		for (int i = - 1; (i + col) >= 0; --i) {
			if (m_board[index + i] == type) {
				++count;
			}
			else {
				break;
			}
		}
		if (count >= 5)
			return type;
		// 세로
		count = 1;
		for (int i = 1; (i + row) < 14; ++i) {
			if (m_board[index + i * 14] == type) {
				++count;
			}
			else {
				break;
			}
		}
		for (int i = - 1; (i + row) >= 0; --i) {
			if (m_board[index + i * 14] == type) {
				++count;
			}
			else {
				break;
			}
		}
		if (count >= 5)
			return type;
		return 0;
	}


	int isOver() {
		for (int i = 0; i < 14; ++i) {
			uint8_t offset = i * 14;
			int8_t prev = m_board[offset];
			int count = prev ? 1 : 0;
			for (int j = 1; j < 14; ++j) {
				int8_t cur = m_board[j + offset];
				if (cur) {
					if (cur == prev) {
						++count;
						if (count == 5) {
							return cur;
						}
					}
					else {
						count = 1;
					}
				}
				else {
					count = 0;
				}
				prev = cur;
			}
		}

		for (int j = 0; j < 14; ++j) {
			int8_t prev = m_board[j];
			int count = prev ? 1 : 0;
			for (int i = 1; i < 14; ++i) {
				int8_t cur = m_board[i * 14 + j];
				if (cur) {
					if (cur == prev) {
						++count;
						if (count == 5) {
							return cur;
						}
					}
					else {
						count = 1;
					}
				}
				else {
					count = 0;
				}
				prev = cur;
			}
		}

		return 0;
	}

	static std::vector<uint8_t> nextMoves(Board& board) {
		std::vector<uint8_t> moves;
		for (uint8_t i = 0; i < 196; ++i) {
			if (board[i] == 0) {
				moves.push_back(i);
			}
		}
		return moves;
	}

	//static std::vector<Board> makeChildren(Board& board) {
	//}
	
	static int evaluate(Board& board) {
		return 100;
	}
private:


};

