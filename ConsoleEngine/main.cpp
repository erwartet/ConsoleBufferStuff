#include "pAulConsoleEngine.h"

class Sample :
	public pAul::ConsoleEngine
{
public:
	bool OnUserCreate() override
	{
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		for (uint32_t y = 0u; y < GetHeight(); y++)
			for (uint32_t x = 0u; x < GetWidth(); x++)
				PutPixel({ x, y }, pAul::HALF, rand() % 0xF0);
		return true;
	}

private:

};

int main()
{
	Sample sample;
	sample.Construct(L"Sample", 80u, 80u, 8, 8);
	sample.Run();
	return 0;
}