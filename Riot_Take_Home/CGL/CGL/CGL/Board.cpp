
#include <cstdint>
#include <stdexcept>

#include "Board.h"

/// <summary>
/// ctor
/// </summary>
/// <param name="b"></param>
/// <param name="v"></param>
Board::BoardVisitor::BoardVisitor(Board* b, Board::Visitor* v) : m_board(b), m_visitor(v)
{
	if (b == nullptr)
		throw std::invalid_argument("board ptr cannot be null");
	if (v == nullptr)
		throw std::invalid_argument("visitor ptr cannot be null");
}

/// <summary>
/// Calls the encapsulated visitor for visited cell
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
/// <returns></returns>
bool Board::BoardVisitor::Visit(int64_t row, int64_t col)
{
	return m_visitor->Visit(*m_board, row, col);
}

/// <summary>
/// Toggle visitor : Toggles the cur state of the visited cell 
/// </summary>
/// <param name="board"></param>
/// <param name="row"></param>
/// <param name="col"></param>
/// <returns></returns>
bool Board::ToggleVisitor::Visit(Board& board, int64_t row, int64_t col)
{
	board.m_curState.Toggle(row, col);
	return true;
}

/// <summary>
/// End of toggle visit
/// </summary>
/// <param name="board"></param>
void Board::ToggleVisitor::OnEnded(Board& board)
{
	// clear all pending toggles
	board.m_toggled.Clear();
}

/// <summary>
/// Remembers a cell to be toggled later
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
void Board::QueueToggle(int64_t row, int64_t col)
{
	m_toggled.Set(row, col, true);
}

/// <summary>
/// Apply all remembered toggles to cur state
/// </summary>
void Board::ApplyToggles()
{
	ToggleVisitor tv;
	Accept(*this, m_toggled, &tv);
}

/// <summary>
/// Initialize a cell to alive
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
void Board::Initialize(int64_t row, int64_t col)
{
	m_curState.Set(row, col, true);
}

/// <summary>
/// Return alive status
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
/// <returns></returns>
bool Board::IsAlive(int64_t row, int64_t col)
{
	return m_curState.IsSet(row, col);
}


/// <summary>
/// Accept a visitor
/// </summary>
/// <param name="visitor"></param>
void Board::Accept(Visitor* visitor)
{
	Accept(*this, m_curState, visitor);
}

/// <summary>
/// Helper function to accept a visitor for a given board state
/// </summary>
/// <param name="board"></param>
/// <param name="bs"></param>
/// <param name="visitor"></param>
void Board::Accept(Board& board, BoardState& bs, Visitor* visitor)
{
	if (visitor == nullptr)
		throw std::invalid_argument("visitor cannot be null");
	BoardVisitor bv(&board, visitor);
	visitor->OnStarted(board);
	bs.Accept(&bv);
	visitor->OnEnded(board);
}

