#include "Window.h"
#include <sdl2/SDL.h>
#include <spdlog/spdlog.h>
#include <OpenGL.hpp>

Window::Window(const std::string& title, int width, int height, bool fullscreen, unsigned displayIndex)
{
	windowWidth = width;
	windowHeight = height;

	sdlWindow = nullptr;
	glContext = nullptr;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) //This will return != 0 if it can't initialize
	{
		spdlog::error("Error initializing SDL");
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	sdlWindow = SDL_CreateWindow
	(
		title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS
	);

	assert(sdlWindow != nullptr);

	if (sdlWindow)
	{
		glContext = SDL_GL_CreateContext(sdlWindow);

		if (!glContext)
		{
			spdlog::error("GL Context couldn't be initialized in SDL Window.");
			return;
		}

		assert(glContext != nullptr);

		if (glContext && glt::initialize_opengl_extensions())
		{
			if (fullscreen)
			{
				SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
			}
		}
	}
	spdlog::info("SDL Window created");
}

Window::~Window()
{
	if (glContext) SDL_GL_DeleteContext(glContext);
	if (sdlWindow) SDL_DestroyWindow(sdlWindow);
	spdlog::info("SDL Window destroyed");
}

void Window::SetWindowedFullscreen(){
	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(displayIndex, &displayMode);
	windowWidth = displayMode.w;
	windowHeight = displayMode.h;
	SDL_SetWindowSize(sdlWindow, windowWidth, windowHeight);
	SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void Window::SetVsync(bool isEnabled)
{
	if (glContext)
	{
		if (isEnabled)	SDL_GL_SetSwapInterval(1);
		else			SDL_GL_SetSwapInterval(0);
	}
}

unsigned Window::GetWidth() const
{
	int width = 0, height;

	if (sdlWindow) SDL_GetWindowSize(sdlWindow, &width, &height);

	return unsigned(width);
}

unsigned Window::GetHeight() const
{
	int width, height = 0;

	if (sdlWindow) SDL_GetWindowSize(sdlWindow, &width, &height);

	return unsigned(height);
}

void Window::Clear() const
{
	if (glContext) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::SwapBuffers() const
{
	if (glContext) SDL_GL_SwapWindow(sdlWindow);
}