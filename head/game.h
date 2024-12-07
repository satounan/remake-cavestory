#pragma once

#include "animatedsprite.h"
#include "player.h"
class Graphics;

class Game
{
    public:
        Game();
        ~Game();

    private:
        void gameLoop();
        void draw(Graphics &graphics);
        void update(float elapsedtime);

        Player _player;

};