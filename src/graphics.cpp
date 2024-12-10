// #include <SDL2/SDL.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <graphics.h>
#include <SDL_image.h>
#include <global.h>

Graphics::Graphics()
{
    SDL_CreateWindowAndRenderer(globals::SCREEN_WIDTH, globals::SCREEN_HEIGHT, 0, &this->_window, &this->_renderer);
    SDL_SetWindowTitle(this->_window, "Cavestory");
}

Graphics::~Graphics()
{
    SDL_DestroyWindow(this->_window);
    SDL_DestroyRenderer(this->_renderer);
}

SDL_Surface* Graphics::loadImage(const std::string &filePath)
{
    if (!this->_spriteSheets.count(filePath))
    {
        this->_spriteSheets[filePath] = IMG_Load(filePath.c_str());
    }
    return this->_spriteSheets[filePath];
}

void Graphics::blitSurface(SDL_Texture* texture, SDL_Rect* sourceRectangle,  SDL_Rect* destinationRectangle)
{
    SDL_RenderCopy(this->_renderer, texture, sourceRectangle, destinationRectangle);
}

void Graphics::flip()
{
    SDL_RenderPresent(this->_renderer);
}
//渲染到画面

void Graphics::clear()
{
    SDL_RenderClear(this->_renderer);
}

SDL_Renderer* Graphics::getRenderer() const
{
    return this->_renderer;
}