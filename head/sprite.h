#pragma once

#include <SDL_rect.h>
#include <SDL_render.h>
#include <string>
class Graphics;
// struct SDL_Texture;

class Sprite
{
    public:
    Sprite();
    Sprite(Graphics &graphics, const std::string &filePath, int sourceX, int sourceY, int widght, int height, float posX, float posY);

    virtual ~Sprite();
    virtual void update();
    void draw(Graphics &graphics, int x, int y);


    private:
    SDL_Rect _sourceRect;
    SDL_Texture* _spriteSheet;

    float _X,_y;
};