#pragma once

#include <assert.h>
#include "BoardState.h"

/// <summary>
/// Game of life board container
/// </summary>
class Board
{
	public:

		/// <summary>
		/// Visitor base
		/// </summary>
		class Visitor
		{
			public:
				Visitor() {}
				virtual void OnStarted(Board& board) {}
				virtual void OnEnded(Board& board) {}
				virtual bool Visit(Board& board, int64_t row, int64_t col) = 0;
				virtual ~Visitor() {}

		};


	private:

		/// <summary>
		/// Board state visitor. Enacapsulates board and Board::Visitor
		/// </summary>
		class BoardVisitor : public BoardState::Visitor
		{
			private:
				
				Board* m_board = nullptr;
				Board::Visitor* m_visitor = nullptr;

			public:

				BoardVisitor(Board* b, Board::Visitor* v);

				virtual bool Visit(int64_t row, int64_t col);
		};

		/// <summary>
		/// State toggle visitor. Applies the pending toggles
		/// </summary>
		class ToggleVisitor : public Visitor
		{
			friend class Board;

			public:

				virtual bool Visit(Board& board, int64_t row, int64_t col);
				virtual void OnEnded(Board& board);
		};

		BoardState m_curState;
		BoardState m_toggled;	// Contains only cells whose state is pending toggle in m_curState

		static void Accept(Board& board, BoardState& bs, Visitor* visitor);

	public:

		Board() {}

		inline size_t Size()
		{
			m_curState.Size();
		}

		inline void Clear()
		{
			m_curState.Clear();
			m_toggled.Clear();
		}

		// Add a pending toggle for cur state
		void QueueToggle(int64_t row, int64_t col);
		// Apply all pending toggle for cur state
		void ApplyToggles();
		// Initialize a cell address to alive
		void Initialize(int64_t row, int64_t col);

		bool IsAlive(int64_t row, int64_t col);

		// Accept a visitor
		void Accept(Visitor* visitor);
};

