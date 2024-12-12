#ifndef LEVEL_H
#define LEVEL_H

#include <SDL_render.h>
#include <string>
#include <vector>
#include <filesystem>

#include "tinyxml2.h"
#include "globals.h"
#include "tile.h"
#include "rectangle.h"

class Graphics;
struct SDL_Texture;
struct SDL_Rect;
struct Tileset;

class Level {
public:
	Level();
	Level(std::string mapName, Vector2 spawnPoint, Graphics &graphics);
	~Level();
	void update(int elapsedTime);
	void draw(Graphics &graphics);

	std::vector<Rectangle> checkTileCollisions(const Rectangle &other);

private:
	std::string _mapName;
	Vector2 _spawnPoint;

	Vector2 _size;
	Vector2 _tileSize;

	SDL_Texture* _backgroundTexture;

	std::vector<Tile> _tileList;
	std::vector<Tileset> _tilesets;
	std::vector<Rectangle> _collisionRects;

	/* void loadMap
	 * Loads a map
	 */
	void loadMap(std::string mapName, Graphics &graphics);
	bool parseMapFile(const std::string& mapName, tinyxml2::XMLDocument& doc);
	bool loadMapProperties(tinyxml2::XMLElement* mapNode);
	bool loadTilesets(tinyxml2::XMLElement* mapNode, Graphics &graphics);
	bool loadLayers(tinyxml2::XMLElement* mapNode);

	
	void loadCollisionRectangles(tinyxml2::XMLElement* mapNode);
	void loadCollisions(tinyxml2::XMLElement* mapNode);

};

//Tileset structure
struct Tileset {
	SDL_Texture* Texture;
	int FirstGid;
	Tileset() {
		this->FirstGid = -1;
	}
	Tileset(SDL_Texture* texture, int firstGid) {
		this->Texture = texture;
		this->FirstGid = firstGid;
	}
};




#endif
