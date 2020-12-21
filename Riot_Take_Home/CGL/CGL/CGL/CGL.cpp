
#include <algorithm>
#include <cstdint>
#include <exception>
#include <iostream>
#include <string> 

#include "BoardUpdater.h"
#include "CellCache.h"

const size_t  NUM_ITERATIONS = 10;

inline bool IsEmtptyOrWhiteSpace(const std::string& str)
{
	if (str.empty() || std::all_of(str.begin(), str.end(), [](char c) { return std::isspace(c); })) 
		return true;
    else
		return false;
}

/// <summary>
/// Read input (row, col) from stdin
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
/// <returns>true if success and false of end of file</returns>
bool GetInput(int64_t& row, int64_t& col)
{
    std::string line;
    if (std::getline(std::cin, line))
    {
        std::size_t index = -1;
        try
        {
            row = std::stoull(line, &index);
        }
        catch (const std::invalid_argument&)
        {
			if (!IsEmtptyOrWhiteSpace(line))
			{
                throw std::invalid_argument("invalid input for row");
			}
            return false; // treat blank line as end of data, for test convenience.
        }
        try
        {
            col = std::stoull(line.substr(index), &index);
        }
        catch (const std::invalid_argument&)
        {
            throw std::invalid_argument("invalid input for column");
        }
		if (IsEmtptyOrWhiteSpace(line.substr(index)))
		{
			throw std::invalid_argument("invalid input after column");
		}
        return true;
    }
    else
    {
        return false;
    }
}

/// <summary>
/// Visitor to display board state
/// </summary>
class BoardOutput : public Board::Visitor
{
    virtual void OnStarted(Board& board)
    {
        std::cout << "#Life 1.06\n";
    }

    virtual bool Visit(Board& board, int64_t row, int64_t col)
    {
        std::cout << row << ' ' << col << '\n';
        return true;
    }
};

/// <summary>
/// main
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int main(int argc, char* argv[])
{
    std::cout << "Conway's Game of life\nImplementation by Asim Naseer\nAwaiting input in Life 1.06 format (https://www.conwaylife.com/wiki/Life_1.06)\n...\n";
    
    {
        std::string line;
        std::getline(std::cin, line);
        if (line != "#Life 1.06")
        {
            std::cerr << "Error:Expecting input in Life 1.06 format, not \"" << line << "\"\n";
            return 1;
        }
    }

    int64_t row = 0, col = 0;
    try
    {
            // Initialize the board

		Board board;
        while (GetInput(row, col))
        {
            board.Initialize(row, col);
        }

		BoardOutput display;
        BoardUpdater updater;
#ifdef _DEBUG
		std::cout << "-Initial State ---------------------- " << '\n';
		board.Accept(&display);
		std::cout << "================================= " << '\n';
#endif

            // Update board a fixed number of times

        for (size_t i = 0; i < NUM_ITERATIONS; ++i)
        {
#ifdef _DEBUG
			std::cout << "================================= " << '\n';
			std::cout << "Iteration: " << i << '\n';
#endif
            board.Accept(&updater);
#ifdef _DEBUG
			std::cout << "-New State ---------------------- " << i << '\n';
			board.Accept(&display);
			std::cout << "================================= " << i << '\n';
#endif
        }

            // Display updated board

        board.Accept(&display);
    }
    catch (std::exception e)
    {
        std::cerr << "Error:" << e.what() << "\nAborting...\n";
        return 1;
    }
    return 0;
}

