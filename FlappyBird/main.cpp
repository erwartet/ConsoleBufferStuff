#include "pAulConsoleEngine.h"
#include <vector>
#include <memory>
#include <random>

class FlappyBird :
	public pAul::ConsoleEngine
{
public:
	class PipeManager;

	class Pipe
	{
		friend class PipeManager;
	public:
		Pipe(uint32_t t_x, uint32_t t_gapUpperLeftY, uint32_t t_gapHeight) :
			m_x(t_x), m_gapUpperLeftY(t_gapUpperLeftY), m_gapHeight(t_gapHeight)
		{}

		void Draw(FlappyBird& app)
		{
			for (uint32_t i = 0; i < m_gapUpperLeftY - 1; i++)
				app.DrawArray({ m_x, i }, sc_midSection, (int)std::size(sc_midSection), 1);
			for (uint32_t i = m_gapUpperLeftY + m_gapHeight + 1; i < app.GetHeight(); i++)
				app.DrawArray({ m_x, i }, sc_midSection, (int)std::size(sc_midSection), 1);

			app.DrawArray({ m_x, m_gapUpperLeftY - 1 }, sc_topSection, (int)std::size(sc_topSection), 1);
			app.DrawArray({ m_x, m_gapUpperLeftY + m_gapHeight }, sc_topSection, (int)std::size(sc_topSection), 1);
		}
		void Move(uint32_t xOffset)
		{
			m_x -= xOffset;
		}

	private:
		static constexpr uint16_t sc_midSection[8] = { pAul::Color::BG_AQUA, pAul::Color::BG_BLACK, pAul::Color::BG_GREEN, pAul::Color::BG_WHITE, pAul::Color::BG_GREEN, pAul::Color::BG_GREEN, pAul::Color::BG_BLACK, pAul::Color::BG_AQUA };
		static constexpr uint16_t sc_topSection[8] = { pAul::Color::BG_BLACK, pAul::Color::BG_LIGHT_GREEN, pAul::Color::BG_BRIGHT_WHITE, pAul::Color::BG_LIGHT_GREEN, pAul::Color::BG_LIGHT_GREEN, pAul::Color::BG_LIGHT_GREEN, pAul::Color::BG_LIGHT_GREEN, pAul::Color::BG_BLACK };

		uint32_t m_x;
		uint32_t m_gapUpperLeftY;
		uint32_t m_gapHeight;
	};

	class PipeManager
	{
	public:
		PipeManager(FlappyBird& t_app) :
			m_app(t_app)
		{
			m_pPipes.push_back(std::make_unique<Pipe>(t_app.GetWidth() - std::size(Pipe::sc_midSection), t_app.GetHeight() / 2 - 10, 20));
		}

		void MovePipes()
		{
			if (m_pPipes[0]->m_x <= 1)
			{
				int size = m_pPipes.size();
				m_pPipes.front().release();

				for (int i = 0; i < m_pPipes.size() - 1; i++)
				{
					std::swap(m_pPipes[i], m_pPipes[static_cast<std::vector<std::unique_ptr<FlappyBird::Pipe, std::default_delete<FlappyBird::Pipe>>, std::allocator<std::unique_ptr<FlappyBird::Pipe, std::default_delete<FlappyBird::Pipe>>>>::size_type>(i) + 1]);
				}
				m_pPipes.pop_back();
			}

			if (m_pPipes.back()->m_x == (uint32_t)(m_app.GetWidth() / 2u))
			{
				std::uniform_int_distribution<uint32_t> hDir(6u, 30u);
				uint32_t height = hDir(m_rng);
				std::uniform_int_distribution<uint32_t> nDir(1u, m_app.GetHeight() - height);
				m_pPipes.push_back(std::make_unique<Pipe>(m_app.GetWidth() - std::size(Pipe::sc_midSection), nDir(m_rng), height));
			}

			for (auto& pPipe : m_pPipes)
				pPipe->Move(1u);
		}

		void Draw()
		{
			for (auto& pPipe : m_pPipes)
				pPipe->Draw(m_app);
		}

	private:
		std::vector<std::unique_ptr<Pipe>> m_pPipes;
		FlappyBird& m_app;

		std::random_device m_rd;
		std::mt19937 m_rng;
	};

public:
	bool OnUserCreate() override
	{
		Clear(0, pAul::Color::BG_AQUA);
		m_pPipeManager = std::make_unique<PipeManager>(*this);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (m_fTimer <= 0.0f)
		{
			m_pPipeManager->MovePipes();

			m_fTimer = sc_fMoveTimerTimer;
		} else
			m_fTimer -= fElapsedTime;

		Clear(0, pAul::Color::BG_AQUA);
		m_pPipeManager->Draw();

		return true;
	}

private:
	static constexpr float sc_fMoveTimerTimer = 0.25f;
	float m_fTimer = sc_fMoveTimerTimer;

	std::unique_ptr<PipeManager> m_pPipeManager;
};

int main()
{
	FlappyBird app;
	app.Construct(L"FlappyBird", 80, 60, 8, 8);
	app.Run();
	return 0;
}