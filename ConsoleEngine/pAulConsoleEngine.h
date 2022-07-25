#pragma once

#include <Windows.h>
#include <string>
#include <cinttypes>
#include <algorithm>
#include <sstream>
#include <time.h>
#include <chrono>

namespace pAul {

	struct Pos
	{
		uint32_t x;
		uint32_t y;
	};

	namespace Color {
		enum
		{
			FG_BLACK = 0x0000,
			FG_BLUE = 0x0001,
			FG_GREEN = 0x0002,
			FG_AQUA = 0x0003,
			FG_RED = 0x0004,
			FG_PURPLE = 0x0005,
			FG_YELLOW = 0x0006,
			FG_WHITE = 0x0007,
			FG_GRAY = 0x0008,
			FG_LIGHT_BLUE = 0x0009,
			FG_LIGHT_GREEN = 0x000A,
			FG_LIGHT_AQUA = 0x000B,
			FG_LIGHT_RED = 0x000C,
			FG_LIGHT_PURPLE = 0x000D,
			FG_LIGHT_YELLOW = 0x000E,
			FG_BRIGHT_WHITE = 0x000F,

			BG_BLACK = 0x0000,
			BG_BLUE = 0x00010,
			BG_GREEN = 0x0020,
			BG_AQUA = 0x0030,
			BG_RED = 0x0040,
			BG_PURPLE = 0x0050,
			BG_YELLOW = 0x0060,
			BG_WHITE = 0x0070,
			BG_GRAY = 0x0080,
			BG_LIGHT_BLUE = 0x0090,
			BG_LIGHT_GREEN = 0x00A0,
			BG_LIGHT_AQUA = 0x00B0,
			BG_LIGHT_RED = 0x00C0,
			BG_LIGHT_PURPLE = 0x00D0,
			BG_LIGHT_YELLOW = 0x00E0,
			BG_BRIGHT_WHITE = 0x00F0
		};
	}
	enum PixelType
	{
		SOLID = 0x2588,
		THREE_QUARTERS = 0x2593,
		HALF = 0x2592,
		QUARTER = 0x2591
	};

	class ConsoleEngine
	{
	public:
		ConsoleEngine() = default;
		virtual ~ConsoleEngine()
		{
			OnUserDestroy();
			CloseHandle(m_hConsole);
			delete[] m_pScreenBuff;
		}
		ConsoleEngine(const ConsoleEngine&) = delete;
		ConsoleEngine operator=(const ConsoleEngine&) = delete;

		void Construct(std::wstring sAppName, uint32_t width, uint32_t height, int16_t pixelWidth, int16_t pixelHeight)
		{
			m_sAppName = sAppName;
			m_width = width;
			m_height = height;
			m_pixelWidth = pixelWidth;
			m_pixelHeight = pixelHeight;

			m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

			SetConsoleWindowInfo(m_hConsole, true, &m_srWindow);
			SetConsoleScreenBufferSize(m_hConsole, { (short)m_width, (short)m_height });
			SetConsoleActiveScreenBuffer(m_hConsole);

			CONSOLE_FONT_INFOEX cfi = {};
			cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
			GetCurrentConsoleFontEx(m_hConsole, false, &cfi);
			cfi.FontFamily = FF_DONTCARE;
			cfi.dwFontSize = { m_pixelWidth, m_pixelHeight };
			SetCurrentConsoleFontEx(m_hConsole, false, &cfi);

			m_srWindow = { 0, 0, (short)(m_width - 1), (short)(m_height - 1) };
			SetConsoleWindowInfo(m_hConsole, true, &m_srWindow);

			m_pScreenBuff = new CHAR_INFO[m_width * m_height];
			memset(m_pScreenBuff, 0, sizeof(CHAR_INFO) * m_width * m_height);

			WriteConsoleOutput(m_hConsole, m_pScreenBuff, { (short)m_width, (short)m_height }, { 0, 0 }, &m_srWindow);

			OnUserCreate();
		}

