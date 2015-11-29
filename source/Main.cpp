/**
 * @file
 * Contains the program entry point.
 */

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define WINDOW_RESOLUTION_X (256 * 3)
#define WINDOW_RESOLUTION_Y (240 * 3)

static SDL_Window* window = NULL;

/**
 * Cleanup all resources used by libraries for program exit.
 */
static void cleanup()
{
	SDL_Quit();
}

/**
 * Initialize all libraries for use.
 */
static int initialize()
{
	// Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) != 0 )
	{
		std::cout << "Error: Failed to initialize SDL\nDetails:\n" << SDL_GetError() << std::endl;
		return -1;
	}

	// Create the window
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	window = SDL_CreateWindow(
		"NES",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_RESOLUTION_X,
		WINDOW_RESOLUTION_Y,
		SDL_WINDOW_OPENGL
	);
	if( window == NULL )
	{
		std::cout << "Error: Failed to create the SDL_Window\nDetails:\n" << SDL_GetError() << std::endl;
		return -1;
	}

	// Create the OpenGL context
	if( SDL_GL_CreateContext(window) == NULL )
	{
		std::cout << "Error: failed to create the OpenGL context\nDetails:\n" << SDL_GetError() << std::endl;
		return -1;
	}

	// Enable vsync
	SDL_GL_SetSwapInterval(1);

	return 0;
}

/**
 * Program entry point.
 */
int main( int argc, char** argv )
{
	// Wrap everything in a try-catch
	try
	{
		if( initialize() != 0 )
		{
			std::cout << "initialize() failed\n";
			cleanup();
			return -1;
		}
		else
		{
			// TODO: run the emulator
		}
	}
	catch( std::exception& e )
	{
		std::cout << "Unhandled exception caught in main():\n" << e.what() << std::endl;
	}
	catch( ... )
	{
		std::cout << "Unknown exception caught in main()\n";
	}

	cleanup();

	return 0;
}
