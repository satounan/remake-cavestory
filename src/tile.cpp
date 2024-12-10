#include "tile.h"
#include "graphics.h"
#include <SDL2/SDL.h>

Tile::Tile() {}

Tile::Tile(SDL_Texture* tileset, Vector2 size, Vector2 tilesetPosition, Vector2 position) :
		_tileset(tileset),
		_size(size),
		_tilesetPosition(tilesetPosition),
		_position(Vector2(position.x * static_cast<int>(globals::SPRITE_SCALE), position.y * static_cast<int>(globals::SPRITE_SCALE)))
{}

void Tile::update(int elapsedTime) {}

void Tile::draw(Graphics &graphics) {
	SDL_Rect destRect = { this->_position.x, this->_position.y,
			this->_size.x * static_cast<int>(globals::SPRITE_SCALE), this->_size.y * static_cast<int>(globals::SPRITE_SCALE) };
	SDL_Rect sourceRect = { this->_tilesetPosition.x, this->_tilesetPosition.y, this->_size.x, this->_size.y };

	graphics.blitSurface(this->_tileset, &sourceRect, &destRect);
}
