#include <SDL2/SDL.h>
#include <SDL_scancode.h>
#include <iostream>
#include <vector>
#include "game.h"
#include "graphics.h"
#include "input.h"
#include "slope.h"

/* Game class
 * This class holds all information for our main game loop
 */

namespace {
	const int FPS = 50;
	const int MAX_FRAME_TIME = 1000 / FPS;
}

Game::Game() {
	SDL_Init(SDL_INIT_EVERYTHING);
	this->gameLoop();
}

Game::~Game() {

}

void Game::gameLoop() {
	try{	
		Graphics graphics;
		Input input;
		SDL_Event event;

		this->_level = Level("Map1", Vector2(100, 100), graphics);
		this->_player = Player(graphics, this->_level.getSpawnPoint());

		int LAST_UPDATE_TIME = SDL_GetTicks();
		//Start the game loop
		while (true) {
			input.beginNewFrame();

			if (SDL_PollEvent(&event)) {
				if (event.type == SDL_KEYDOWN) {
					if (event.key.repeat == 0) {
						input.keyDownEvent(event);
					}
				}
				else if (event.type == SDL_KEYUP) {
					input.keyUpEvent(event);
				}
				else if (event.type == SDL_QUIT) {
					return;
				}
			}
			if (input.wasKeyPressed(SDL_SCANCODE_ESCAPE) == true) {
				return;
			}
			else if (input.isKeyHeld(SDL_SCANCODE_LEFT) == true) {
				this->_player.moveLeft();
			}
			else if (input.isKeyHeld(SDL_SCANCODE_RIGHT) == true) {
				this->_player.moveRight();
			}

			if (input.wasKeyPressed(SDL_SCANCODE_Z) ) {//对于英文字母按键需要切换输入法
				this->_player.jump();
			}

			if (!input.isKeyHeld(SDL_SCANCODE_LEFT) && !input.isKeyHeld(SDL_SCANCODE_RIGHT)) {
				this->_player.stopMoving();
			}

			const int CURRENT_TIME_MS = SDL_GetTicks();
			int ELAPSED_TIME_MS = CURRENT_TIME_MS - LAST_UPDATE_TIME;
			this->update(std::min(ELAPSED_TIME_MS, MAX_FRAME_TIME));
			LAST_UPDATE_TIME = CURRENT_TIME_MS;

			this->draw(graphics);
		}
	} catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}
}

void Game::draw(Graphics &graphics) {
	graphics.clear();

	this->_level.draw(graphics);
	this->_player.draw(graphics);

	graphics.flip();
}

void Game::update(float elapsedTime) {
	this->_player.update(elapsedTime);
	this->_level.update(elapsedTime);

	std::vector<Rectangle> others;
	if ((others = this->_level.checkTileCollisions(this->_player.getBoundingBox())).size() > 0) {
		this->_player.handleTileCollisions(others);
	}

	std::vector<Slope> otherSlopes;
	if((otherSlopes = this->_level.checkSlopeCollisions(this->_player.getBoundingBox())).size() > 0)
	{
		this->_player.handleSlopeCollisions(otherSlopes);
	}
}
