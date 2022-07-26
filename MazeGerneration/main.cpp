#include "pAulConsoleEngine.h"

class MazeGerneration :
	public pAul::ConsoleEngine
{
public:
	bool OnUserCreate() override
	{
		m_boardWidth = GetWidth() / (mc_cellSize + mc_gutterWidth);
		m_boardHeight = GetHeight() / (mc_cellSize + mc_gutterWidth);

		for (uint32_t y = 0; y < m_boardHeight; y++)
			for (uint32_t x = 0; x < m_boardWidth; x++)
				FillRect({ x * mc_cellSize + x * mc_gutterWidth + mc_gutterWidth, y * mc_cellSize + y * mc_gutterWidth + mc_gutterWidth }, mc_cellSize - 1, mc_cellSize - 1, 0, pAul::Color::BG_BLUE);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		return true;
	}

private:
	const int mc_cellSize = 4;
	const int mc_gutterWidth = 1;
	int m_boardWidth = 0;
	int m_boardHeight = 0;
};

int main()
{
	MazeGerneration app;
	app.Construct(L"Maze", 101, 101, 8, 8);
	app.Run();
	return 0;
}