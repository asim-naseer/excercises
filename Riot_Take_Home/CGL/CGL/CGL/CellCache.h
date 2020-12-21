#pragma once

#include <map>
#include <list>
#include "BoardState.h"

/// <summary>
/// A very hack cell cache - place holder
/// </summary>
class CellCache
{
	private :

		static const size_t DEFAULT_SIZE = 32;

		
		typedef std::list<BoardState::Cell> Refrences;

		struct CellInfo
		{
			Refrences::iterator m_list_it;
			bool m_alive;
			bool m_processed;

			CellInfo(Refrences::iterator it, bool alive) : m_list_it(it), m_alive(alive), m_processed(false) {}
			CellInfo() : m_alive(false), m_processed(false) {}
		};

		typedef std::map<int64_t, CellInfo> ColumnToCell;
		typedef std::map<int64_t, ColumnToCell> RowToColumns;

		RowToColumns m_rows;
		Refrences m_refernces;
		size_t m_maxSize = 32;

		CellInfo& CacheInternal(int64_t row, int64_t col, bool alive);
		CellInfo& Set(int64_t row, int64_t col, Refrences::iterator it, bool alive);
		Refrences::iterator GetListIterator(int64_t row, int64_t col);
		void Clear(int64_t row, int64_t col);


	public:

		CellCache() {}
		CellCache(size_t maxSize) : m_maxSize(maxSize) {}
		virtual ~CellCache() {}
		void Clear(size_t newMaxSize = DEFAULT_SIZE);
		void Cache(int64_t row, int64_t col, bool alive);
		void Processed(int64_t row, int64_t col, bool alive);
		bool TryGetCached(int64_t row, int64_t col, bool& alive);
		bool IsCached(int64_t row, int64_t col);
		bool IsProcessed(int64_t row, int64_t col);


};

