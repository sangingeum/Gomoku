#pragma once
#include <vector>
#include <array>
#include <limits>
#include <iostream>
#include <format>
#include <algorithm>
#include <random>

#define PIECE_NUM 196
#define SQRT_PIECE_NUM 14

class Gomoku
{
private:
	using Board = typename std::array<int8_t, PIECE_NUM>;
	Board m_board;
	bool m_turn; // True: white, False: black
	uint8_t m_lastWhiteAction;
	uint8_t m_lastBlackAction;
public:
	Gomoku();
	bool getTurn() const;
	uint8_t getLastWhiteAction() const;
	uint8_t getLastBlackAction() const;
	void reset();
	bool putPiece(uint8_t index);
	int isOver(uint8_t index) const;
	uint8_t minimax(int depth) const;
	int evaluate() const;
private:
	static uint8_t minimax(Board board, bool max, int depth, uint8_t index1, uint8_t index2);
	static int isOver(const Board& board, uint8_t index);
	static std::vector<uint8_t> getNextMoves(const Board& board);
	static std::vector<uint8_t> getNextMoves(const Board& board, uint8_t index1, uint8_t index2, uint8_t maxLen = 5);
	static int evaluate(const Board& board);
	static int minimaxHelper(Board& board, bool max, int alpha, int beta, int depth, uint8_t lastAction);
	static int calculateScore(int8_t length, int8_t closedCount);
	inline static int scoreHelper(int8_t cur, int8_t type, int8_t& closedCounter, int8_t& stoneCounter);
	inline static int diagonalScoreHelper(const Board& board, uint8_t index, int8_t type, int adder, int8_t length);
	static int evaluateHelper(const Board& board, int8_t type);

};

