#pragma once

#include "animatedsprite.h"
#include "global.h"
#include "graphics.h"

class Player: public AnimatedSprite
{
    public:
    Player();
    Player(Graphics& graphics, float x, float y);
    void draw(Graphics &graphics);
    void update(float elapsedTime);

    void moveLeft();
    void moveRight();
    void stopMoving();

    virtual void animationDone(std::string currentAnimation);
    virtual void setupAnimations();

    private:
    float _dx, _dy;

    Direction _facing;
};