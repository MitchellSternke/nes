#ifndef DEBUGWINDOW_HPP
#define DEBUGWINDOW_HPP

#include <string>

#include <SDL2/SDL.h>

/**
 * A debug window used to output pixels.
 */
class DebugWindow
{
public:
	DebugWindow( const std::string& title, int width, int height, int scale = 1 );
	~DebugWindow();

	void render( const uint32_t* pixels ) const;

private:
	int width;
	int height;
	SDL_Window*   window;
	SDL_Renderer* renderer;
	SDL_Texture*  texture;
};

#endif // DEBUGWINDOW_HPP
