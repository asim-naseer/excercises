#pragma once

#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>

/// <summary>
/// Container for active cells
/// </summary>
class BoardState
{
	public:

		struct Cell
		{
			int64_t m_row = 0;
			int64_t m_col = 0;

			Cell(int64_t r, int64_t c) : m_row(r), m_col(c) {}
		};

		class Visitor
		{
		public:
			Visitor() {}
			virtual bool Visit(int64_t row, int64_t col) = 0;
			virtual ~Visitor() {}
		};

		
	private:

		// The 64 bit row, col address (128 bit) is split into 4 32 bit values
		// and stored in the following structure.
		// A address exists in the container if all 4 parts of it can be found by
		// traversing the maps.

		typedef std::set<uint32_t> INT32_0;
		typedef std::map<uint32_t, INT32_0> INT32_1;
		typedef std::map<uint32_t, INT32_1> INT32_2;
		typedef std::map<uint32_t, INT32_2> INT32_3;

		INT32_3 m_r0_map;
		size_t m_size = 0;

		void Set(int64_t row, int64_t col);
		void Clear(int64_t row, int64_t col);

	public:

		
		BoardState()	{}

		size_t Size();

		
		inline void Clear()
		{
			m_r0_map.clear();
			m_size = 0;
		}

		inline void Set(int64_t row, int64_t col, bool aliveStatus)
		{
			if (aliveStatus)
				Set(row, col);
			else
				Clear(row, col);
		}

		bool IsSet(int64_t row, int64_t col) const;
		void Toggle(int64_t row, int64_t col);

		// Accept a visitor to visit all contained cells
		void Accept(Visitor* visitor);

		// Helper functions
		static void UnPack64(int64_t in, uint32_t& w0, uint32_t& w1);
		static int64_t Pack64(uint32_t w0, uint32_t w1);
};


