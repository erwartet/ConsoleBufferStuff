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
		DrawRect({ 10u, 10u }, { 50u, 50u });
		return true;
	}

private:

};

int main()
{
	Sample sample;
	sample.Construct(L"Sample", 160u, 160u, 4, 4);
	sample.Run();
	return 0;
}