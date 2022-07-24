#include "pAulConsoleEngine.h"
#include <vector>
#include <iostream>
#include <thread>
#include <random>

constexpr int gc_boardWidth = 64;
constexpr int gc_boardHeight = 36;

class SnakeGame :
	public pAul::ConsoleEngine
{
public:
	struct Cell
	{
		uint32_t x;
		uint32_t y;
		bool operator==(const Cell& rhs) const
		{
			return x == rhs.x && y == rhs.y;
		}
	};

	class Snake
	{
	private:
		class Tale
		{
		public:
			Tale(Cell t_cell) :
				m_cell(t_cell)
			{}
			void Follow(Tale front)
			{
				m_cell = front.m_cell;
			}
			void Draw(uint16_t board[gc_boardWidth][gc_boardHeight], uint16_t c, const Cell& dir)
			{
				board[m_cell.x][m_cell.y] = c;
			}
			Cell& pos()
			{
				return m_cell;
			}

		protected:
			Cell m_cell;
		};
		class Head :
			public Tale
		{
		public:
			Head() :
				Tale({ (uint32_t)(gc_boardWidth / 2), (uint32_t)(2 * gc_boardHeight / 3) })
			{}
			void ChangeVelocity(int x, int y)
			{
				if (x == m_x * -1 || y == m_y * -1)
					return;

				m_x = x;
				m_y = y;
			}
			void Move()
			{
				m_cell.x += m_x;
				m_cell.y += m_y;
			}
			bool CanMove(std::vector<Tale>& tale)
			{
				int nextX = m_cell.x + m_x; 
				int nextY = m_cell.y + m_y;

				if (nextX < 0 && nextX >= gc_boardWidth &&
					 nextY < 0 && nextY >= gc_boardHeight)
					return false;
				
				Cell next = { (uint32_t)nextX, (uint32_t)nextY };
				for (auto& segment : tale)
				{
					if (segment.pos() == next)
						return false;
				}

				return true;
			}
			void Draw(uint16_t board[gc_boardWidth][gc_boardHeight], uint16_t c)
			{
				board[m_cell.x][m_cell.y] = c;
			}
			
		private:
			int m_x = 0;
			int m_y = -1;
		};

	public:
		Snake()
		{
			m_tale.push_back(Tale({ m_head.pos().x, m_head.pos().y + 1 }));
			m_tale.push_back(Tale({ m_head.pos().x, m_head.pos().y + 2 }));
		}
		bool Move()
		{
			if (!m_head.CanMove(m_tale))
				return false;

			for (int i = static_cast<int>(m_tale.size()) - 1; i >= 1; i--)
				m_tale[i].Follow(m_tale[static_cast<std::vector<SnakeGame::Snake::Tale, std::allocator<SnakeGame::Snake::Tale>>::size_type>(i) - 1]);
			m_tale[0].Follow(m_head);
			m_head.Move();
			return true;
		}
		void Draw(uint16_t board[gc_boardWidth][gc_boardHeight])
		{
			for (int i = 1; i < m_tale.size(); i++)
			{
				m_tale[i].Draw(board, pAul::Color::BG_LIGHT_GREEN, m_tale[static_cast<std::vector<SnakeGame::Snake::Tale, std::allocator<SnakeGame::Snake::Tale>>::size_type>(i) - 1].pos());
			}
			m_tale[0].Draw(board, pAul::Color::BG_LIGHT_GREEN, m_head.pos());
			m_head.Draw(board, pAul::Color::BG_YELLOW);
		}
		bool IsTale(const Cell& cell)
		{
			if (m_head.pos() == cell)
				return true;
			for (auto& segment : m_tale)
			{
				if (segment.pos() == cell)
					return true;
			}
			return false;
		}
		bool Eat(const Cell& food, float& fWaitTime)
		{
			if (food == m_head.pos())
			{
				Grow();
				return true;
			}
			return false;
		}
		void Grow()
		{
			m_tale.push_back(Tale(m_tale.back().pos()));
		}
		void ChangeVelocity(int x, int y)
		{
			m_head.ChangeVelocity(x, y);
		}

	private:
		std::vector<Tale> m_tale;
		Head m_head;
	};

public:
	bool SpawnFood()
	{
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<uint32_t> xDist(0, gc_boardWidth - 1);
		std::uniform_int_distribution<uint32_t> yDist(0, gc_boardHeight - 1);
		m_food = { xDist(rng), yDist(rng) };
		while (m_snake.IsTale(m_food))
		{
			xDist.reset();
			yDist.reset();
			m_food = { xDist(rng), yDist(rng) };
		}
		return true;
	}

	bool OnUserCreate() override
	{
		for (int y = 0; y < gc_boardHeight; y++)
			for (int x = 0; x < gc_boardWidth; x++)
				m_board[x][y] = pAul::Color::BG_BLACK;

		SpawnFood();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetKeyState('W') & 0x8000)
		{
			m_x = 0;
			m_y = -1;
		} else if (GetKeyState('S') & 0x8000)
		{
			m_x = 0;
			m_y = 1;
		} else if (GetKeyState('A') & 0x8000)
		{
			m_x = -1;
			m_y = 0;
		} else if (GetKeyState('D') & 0x8000)
		{
			m_x = 1;
			m_y = 0;
		}
		m_snake.ChangeVelocity(m_x, m_y);

		if (m_fTimer <= 0.0f)
		{
			if (!m_snake.Move())
				return false;

			if (m_snake.Eat(m_food, m_fWaitTime))
				SpawnFood();

			m_board[m_food.x][m_food.y] = pAul::Color::BG_LIGHT_RED;
			m_snake.Draw(m_board);

			for (uint32_t y = 0; y < gc_boardHeight; y++)
				for (uint32_t x = 0; x < gc_boardWidth; x++)
				{
					FillRect({ x * sc_cellSize, y * sc_cellSize }, { x * sc_cellSize + sc_cellSize, y * sc_cellSize + sc_cellSize }, 0, m_board[x][y]);
					m_board[x][y] = pAul::Color::BG_BLACK;
				}

			m_fTimer = m_fWaitTime;
		} else
			m_fTimer -= fElapsedTime;
		return true;
	}

private:
	static constexpr int sc_cellSize = 5;
	static constexpr int sc_outline = 1;
	float m_fWaitTime = 0.2f;
	float m_fTimer = m_fWaitTime;

	uint16_t m_board[gc_boardWidth][gc_boardHeight] = {};
	Snake m_snake;
	Cell m_food = { 0, 0 };

	int m_x = 0;
	int m_y = -1;
};

int main()
{
	SnakeGame app;
	app.Construct(L"SnakeGame", 320u, 180u, 4, 4);
	app.Run();
	return 0;
}