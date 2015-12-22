#include "DebugWindow.hpp"

DebugWindow::DebugWindow( const std::string& title, int width, int height, int scale ) :
	width(width),
	height(height)
{
	window = SDL_CreateWindow(
		title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width * scale,
		height * scale,
		0
	);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(renderer, width, height);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
}

DebugWindow::~DebugWindow()
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void DebugWindow::render( const uint32_t* pixels ) const
{
	SDL_UpdateTexture(texture, NULL, pixels, sizeof(uint32_t) * width);

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}


