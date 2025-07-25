#include <iostream>

#include "Game.h"


int main(int argc,char* args[])
{
	
	auto pGame = std::make_unique<Game>(Window{"Sandbox - Marek Lens", 1000.f , 800.f});
	pGame->Run();


	return 0;
}

