#include "pAulConsoleEngine.h"
#include <random>
#include <cassert>

class GameOfLife :
	public pAul::ConsoleEngine
{
public:
	void ApplyRules(int x, int y)
	{
		// Get number of neighbours
		int nNeighbour = 0;
		if (y - 1 >= 0 && static_cast<unsigned long long>(y) - 1 < m_currentGen[x].size())
			if (m_currentGen[x][static_cast<std::vector<bool, std::allocator<bool>>::size_type>(y) - 1] == true)
				nNeighbour++;

		if (x - 1 >= 0 && static_cast<unsigned long long>(x) - 1 < m_currentGen.size())
			if (m_currentGen[static_cast<std::vector<std::vector<bool, std::allocator<bool>>, std::allocator<std::vector<bool, std::allocator<bool>>>>::size_type>(x) - 1][y] == true)
				nNeighbour++;

		if (y + 1 >= 0 && static_cast<unsigned long long>(y) + 1 < m_currentGen[x].size())
			if (m_currentGen[x][static_cast<std::vector<bool, std::allocator<bool>>::size_type>(y) + 1] == true)
				nNeighbour++;

		if (x + 1 >= 0 && static_cast<unsigned long long>(x) + 1 < m_currentGen.size())
			if (m_currentGen[static_cast<std::vector<std::vector<bool, std::allocator<bool>>, std::allocator<std::vector<bool, std::allocator<bool>>>>::size_type>(x) + 1][y] == true)
				nNeighbour++;

		// Rule 1: Any live cell with fewer than two live neighbours dies, as if by underpopulation.
		if (m_currentGen[x][y] == true && (nNeighbour < 2))
		{
			m_nextGen[x][y] = false;
			return;
		}

		// Rule 2: Any live cell with two or three live neighbours lives on to the next generation.
		if (m_currentGen[x][y] == true && (nNeighbour == 2 || nNeighbour == 3))
		{
			m_nextGen[x][y] = true;
			return;
		}

		// Rule 3: Any live cell with more than three live neighbours dies, as if by overpopulation.
		if (m_currentGen[x][y] == true && (nNeighbour > 3))
		{
			m_nextGen[x][y] = false;
			return;
		}

		// Rule 4: Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
		if (m_currentGen[x][y] == false && (nNeighbour == 3))
		{
			m_nextGen[x][y] = true;
			return;
		}
	}

public:
	void Setup()
	{
		m_currentGen.resize(static_cast<std::vector<std::vector<bool, std::allocator<bool>>, std::allocator<std::vector<bool, std::allocator<bool>>>>::size_type>(GetWidth()) - 100);
		for (auto& vx : m_currentGen)
			vx.resize(GetHeight());

		m_nextGen.resize(static_cast<std::vector<std::vector<bool, std::allocator<bool>>, std::allocator<std::vector<bool, std::allocator<bool>>>>::size_type>(GetWidth()) - 100);
		for (auto& vx : m_nextGen)
			vx.resize(GetHeight());
	}

	void Reset()
	{
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> bDist(0, 2);

		for (int x = 0; x < m_currentGen.size(); x++)
		{
			for (int y = 0; y < m_currentGen[x].size(); y++)
			{
				m_currentGen[x][y] = bDist(rng) > 0 ? false : true;
				bDist.reset();
			}
		}
	}

	bool OnUserCreate()
	{
		Setup();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		if (GetKeyState(VK_SPACE) & 0x8000)
		{
			Reset();
		}

		if (m_fTimer <= 0.0f)
		{
			m_fTimer = sc_fTime;

			Clear();
			FillRect({ GetWidth() - sc_nPixelOversize, 0 },{ GetWidth(), GetHeight()}, 0, pAul::Color::BG_BLUE);

			for (int x = 0; x < m_currentGen.size(); x++)
			{
				for (int y = 0; y < m_currentGen[x].size(); y++)
				{
					ApplyRules(x, y);
				}
			}

			m_currentGen = m_nextGen;
			for (int x = 0; x < m_nextGen.size(); x++)
			{
				for (int y = 0; y < m_nextGen[x].size(); y++)
				{
					m_nextGen[x][y] = false;
				}
			}

			for (uint32_t x = 0; x < m_currentGen.size(); x++)
			{
				for (uint32_t y = 0; y < m_currentGen[x].size(); y++)
				{
					if (m_currentGen[x][y])
						PutPixel({ x, y });
				}
			}


			m = true;
		} else
			m_fTimer -= fElapsedTime;
		return true;
	}

private:
	static constexpr float sc_fTime = 0.5f;
	static constexpr int sc_nPixelOversize = 100;

	std::vector<std::vector<bool>> m_currentGen;
	std::vector<std::vector<bool>> m_nextGen;

	float m_fTimer = sc_fTime;

	bool m = false;
};

int main()
{
	GameOfLife app;
	app.Construct(L"John Conway's Game Of Life", 300, 200, 4, 4);
	app.Run();
	return 0;
}