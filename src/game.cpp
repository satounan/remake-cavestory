#include "animatedsprite.h"
#include <SDL_scancode.h>
#include <SDL_timer.h>
#include <input.h>
#include <SDL.h>
#include <SDL_events.h>
#include <game.h>
#include <graphics.h>

namespace {
    constexpr int FPS = 50;
    constexpr int MAX_FRAME_TIME = 1000 / FPS;
}

Game::Game()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    this->gameloop();
}

Game::~Game()
{

}

void Game::gameloop()
{
    Graphics graphics;
    Input input;
    SDL_Event event;

    this->_player = Player(graphics,100,100);


    int LAST_UPDATE_TIME = SDL_GetTicks();

    while (true) {
        input.beginNewFrame();
        if(SDL_PollEvent(&event))
        {
            if(event.type == SDL_KEYDOWN)
            {
                if(event.key.repeat == 0)
                {
                    input.keyDownEvent(event);
                }
            }else if (event.type ==SDL_KEYUP) {
                input.keyUpEvent(event);
            }else if(event.type == SDL_QUIT)
            {
                return;
            }
        }

        if (input.wasKeyPresssed(SDL_SCANCODE_ESCAPE))
        {
            return;
        }

        const int CURRENT_TIME_MS = SDL_GetTicks();
        int ELAPSED_TIME_MS = CURRENT_TIME_MS -LAST_UPDATE_TIME;
        this->update(std::min(ELAPSED_TIME_MS,MAX_FRAME_TIME));
        LAST_UPDATE_TIME = CURRENT_TIME_MS;

        this->draw(graphics);
    }
};
void Game::draw(Graphics &graphics)
{
    graphics.clear();

    this->_player.draw(graphics);

    graphics.filp();
};
void Game::update(float elapsedtime)
{
    this->_player.update(elapsedtime);
};