#include <stdexcept>

#include "BoardState.h"

/// <summary>
/// Helper function to unpack a 64 bit int to two 32 bits ints
/// </summary>
/// <param name="in"></param>
/// <param name="w0"></param>
/// <param name="w1"></param>
void BoardState::UnPack64(int64_t in, uint32_t& w0, uint32_t& w1)
{
	uint64_t uin = *(reinterpret_cast<uint64_t*>(&in));
	w1 = (uint32_t)(uin & 0xFFFFFFFFLL);
	w0 = (uint32_t)((uin & 0xFFFFFFFF00000000LL) >> 32);
}

/// <summary>
/// Helper function to pack two 32 bits ints in to one 64 bit int
/// </summary>
/// <param name="w0"></param>
/// <param name="w1"></param>
/// <returns></returns>
int64_t BoardState::Pack64(uint32_t w0, uint32_t w1)
{
	uint64_t u = (static_cast<uint64_t>(w0)) << 32 | (static_cast<uint64_t>(w1));
	int64_t i = *(reinterpret_cast<int64_t*>(&u));
	return i;
}

/// <summary>
/// Set a cell as active
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
void BoardState::Set(int64_t row, int64_t col)
{
	uint32_t r0, r1, c0, c1;
	UnPack64(row, r0, r1);
	UnPack64(col, c0, c1);
	INT32_0& set = m_r0_map[r0][r1][c0];
	if (set.find(c1) == set.end())
	{
		++m_size;
		set.insert(c1);
	}
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
size_t BoardState::Size()
{
	return m_size;
}

/// <summary>
/// Clears cell from row, col (makes dead)
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
void BoardState::Clear(int64_t row, int64_t col)
{
	uint32_t r0, r1;
	UnPack64(row, r0, r1);

	auto r0_it = m_r0_map.find(r0);
	if (r0_it == m_r0_map.end())
		return; // no such element

	INT32_2& r1_map = r0_it->second;
	auto r1_it = r1_map.find(r1);
	if (r1_it == r1_map.end())
		return; // no such element

	uint32_t c0, c1;
	UnPack64(col, c0, c1);

	INT32_1& c0_map = r1_it->second;
	auto c0_it = c0_map.find(c0);
	if (c0_it == c0_map.end())
		return; // no such element

	INT32_0& c1_set = c0_it->second;
	auto c1_it = c1_set.find(c1);
	if (c1_it == c1_set.end())
		return; // no such element

	c1_set.erase(c1_it);
	--m_size;

	if (c1_set.empty())
	{
		c0_map.erase(c0_it);
		if (c0_map.empty())
		{
			r1_map.erase(r1_it);
			if (r1_map.empty())
			{
				m_r0_map.erase(r0_it);
			}
		}
	}
}

/// <summary>
/// Checks active statos of cell at row, col
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
/// <returns></returns>
bool BoardState::IsSet(int64_t row, int64_t col) const
{
	uint32_t r0, r1;
	UnPack64(row, r0, r1);

	auto r0_it = m_r0_map.find(r0);
	if (r0_it == m_r0_map.end())
		return false; // no such element

	const INT32_2& r1_map = r0_it->second;
	auto r1_it = r1_map.find(r1);
	if (r1_it == r1_map.end())
		return false; // no such element

	uint32_t c0, c1;
	UnPack64(col, c0, c1);

	const INT32_1& c0_map = r1_it->second;
	auto c0_it = c0_map.find(c0);
	if (c0_it == c0_map.end())
		return false; // no such element

	const INT32_0& c1_set = c0_it->second;
	auto c1_it = c1_set.find(c1);
	if (c1_it == c1_set.end())
		return false; // no such element

	return true;
}

/// <summary>
/// Toggles the current state of the cell at row, col
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
void BoardState::Toggle(int64_t row, int64_t col)
{
	uint32_t r0, r1;
	UnPack64(row, r0, r1);
	uint32_t c0, c1;
	UnPack64(col, c0, c1);

	auto r0_it = m_r0_map.find(r0);
	if (r0_it == m_r0_map.end())
	{
		m_r0_map[r0][r1][c0].insert(c1);
		return;
	}

	INT32_2& r1_map = r0_it->second;
	auto r1_it = r1_map.find(r1);
	if (r1_it == r1_map.end())
	{
		r1_map[r1][c0].insert(c1);
		return;
	}

	INT32_1& c0_map = r1_it->second;
	auto c0_it = c0_map.find(c0);
	if (c0_it == c0_map.end())
	{
		c0_map[c0].insert(c1);
		return;
	}

	INT32_0& c1_set = c0_it->second;
	auto c1_it = c1_set.find(c1);
	if (c1_it == c1_set.end())
	{
		c1_set.insert(c1);
		return;
	}

	c1_set.erase(c1_it);
	--m_size;

	if (c1_set.empty())
	{
		c0_map.erase(c0_it);
		if (c0_map.empty())
		{
			r1_map.erase(r1_it);
			if (r1_map.empty())
			{
				m_r0_map.erase(r0_it);
			}
		}
	}
}

/// <summary>
/// Accept a visitor for all active cells
/// </summary>
/// <param name="visitor"></param>
void BoardState::Accept(Visitor* visitor)
{
	if (visitor == nullptr)
		throw std::invalid_argument("visitor cannot be null");

	for (INT32_3::iterator r0_it = m_r0_map.begin(); r0_it != m_r0_map.end(); ++r0_it)
	{
		INT32_2 r1_map = r0_it->second;
		for (INT32_2::iterator r1_it = r1_map.begin(); r1_it != r1_map.end(); ++r1_it)
		{
			INT32_1 c0_map = r1_it->second;
			for (INT32_1::iterator c0_it = c0_map.begin(); c0_it != c0_map.end(); ++c0_it)
			{
				INT32_0 c1_set = c0_it->second;
				for (INT32_0::iterator c1_it = c1_set.begin(); c1_it != c1_set.end(); ++c1_it)
				{
					int64_t row = Pack64(r0_it->first, r1_it->first);
					int64_t col = Pack64(c0_it->first, *c1_it);
					if (!visitor->Visit(row, col))
						return;
				}
			}
		}
	}
}


