/**
 * @file
 * Contains the program entry point.
 */

#include <cstdio>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "NES.hpp"

#define WINDOW_RESOLUTION_X (256 * 3)
#define WINDOW_RESOLUTION_Y (240 * 3)

static uint8_t*    romData = nullptr;
static SDL_Window* window  = NULL;

/**
 * Cleanup all resources used by libraries for program exit.
 */
static void cleanup()
{
	if( romData != nullptr )
	{
		delete [] romData;
		romData = nullptr;
	}

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
 * Main emulation loop.
 */
static void mainLoop()
{
	NES nes(romData);

	bool running = true;
	while( running )
	{
		// Check input events
		SDL_Event event;
		while( SDL_PollEvent(&event) )
		{
			switch( event.type )
			{
			case SDL_QUIT:
				running = false;
				break;
			}
		}

		// Run a frame of emulation
		nes.stepFrame();
	}
}

/**
 * Program entry point.
 */
int main( int argc, char** argv )
{
	if( argc != 2 )
	{
		std::cout << "Please specify a ROM file to load as the second argument.\n";
		return -1;
	}

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
			// Load the ROM
			std::cout << "Loading ROM \"" << argv[1] << "\"\n";
			FILE* file = fopen(argv[1], "r");
			if( file == NULL )
			{
				std::cout << "Failed to open ROM file\n";
				cleanup();
				return -1;
			}

			fseek(file, 0L, SEEK_END);
			size_t fileSize = ftell(file);
			fseek(file, 0L, SEEK_SET);

			romData = new uint8_t[fileSize];
			fread(romData, sizeof(uint8_t), fileSize, file);
			fclose(file);

			// Run the emulator
			mainLoop();
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
