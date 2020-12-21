
#include <algorithm>
#include "CellCache.h"

/// <summary>
/// Clears the cache
/// </summary>
/// <param name="newMaxSize"></param>
void CellCache::Clear(size_t newMaxSize)
{
	m_rows.clear();
	m_refernces.clear();
	m_maxSize = newMaxSize;
}


/// <summary>
/// Set the cache
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
/// <param name="it"></param>
/// <param name="alive"></param>
CellCache::CellInfo& CellCache::Set(int64_t row, int64_t col, Refrences::iterator it, bool alive)
{
	CellInfo& info = m_rows[row][col];
	info = CellInfo(it, alive);
	return info;
}

/// <summary>
/// 
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
/// <returns></returns>
CellCache::Refrences::iterator CellCache::GetListIterator(int64_t row, int64_t col)
{
	auto it = m_rows.find(row);
	if (it != m_rows.end())
	{
		ColumnToCell& columnToCell = it->second;
		auto cell_it = columnToCell.find(col);
		if (cell_it != columnToCell.end())
		{
			CellInfo& cellInfo = cell_it->second;
			return cellInfo.m_list_it;
		}
	}
	return m_refernces.end();
}

/// <summary>
/// Clear from cache
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
void CellCache::Clear(int64_t row, int64_t col)
{
	auto it = m_rows.find(row);
	if (it != m_rows.end())
	{
		ColumnToCell& columnToCell = it->second;
		auto cell_it = columnToCell.find(col);
		if (cell_it != columnToCell.end())
		{
			CellInfo& cellInfo = cell_it->second;
			m_refernces.erase(cellInfo.m_list_it);

			columnToCell.erase(cell_it);
			if (columnToCell.empty())
			{
				m_rows.erase(it);
			}
		}
	}
}

CellCache::CellInfo& CellCache::CacheInternal(int64_t row, int64_t col, bool alive)
{
	if (IsCached(row, col))
	{
		// move to start of list
		auto list_it = GetListIterator(row, col);
		if (list_it != m_refernces.begin())
			m_refernces.splice(m_refernces.begin(), m_refernces, list_it, std::next(list_it));
	}
	else
	{
		if (m_refernces.size() >= m_maxSize)
		{
			BoardState::Cell cell = m_refernces.back();
			Clear(cell.m_row, cell.m_col);
		}
		m_refernces.push_front(BoardState::Cell(row, col));
	}
	return Set(row, col, m_refernces.begin(), alive);
}


/// <summary>
/// Cache a cell status
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
/// <param name="alive"></param>
void CellCache::Cache(int64_t row, int64_t col, bool alive)
{
	CacheInternal(row, col, alive);
}

///
void CellCache::Processed(int64_t row, int64_t col, bool alive)
{
	CellCache::CellInfo& info = CacheInternal(row, col, alive);
	info.m_processed = true;
}

/// <summary>
/// Try get cell status from cache
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
/// <param name="alive">:returned status if cell was cached</param>
/// <returns>return true if cache had cell status</returns>
bool CellCache::TryGetCached(int64_t row, int64_t col, bool& alive)
{
	auto it = m_rows.find(row);
	if (it != m_rows.end())
	{
		ColumnToCell& columnToCell = it->second;
		auto cell_it = columnToCell.find(col);
		if (cell_it != columnToCell.end())
		{
			CellInfo& cellInfo = cell_it->second;
			// move to start of list
			if (cellInfo.m_list_it != m_refernces.begin())
				m_refernces.splice(m_refernces.begin(), m_refernces, cellInfo.m_list_it, std::next(cellInfo.m_list_it));
			// return status
			alive = cellInfo.m_alive;
			return true;
		}
	}
	return false;
}

/// <summary>
/// Cache status
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
/// <returns></returns>
bool CellCache::IsCached(int64_t row, int64_t col)
{
	bool unused;
	return TryGetCached(row, col, unused);
}

bool CellCache::IsProcessed(int64_t row, int64_t col)
{
	auto it = m_rows.find(row);
	if (it != m_rows.end())
	{
		ColumnToCell& columnToCell = it->second;
		auto cell_it = columnToCell.find(col);
		if (cell_it != columnToCell.end())
		{
			CellInfo& cellInfo = cell_it->second;
			return cellInfo.m_processed;
		}
	}
	return false;
}

