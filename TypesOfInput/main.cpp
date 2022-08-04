#include "pAulConsoleEngine.h"

class TypesOfInput :
	public pAul::ConsoleEngine
{
public:
	class Controller
	{
	public:
		virtual void Update() noexcept = 0;

		float GetDeltaX() const noexcept
		{
			return m_dx;
		}
		float GetDeltaY() const noexcept
		{
			return m_dy;
		}

	protected:
		static constexpr float sc_dv = 40.0f;

		float m_dx = 0.0f;
		float m_dy = 0.0f;
	};

	class Keyboard :
		public Controller
	{
	public:
		void Update() noexcept override
		{
			if (GetKeyState('W') & 0x8000)
				m_dy = -sc_dv;
			else if (GetKeyState('S') & 0x8000)
				m_dy = sc_dv;
			else
				m_dy = 0.0f;

			if (GetKeyState('A') & 0x8000)
				m_dx = -sc_dv;
			else if (GetKeyState('D') & 0x8000)
				m_dx = sc_dv;
			else
				m_dx = 0.0f;
		}
	};
	
	class Mouse :
		public Controller
	{
	public:
		void Update() noexcept override
		{
			
		}
	};

	class Player
	{
	public:
		Player(float t_x, float t_y, const Controller& t_controller, TypesOfInput& t_app, uint16_t t_c) :
			m_x(t_x), m_y(t_y), m_controller(t_controller), m_app(t_app), m_c(t_c)
		{}

		void Update(float fElapsedTime)
		{
			m_x += m_controller.GetDeltaX() * fElapsedTime;
			m_y += m_controller.GetDeltaY() * fElapsedTime;

			if (m_x < 0.0f || m_x >= m_app.GetWidth() - sc_width)
				if (m_x < 0.0f)
					m_x = 0.0f;
				else
					m_x = m_app.GetWidth() - sc_width;

			if (m_y < 0.0f || m_y >= m_app.GetHeight() - sc_height)
				if (m_y < 0.0f)
					m_y = 0.0f;
				else
					m_y = m_app.GetHeight() - sc_height;
		}
		void Draw()
		{
			uint32_t x = (uint32_t)((int)m_x);
			uint32_t y = (uint32_t)((int)m_y);

			m_app.DrawArray({ x, y }, m_spr, sc_width, sc_height);
		}

	private:
		float m_x;
		float m_y;
		const Controller& m_controller;
		TypesOfInput& m_app;
		uint16_t m_c;

		static constexpr int sc_width = 8;
		static constexpr int sc_height = 8;
		uint16_t m_spr[sc_width * sc_height] = {
			pAul::Color::BG_BLUE, pAul::Color::BG_BLUE, pAul::Color::BG_BLACK, pAul::Color::BG_BLACK, pAul::Color::BG_BLACK, pAul::Color::BG_BLACK, pAul::Color::BG_BLUE, pAul::Color::BG_BLUE,
			pAul::Color::BG_BLUE, pAul::Color::BG_BLACK, pAul::Color::BG_BRIGHT_WHITE, pAul::Color::BG_BRIGHT_WHITE, pAul::Color::BG_BRIGHT_WHITE, pAul::Color::BG_BRIGHT_WHITE, pAul::Color::BG_BLACK, pAul::Color::BG_BLUE,
			pAul::Color::BG_BLUE, pAul::Color::BG_BLACK, pAul::Color::BG_BRIGHT_WHITE, pAul::Color::BG_LIGHT_AQUA, pAul::Color::BG_AQUA, pAul::Color::BG_BRIGHT_WHITE, pAul::Color::BG_BLACK, pAul::Color::BG_BLUE,
			pAul::Color::BG_BLUE, pAul::Color::BG_BLACK, pAul::Color::BG_BRIGHT_WHITE, pAul::Color::BG_AQUA, pAul::Color::BG_AQUA, m_c, pAul::Color::BG_BLACK, pAul::Color::BG_BLUE,
			pAul::Color::BG_BLACK, m_c, m_c, m_c, m_c, m_c, m_c, pAul::Color::BG_BLACK,
			pAul::Color::BG_BLACK, m_c, m_c, m_c, m_c, m_c, m_c, pAul::Color::BG_BLACK,
			pAul::Color::BG_BLUE, pAul::Color::BG_BLACK, m_c, pAul::Color::BG_BLACK, pAul::Color::BG_BLACK, m_c, pAul::Color::BG_BLACK, pAul::Color::BG_BLUE,
			pAul::Color::BG_BLUE, pAul::Color::BG_BLUE, pAul::Color::BG_BLACK, pAul::Color::BG_BLACK, pAul::Color::BG_BLACK, pAul::Color::BG_BLACK, pAul::Color::BG_BLUE, pAul::Color::BG_BLUE
		};
	};

public:
	bool OnUserCreate() override
	{
		Clear(0, pAul::Color::BG_BLUE);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		m_con.Update();

		m_player.Update(fElapsedTime);

		AutoClear();
		m_player.Draw();
		return true;
	}

private:
	Keyboard m_con = {};
	Player m_player = Player(20.0f, 20.0f, m_con, *this, pAul::Color::BG_PURPLE);
};

int main()
{
	TypesOfInput app;
	app.Construct(L"Types of input", 200, 200, 4, 4);
	app.Run();
	return 0;
}