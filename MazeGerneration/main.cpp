#include "pAulConsoleEngine.h"
#include <vector>
#include <stack>
#include <optional>
#include <random>

class MazeGerneration :
	public pAul::ConsoleEngine
{
public:
	struct Cell
	{
		int x;
		int y;
		Cell* next;
	};

	bool AlreadyInStack(int x, int y)
	{
		const int in = y * m_boardWidth + x;
		for (auto& i : m_alreadyTaken)
			if (in == i)
				return true;
		return false;
	}

	bool InsideBoard(int x, int y)
	{
		return x >= 0 && x < m_boardWidth&& y >= 0 && y < m_boardHeight;
	}

	void Draw(const Cell& cell, int dirX, int dirY)
	{
		if (dirX < 0 || dirY < 0)
			FillRect({ TranslateCoord(cell.x, true) + dirX, TranslateCoord(cell.y, false) + dirY }, mc_cellSize - dirX - 1, mc_cellSize - dirY - 1, 0, pAul::Color::BG_BRIGHT_WHITE);
		else
			FillRect({ TranslateCoord(cell.x, true), TranslateCoord(cell.y, false) }, mc_cellSize + dirX - 1, mc_cellSize + dirY - 1, 0, pAul::Color::BG_BRIGHT_WHITE);
	}

	void RandomDirection()
	{
		Cell& cell = *m_stack.top();

		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> dirDis(0, 3);
		int nDir = dirDis(rng);
		Cell* dir = nullptr;
		int dirX = 0;
		int dirY = 0;

		for (int i = 0; i < 4; i++)
		{
			switch (nDir)
			{
			case 0:
				if (InsideBoard(cell.x, cell.y - 1))
				{
					if (!AlreadyInStack(cell.x, cell.y - 1))
					{
						dir = &m_board[(cell.y - 1) * m_boardWidth + cell.x];
						dirY = -1;
					}
				}
				break;
			case 1:
				if (InsideBoard(cell.x + 1, cell.y))
				{
					if (!AlreadyInStack(cell.x + 1, cell.y))
					{
						dir = &m_board[cell.y * m_boardWidth + cell.x + 1];
						dirX = 1;
					}
				}
				break;
			case 2:
				if (InsideBoard(cell.x, cell.y + 1))
				{
					if (!AlreadyInStack(cell.x, cell.y + 1))
					{
						dir = &m_board[(cell.y + 1) * m_boardWidth + cell.x];
						dirY = 1;
					}
				}
				break;
			case 3:
				if (InsideBoard(cell.x - 1, cell.y))
				{
					if (!AlreadyInStack(cell.x - 1, cell.y))
					{
						dir = &m_board[cell.y * m_boardWidth + cell.x - 1];
						dirX = -1;
					}
				}
				break;
			}
			if (dir != nullptr)
				break;
			if (nDir < 3)
				nDir++;
			else
				nDir = 0;
		}
		
		if (dir == nullptr)
			m_stack.pop();
		else
		{
			cell.next = dir;
			Draw(*cell.next, dirX * -1, dirY * -1);
			m_alreadyTaken.push_back(cell.next->y * m_boardWidth + cell.next->x);
			m_stack.push(cell.next);
		}
	}

	uint32_t TranslateCoord(int a, bool bX)
	{
		if (bX)
			return (uint32_t)(a * mc_cellSize + a * mc_gutterWidth + mc_gutterWidth);
		else
			return (uint32_t)(a * mc_cellSize + a * mc_gutterWidth + mc_gutterWidth);
	}

public:
	void Build()
	{
		Clear();
		for (uint32_t y = 0; y < m_boardHeight; y++)
			for (uint32_t x = 0; x < m_boardWidth; x++)
				FillRect({ TranslateCoord(x, true), TranslateCoord(y, false) }, mc_cellSize - 1, mc_cellSize - 1, 0, pAul::Color::BG_BLUE);

		m_stack.push(&m_board[0]);

		FillRect({ 1, 1 }, mc_cellSize - 1, mc_cellSize - 1, 0, pAul::Color::BG_BRIGHT_WHITE);
		for (uint32_t i = 1; i <= 4; i++)
			PutPixel({ i, 0 }, 0, pAul::Color::BG_BRIGHT_WHITE);
		for (uint32_t i = GetWidth() - 2; i >= GetWidth() - 5; i--)
			PutPixel({ i, GetHeight() - 1u }, 0, pAul::Color::BG_BRIGHT_WHITE);
	}

	bool OnUserCreate() override
	{
		m_boardWidth = GetWidth() / (mc_cellSize + mc_gutterWidth);
		m_boardHeight = GetHeight() / (mc_cellSize + mc_gutterWidth);

		for (int y = 0; y < m_boardHeight; y++)
			for (int x = 0; x < m_boardWidth; x++)
				m_board.push_back({ x, y, nullptr });

		Build();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (m_stack.size() > 0)
		{
			if (m_timer <= 0)
			{
				RandomDirection();
			} else
				m_timer = sc_waitTime;
			m_timer -= fElapsedTime;
		}

		if (GetKeyState(VK_BACK) & 0x8000)
		{
			m_alreadyTaken.clear();
			Build();
		}
		return true;
	}

private:
	static constexpr float sc_waitTime = 0.01f;
	float m_timer = sc_waitTime;

	const int mc_cellSize = 4;
	const int mc_gutterWidth = 1;
	int m_boardWidth = 0;
	int m_boardHeight = 0;

	std::vector<Cell> m_board;
	std::stack<Cell*> m_stack;
	std::vector<int> m_alreadyTaken;
};

int main()
{
	MazeGerneration app;
	app.Construct(L"Maze", 101, 101, 8, 8);
	app.Run();
	return 0;
}