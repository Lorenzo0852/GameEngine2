/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

//Idea nombre engine:
// ARGE? -> Awesome Raw Game Engine
// NIA Engine -> No Idea at All Engine
// HSLM Engine -> Hecho Sobre La Marcha Engine

#include <iostream>
#include "Game/Game.h"
#include <sdl2/SDL.h>

// SDL Requires a number of arguments and an array of the actual parameters in the main function.
int main(int args, char* argv[])
{
	// We create the game in the stack. We don't need the 'new' keyword for stack-only variables.
	Game game;
	game.Initialize(true, 0);
	game.SetupScene();
	game.Run();
	
	return 0;
}