		void Run()
		{
			auto tp1 = std::chrono::system_clock::now();
			auto tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsedTime{};
			float fElapsedTime = 0.0f;
			while (m_bRunning)
			{
				tp1 = std::chrono::system_clock::now();

				if (!OnUserUpdate(fElapsedTime))
					m_bRunning = false;

				WriteConsoleOutput(m_hConsole, m_pScreenBuff, { (short)m_width, (short)m_height }, { 0, 0 }, &m_srWindow);

				std::wstringstream wss;
				wss << m_sAppName << L" - FPS: " << 1.0f / fElapsedTime;
				SetConsoleTitle(wss.str().c_str());

				tp2 = std::chrono::system_clock::now();

				elapsedTime = tp2 - tp1;
				fElapsedTime = elapsedTime.count();
			}
		}

		virtual bool OnUserCreate() = 0;
		virtual bool OnUserUpdate(float fElapsedTime) = 0;
		virtual void OnUserDestroy() {};

		const uint32_t GetWidth() const noexcept
		{
			return m_width;
		}

		const uint32_t GetHeight() const noexcept
		{
			return m_height;
		}

		void Clear() noexcept
		{
			FillRect({ 0, 0 }, { m_width - 1, m_height - 1 }, m_clearChar, m_clearInfo);
		}

		void Clear(wchar_t c = 0, const uint16_t info = Color::BG_BLACK) noexcept
		{
			m_clearChar = c;
			m_clearInfo = info;
			FillRect({ 0, 0 }, { m_width - 1, m_height - 1 }, m_clearChar, m_clearInfo);
		}

		void PutPixel(Pos pos, const wchar_t c = 0, const uint16_t info = Color::BG_BRIGHT_WHITE) noexcept
		{
			GetChar(pos).Char.UnicodeChar = c;
			GetChar(pos).Attributes = info;
		}

		void FillRect(Pos pos1, Pos pos2, const wchar_t c = 0, const uint16_t info = Color::BG_BRIGHT_WHITE) noexcept
		{
			if (!IsInSide(pos1) && IsInSide(pos2))
				return;

			if (pos1.x > pos2.x)
				std::swap(pos1.x, pos2.x);
			if (pos1.y > pos2.y)
				std::swap(pos1.y, pos2.y);

			for (uint32_t y = pos1.y; y <= pos2.y; y++)
				for (uint32_t x = pos1.x; x <= pos2.x; x++)
					PutPixel({ x, y }, c, info);
		}

		void DrawArray(Pos upperLeft, const uint16_t* arr, int width, int height)
		{
			for (uint32_t y = upperLeft.y, i = 0; y <= upperLeft.y + height - 1; y++)
			{
				for (uint32_t x = upperLeft.x; x <= upperLeft.x + width - 1; x++, i++)
				{
					PutPixel({ x, y }, 0, arr[i]);
				}
			}
		}
		void DrawArray(Pos upperLeft, const wchar_t* arr, int width, int height)
		{
			for (uint32_t y = upperLeft.y, i = 0; y <= upperLeft.y + height - 1; y++)
			{
				for (uint32_t x = upperLeft.x; x <= upperLeft.x + width - 1; x++, i++)
				{
					PutPixel({ x, y }, arr[i]);
				}
			}
		}
		void DrawArray(Pos upperLeft, const wchar_t* arrChar, const uint16_t* arrInfo, int width, int height)
		{
			for (uint32_t y = upperLeft.y, i = 0; y <= upperLeft.y + height - 1; y++)
			{
				for (uint32_t x = upperLeft.x ; x <= upperLeft.x + width - 1; x++, i++)
				{
					PutPixel({ x, y }, arrChar[i], arrInfo[i]);
				}
			}
		}

	private:
		CHAR_INFO& GetChar(Pos pos)
		{
			if (IsInSide(pos))
				return m_pScreenBuff[pos.x + pos.y * m_width];
			else
				return m_pScreenBuff[0];
		}

		bool IsInSide(Pos pos)
		{
			return pos.x >= 0 && (uint32_t)pos.x < m_width && pos.y >= 0 && (uint32_t)pos.y < m_height;
		}

	private:
		std::wstring m_sAppName = L"";
		uint32_t m_width = 0u;
		uint32_t m_height = 0u;
		int16_t m_pixelWidth = 0u;
		int16_t m_pixelHeight = 0u;

		bool m_bRunning = true;
		wchar_t m_clearChar = 0;
		uint16_t m_clearInfo = Color::BG_BLACK;

		CHAR_INFO* m_pScreenBuff = nullptr;
		HANDLE m_hConsole = 0;
		SMALL_RECT m_srWindow = { 0, 0, 1, 1 };
	};

}