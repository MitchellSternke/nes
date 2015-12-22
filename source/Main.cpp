/**
 * @file
 * Contains the program entry point.
 */

#include <cstdio>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "DebugWindow.hpp"
#include "NES.hpp"

static uint8_t* romData = nullptr;

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

	// Enable double buffering
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	// Enable vsync
	SDL_GL_SetSwapInterval(1);

	return 0;
}

/**
 * Load a ROM from a file.
 */
static bool loadROM( const std::string& filename )
{
	std::cout << "Loading ROM \"" << filename << "\"\n";

	FILE* file = fopen(filename.c_str(), "r");
	if( file == NULL )
	{
		return false;
	}

	fseek(file, 0L, SEEK_END);
	size_t fileSize = ftell(file);
	fseek(file, 0L, SEEK_SET);

	romData = new uint8_t[fileSize];
	fread(romData, sizeof(uint8_t), fileSize, file);
	fclose(file);

	return true;
}

/**
 * Main emulation loop.
 */
static void mainLoop()
{
	NES nes(romData);

#if 0
	DebugWindow patternTableWindow("Pattern Table", 256, 128, 2);
	DebugWindow nametableWindow("Nametables", 512, 480, 2);
	DebugWindow paletteWindow("Palette", 4, 8, 32);
#endif
	DebugWindow mainWindow("NES", 256, 240, 3);

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
			case SDL_WINDOWEVENT:
				switch( event.window.event )
				{
				case SDL_WINDOWEVENT_CLOSE:
					running = false;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}

		// Run a frame of emulation
		nes.stepFrame();

		// Render
#if 0
		uint32_t* patternTable = nes.getPPU().getVisualPatternTable();
		patternTableWindow.render(patternTable);
		delete [] patternTable;

		uint32_t* nametable = nes.getPPU().getVisualNametable();
		nametableWindow.render(nametable);
		delete [] nametable;

		uint32_t* palette = nes.getPPU().getVisualPalette();
		paletteWindow.render(palette);
		delete [] palette;
#endif
		mainWindow.render(nes.getPPU().getFrameBuffer());
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
			if( !loadROM(argv[1]) )
			{
				std::cout << "Failed to open ROM file\n";
				cleanup();
				return -1;
			}

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
