#include "anmitedsprite.h"
#include "global.h"
#include "graphics.h"
#include "sprite.h"
#include <SDL_rect.h>
#include <string>
#include <utility>
#include <vector>

AnmitedSprite::AnmitedSprite(){}

AnmitedSprite::AnmitedSprite(Graphics &graphics, const std::string &filePath, int sourceX, int sourceY, int width, int height, float posX, float posY, float timeToUpdate):
    Sprite(graphics, filePath, sourceX, sourceY, width, height, posX, posY), _frameIndex(0), _timeToUpdate(timeToUpdate), _visible(true), _currentAnimationOnce(false), _currentAnimation("")
{}

void AnmitedSprite::addAnimation(int frames, int x, int y, std::string name, int width, int height, Vector2 offset)
{
    std::vector<SDL_Rect> rectangles;
    for(int i = 0; i < frames; i++)
    {
        SDL_Rect newRect = {(i + x) * width, y, width, height};
        rectangles.emplace_back(newRect);
    }

    this->_animations.insert(std::pair<std::string, std::vector<SDL_Rect>>(name, rectangles));
    this->_offsets.insert(std::pair<std::string, Vector2>(name, offset));
}

void AnmitedSprite::resetAnimations()
{
    this->_animations.clear();
    this->_offsets.clear();
}

void AnmitedSprite::playtAnimation(std::string animation, bool once)
{
    this->_currentAnimationOnce = once;
    if(this->_currentAnimation != animation)
    {
        this->_currentAnimation = animation;
        this->_frameIndex = 0;
    }
}

void AnmitedSprite::setVisible(bool visible)
{
    this->_visible = visible;
}

void AnmitedSprite::stopAnimation()
{
    this->_frameIndex = 0;
    this->animationDone(this->_currentAnimation);
}

void AnmitedSprite::update(int elapsedTime)
{
    Sprite::update();

    this->_timeElapsed += elapsedTime;
    if(this->_timeElapsed > this->_timeToUpdate)
    {
        if (this->_frameIndex < this->_animations[this->_currentAnimation].size() - 1) {
            this->_frameIndex++;
        }else {
            if(this->_currentAnimationOnce == true)
            {
                this->setVisible(false);
            }
            this->_frameIndex = 0;
        }
    }
}

void AnmitedSprite::draw(Graphics &graphics, int x, int y)
{
    if (this->_visible) {
        SDL_Rect destinaionRectangle;
        destinaionRectangle.x = x + this->_offsets[this->_currentAnimation].x;
        destinaionRectangle.y = y + this->_offsets[this->_currentAnimation].y;
        destinaionRectangle.w = this->_sourceRect.w * globals::SPRITE_SCALE;
        destinaionRectangle.h = this->_sourceRect.y * globals::SPRITE_SCALE;

        SDL_Rect sourceRect = this->_animations[this->_currentAnimation][this->_frameIndex];
        graphics.blitSurface(this->_spriteSheet, &sourceRect, &destinaionRectangle);
    }
}

void AnmitedSprite::animationDone(std::string currentAnimation)
{}

void AnmitedSprite::setupAnimations()
{
    this->(3, 0, 0, "RunLeft", 16, 16,)
}