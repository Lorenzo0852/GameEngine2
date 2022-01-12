/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

//Idea nombre engine:
// ARGE? -> Awesome Raw Game Engine
// NIA Engine -> No Idea at All Engine
// HSLM Engine -> Hecho Sobre La Marcha Engine

#include <iostream>
#include "Game/Game.h"
#include "Window/Window.h"

// SDL Requires a number of arguments and an array of the actual parameters in the main function.
int main(int args, char* argv[])
{
	Window window("Unnamed game engine", 100, 100, false, -1);
	// We create the game in the stack. We don't need the 'new' keyword for stack-only variables.
	Game game;
	game.Initialize(window);
	game.SetupScene();
	game.Run();
	
	return 0;
}
