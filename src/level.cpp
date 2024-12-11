#include "level.h"
#include "graphics.h"
#include "globals.h"
#include "tinyxml2.h"
#include <SDL2/SDL.h>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include "fstools.h"

using namespace tinyxml2;

Level::Level() {}

Level::Level(std::string mapName, Vector2 spawnPoint, Graphics &graphics) :
		_mapName(mapName),
		_spawnPoint(spawnPoint),
		_size(Vector2(0,0))
{
	this->loadMap(mapName, graphics);
}

Level::~Level() {

}


void Level::loadMap(std::string mapName, Graphics &graphics) {
    // Parse the .tmx file
    XMLDocument doc;
    std::filesystem::path neow = std::filesystem::current_path();
    std::filesystem::path fix = truncatePathAtDirectoryName(neow, "remake_cavestory");
    std::filesystem::path ss = fix / "content" / "maps" / std::filesystem::path{mapName+".tmx"};

    XMLError result = doc.LoadFile(ss.string().c_str());
    if (result != XML_SUCCESS) {
        std::cerr << "Failed to load map file: " << ss << std::endl;
        return;
    }

    XMLElement* mapNode = doc.FirstChildElement("map");
    if (!mapNode) {
        std::cerr << "Invalid map file format: No 'map' node found." << std::endl;
        return;
    }

    // Get the width and the height of the whole map and store it in _size
    int width, height;
    if (mapNode->QueryIntAttribute("width", &width) != XML_SUCCESS || 
        mapNode->QueryIntAttribute("height", &height) != XML_SUCCESS) {
        std::cerr << "Invalid map file format: Missing or invalid 'width' or 'height' attribute." << std::endl;
        return;
    }
    this->_size = Vector2(width, height);

    // Get the width and the height of the tiles and store it in _tileSize
    int tileWidth, tileHeight;
    if (mapNode->QueryIntAttribute("tilewidth", &tileWidth) != XML_SUCCESS || 
        mapNode->QueryIntAttribute("tileheight", &tileHeight) != XML_SUCCESS) {
        std::cerr << "Invalid map file format: Missing or invalid 'tilewidth' or 'tileheight' attribute." << std::endl;
        return;
    }
    this->_tileSize = Vector2(tileWidth, tileHeight);

    // Loading the tilesets
    XMLElement* pTileset = mapNode->FirstChildElement("tileset");
    while (pTileset) {
        int firstgid;
        const char* source_related = pTileset->FirstChildElement("image")->Attribute("source");

        const std::string source_wrong = removeLeadingDotDot(source_related); // 相对地址转绝对，截断..

        std::filesystem::path source = fix / std::filesystem::path{"content/"} / source_wrong;
        if (source.empty()) {
            std::cerr << "Invalid map file format: Missing 'source' attribute in 'image' element." << std::endl;
            return;
        }
        pTileset->QueryIntAttribute("firstgid", &firstgid);
        SDL_Surface* surface = graphics.loadImage(source.string());
        if (!surface) {
            std::cerr << "Failed to load image: " << source << std::endl;
            return;
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(graphics.getRenderer(), surface);
        SDL_FreeSurface(surface);
        if (!tex) {
            std::cerr << "Failed to create texture from surface: " << source << std::endl;
            return;
        }
        this->_tilesets.push_back(Tileset(tex, firstgid));

        pTileset = pTileset->NextSiblingElement("tileset");
    }

    // Loading the layers
    XMLElement* pLayer = mapNode->FirstChildElement("layer");
    while (pLayer) {
        // Loading the data element
        XMLElement* pData = pLayer->FirstChildElement("data");
        while (pData) {
            // Loading the tile element
            XMLElement* pTile = pData->FirstChildElement("tile");
            int tileCounter = 0;
            while (pTile) {
                // Build each individual tile here
                // If gid is 0, no tile should be drawn. Continue loop
                if (pTile->IntAttribute("gid") == 0) {
                    tileCounter++;
                    pTile = pTile->NextSiblingElement("tile");
                    continue;
                }

                // Get the tileset for this specific gid
                int gid = pTile->IntAttribute("gid");
                Tileset tls;
                bool found = false;
                for (int i = 0; i < this->_tilesets.size(); i++) {
                    if (this->_tilesets[i].FirstGid <= gid) {
                        // This is the tileset we want
                        tls = this->_tilesets.at(i);
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    // No tileset was found for this gid
                    tileCounter++;
                    pTile = pTile->NextSiblingElement("tile");
                    continue;
                }

                // Get the position of the tile in the level
                int xx = 0;
                int yy = 0;
                xx = tileCounter % width;
                xx *= tileWidth;
                yy += tileHeight * (tileCounter / width);
                Vector2 finalTilePosition = Vector2(xx, yy);

                // Calculate the position of the tile in the tileset
                int tilesetWidth, tilesetHeight;
                SDL_QueryTexture(tls.Texture, NULL, NULL, &tilesetWidth, &tilesetHeight);
                int tsxx = gid % (tilesetWidth / tileWidth) - 1;
                tsxx *= tileWidth;
                int tsyy = 0;
                int amt = (gid / (tilesetWidth / tileWidth));
                tsyy = tileHeight * amt;
                Vector2 finalTilesetPosition = Vector2(tsxx, tsyy);

                // Build the actual tile and add it to the level's tile list
                Tile tile(tls.Texture, Vector2(tileWidth, tileHeight),
                           finalTilesetPosition, finalTilePosition);
                this->_tileList.push_back(tile);
                tileCounter++;

                pTile = pTile->NextSiblingElement("tile");
            }

            pData = pData->NextSiblingElement("data");
        }

        pLayer = pLayer->NextSiblingElement("layer");
    }
}

void Level::update(int elapsedTime) {

}

void Level::draw(Graphics &graphics) {
	for (int i = 0; i < this->_tileList.size(); i++) {
		this->_tileList.at(i).draw(graphics);
	}
}
