#pragma once
#include <sdl2/SDL.h>
#include <string>

namespace engine
{
	/// <summary>
/// Creates and window and initializes its GLContext
/// </summary>
	class Window
	{
	public:

		SDL_Window* sdlWindow;

	private:
		SDL_GLContext glContext;
		int windowWidth;
		int windowHeight;
		unsigned displayIndex;


	public:

		Window(const std::string& title, int width, int height, bool fullscreen = false, unsigned displayIndex = -1);
		Window(const std::string& title, unsigned displayIndex = -1);

		~Window();

	public:

		void SetWindowedFullscreen();

		/** Retorna el ancho actual de la ventana (el usuario puede cambiarlo).
		  */
		unsigned GetWidth() const;

		/** Retorna el alto actual de la ventana (el usuario puede cambiarlo).
		  */
		unsigned GetHeight() const;

		void SetVsync(bool isEnabled);

		/** Borra el buffer de la pantalla usando OpenGL.
		  */
		void Clear() const;

		/** Intercambia el buffer visible con el buffer oculto.
		  */
		void SwapBuffers() const;

	};
}