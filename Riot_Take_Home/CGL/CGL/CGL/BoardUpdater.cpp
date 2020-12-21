
#include <cstdint>
#include <iostream>
#include <queue>

#include "BoardUpdater.h"

/// <summary>
/// For a given dead cell at row, col update its status if needed.
/// </summary>
/// <param name="board"></param>
/// <param name="row"></param>
/// <param name="col"></param>
/// <param name="alive_neighbour_row"></param>
/// <param name="alive_neighbour_col"></param>
void BoardUpdater::UpdateDeadCellStatus(Board& board, int64_t row, int64_t col, int64_t alive_neighbour_row, int64_t alive_neighbour_col)
{
	if (m_cellCache.IsProcessed(row, col))
		return;
	m_cellCache.Processed(row, col, false);
#ifdef _DEBUG
	std::list<BoardState::Cell> neighbours;
	neighbours.push_back(BoardState::Cell(alive_neighbour_row, alive_neighbour_col));
#endif

	int64_t left, top, right, bottom;
	GetNeighbourhood(row, col, left, top, right, bottom);
	int countAlive = 1; // We have at least one alive neighbor
	for (int64_t r = top; r <= bottom; ++r)
	{
		for (int64_t c = left; c <= right; ++c)
		{
			if (r == row && c == col)
				continue;
			if (r == alive_neighbour_row && c == alive_neighbour_col)
				continue;
			if (IsAlive(board, r, c))
			{
#ifdef _DEBUG
				neighbours.push_back(BoardState::Cell(r, c));
#endif
				++countAlive;
			}
		}
	}
	if (countAlive == 3)
	{
		board.QueueToggle(row, col); // Bring to life
#ifdef _DEBUG
		std::cout << "BORN: " << row << ' ' << col << " Neighbors: " << countAlive << '\n';
		for (const auto& c : neighbours)
		{
			std::cout << "\t\tNeighbour: " << c.m_row << ' ' << c.m_col << '\n';
		}
#endif
	}
}

/// <summary>
/// For a given alive cell at row, col update its status if needed
/// </summary>
/// <param name="board"></param>
/// <param name="row"></param>
/// <param name="col"></param>
void BoardUpdater::UpdateAliveCellStatus(Board& board, int64_t row, int64_t col)
{
#ifdef _DEBUG
	std::list<BoardState::Cell> neighbours;
#endif

	int64_t left, top, right, bottom;
	GetNeighbourhood(row, col, left, top, right, bottom);
	int countAlive = 0;
	for (int64_t r = top; r <= bottom; ++r)
	{
		for (int64_t c = left; c <= right; ++c)
		{
			if (r == row && c == col)
				continue;
			if (IsAlive(board, r, c))
			{
#ifdef _DEBUG
				neighbours.push_back(BoardState::Cell(r, c));
#endif
				++countAlive;
			}
			else
			{
				UpdateDeadCellStatus(board, r, c, row, col);
			}
		}
	}
	if (countAlive < 2 || countAlive > 3)
	{
		board.QueueToggle(row, col); // Kill it
#ifdef _DEBUG
		std::cout << "DIED: " << row << ' ' << col << " Neighbors: " << countAlive << '\n';
		for (const auto& c : neighbours)
		{
			std::cout << "\t\tNeighbour: " << c.m_row << ' ' << c.m_col << '\n';
		}
#endif
	}
}

/// <summary>
/// Returns status of cell
/// </summary>
/// <param name="board"></param>
/// <param name="row"></param>
/// <param name="col"></param>
/// <returns></returns>
bool BoardUpdater::IsAlive(Board& board, int64_t row, int64_t col)
{
	bool alive = false;
	if (!m_cellCache.TryGetCached(row, col, alive))
	{
		alive = board.IsAlive(row, col);
		m_cellCache.Cache(row, col, alive);
	}
	return alive;
}

BoardUpdater::BoardUpdater()
{
}

/// <summary>
/// Called on visit start
/// </summary>
/// <param name="board"></param>
void BoardUpdater::OnStarted(Board& board)
{
	m_cellCache.Clear();
}

/// <summary>
/// Call back function called for each alive cell during visit of board.
/// </summary>
/// <param name="board"></param>
/// <param name="row"></param>
/// <param name="col"></param>
/// <returns></returns>
bool BoardUpdater::Visit(Board& board, int64_t row, int64_t col)
{
#ifdef _DEBUG
	std::cout << "Update: " << row << ' ' << col << '\n';
#endif
	UpdateAliveCellStatus(board, row, col);
	return true;
}

/// <summary>
/// Called on visit ended
/// </summary>
/// <param name="board"></param>
void BoardUpdater::OnEnded(Board& board)
{
	// Apply any pending cell state changes
	board.ApplyToggles();
	m_cellCache.Clear();
}
