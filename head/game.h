#pragma once

class Graphics;

class Game
{
    public:
        Game();
        ~Game();

    private:
        void gameloop();
        void draw(Graphics &graphics);
        void update(float elapsedtime);

};