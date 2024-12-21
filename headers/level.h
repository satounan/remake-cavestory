#ifndef LEVEL_H
#define LEVEL_H

#include <SDL_render.h>
#include <functional>
#include <string>
#include <vector>

#include "animatedtile.h"
#include "slope.h"
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
	std::vector<Slope> checkSlopeCollisions(const Rectangle &other);

	const Vector2 getSpawnPoint() const;

private:
	std::string _mapName;
	Vector2 _spawnPoint;

	Vector2 _size;
	Vector2 _tileSize;

	SDL_Texture* _backgroundTexture;

	std::vector<Tile> _tileList;
	std::vector<Tileset> _tilesets;
	std::vector<Rectangle> _collisionRects;
	std::vector<Slope> _slopes;

	std::vector<AnimatedTile> _animatedTileList;
	std::vector<AnimatedTileInfo> _animatedTileInfos;

	/* void loadMap
	 * Loads a map
	 */
	void loadMap(std::string mapName, Graphics &graphics);
	bool parseMapFile(const std::string& mapName, tinyxml2::XMLDocument& doc);
	bool loadMapProperties(tinyxml2::XMLElement* mapNode);
	bool loadTilesets(tinyxml2::XMLElement* mapNode, Graphics &graphics);
	bool loadLayers(tinyxml2::XMLElement* mapNode);

	void loadObjects(tinyxml2::XMLElement* objectGroup, std::function<void(float, float, float, float, tinyxml2::XMLElement* )> callback);


	void loadCollisionRectangles(tinyxml2::XMLElement* mapNode);
	void loadCollisions(tinyxml2::XMLElement* mapNode);

	Vector2 getTilesetPosition(Tileset tls, int gid, int tileWidth, int tileHeight);
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
