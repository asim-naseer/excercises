#pragma once

#include "Board.h"
#include "CellCache.h"

/// <summary>
/// BoardUpdater - visitor used to update the game pf life
/// </summary>
class BoardUpdater : public Board::Visitor
{
	const static int64_t MAX = std::numeric_limits<int64_t>::max();
	const static int64_t MIN = std::numeric_limits<int64_t>::min();
	const static size_t MAX_CACHE = 25;

	/// <summary>
	/// Returns boundary of neighbour hood centered at row, col;
	/// </summary>
	/// <param name="row"></param>
	/// <param name="col"></param>
	/// <param name="left"></param>
	/// <param name="top"></param>
	/// <param name="right"></param>
	/// <param name="bottom"></param>
	inline static void GetNeighbourhood(int64_t row, int64_t col, int64_t& left, int64_t& top, int64_t& right, int64_t& bottom)
	{
		left = col > MIN ? col - 1 : col;
		top = row > MIN ? row - 1 : row;
		right = col < MAX ? col + 1 : col;
		bottom = row < MAX ? row + 1 : row;
	}

	void UpdateDeadCellStatus(Board& board, int64_t row, int64_t col, int64_t alive_neighbour_row, int64_t alive_neighbour_col);
	void UpdateAliveCellStatus(Board& board, int64_t row, int64_t col);
	bool IsAlive(Board& board, int64_t row, int64_t col);

	CellCache m_cellCache;

public:

	BoardUpdater();
	virtual ~BoardUpdater() {}

	void OnStarted(Board& board) override;
	bool Visit(Board& board, int64_t row, int64_t col) override;
	void OnEnded(Board& board) override;
};

