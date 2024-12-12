#include "level.h"
#include "graphics.h"
#include "globals.h"
#include "tinyxml2.h"
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <vector>
#include "fstools.h"

using namespace tinyxml2;
// std::filesystem::path neow = std::filesystem::current_path();
// std::filesystem::path fix = truncatePathAtDirectoryName(neow, "remake_cavestory");

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
    XMLDocument doc;
    if (!parseMapFile(mapName, doc)) {
        return;
    }

    XMLElement* mapNode = doc.FirstChildElement("map");
    if (!mapNode) {
        std::cerr << "Invalid map file format: No 'map' node found." << std::endl;
        return;
    }

    if (!loadMapProperties(mapNode)) {
        return;
    }

    if (!loadTilesets(mapNode, graphics)) {
        return;
    }

    if (!loadLayers(mapNode)) {
        return;
    }

    loadCollisionRectangles(mapNode);
}

bool Level::parseMapFile(const std::string& mapName, XMLDocument& doc) {

    std::filesystem::path ss = fix / "content" / "maps" / std::filesystem::path{mapName + ".tmx"};

    XMLError result = doc.LoadFile(ss.string().c_str());
    if (result != XML_SUCCESS) {
        std::cerr << "Failed to load map file: " << ss << std::endl;
        return false;
    }
    return true;
}

bool Level::loadMapProperties(XMLElement* mapNode) {
    int width, height;
    if (mapNode->QueryIntAttribute("width", &width) != XML_SUCCESS ||
        mapNode->QueryIntAttribute("height", &height) != XML_SUCCESS) {
        std::cerr << "Invalid map file format: Missing or invalid 'width' or 'height' attribute." << std::endl;
        return false;
    }
    this->_size = Vector2(width, height);

    int tileWidth, tileHeight;
    if (mapNode->QueryIntAttribute("tilewidth", &tileWidth) != XML_SUCCESS ||
        mapNode->QueryIntAttribute("tileheight", &tileHeight) != XML_SUCCESS) {
        std::cerr << "Invalid map file format: Missing or invalid 'tilewidth' or 'tileheight' attribute." << std::endl;
        return false;
    }
    this->_tileSize = Vector2(tileWidth, tileHeight);
    return true;
}

bool Level::loadTilesets(XMLElement* mapNode, Graphics &graphics) {
    XMLElement* pTileset = mapNode->FirstChildElement("tileset");
    while (pTileset) {
        int firstgid;
        const char* source_related = pTileset->FirstChildElement("image")->Attribute("source");

        const std::string source_wrong = removeLeadingDotDot(source_related); // 相对地址转绝对，截断..

        std::filesystem::path source = fix / std::filesystem::path{"content/"} / source_wrong;
        if (source.empty()) {
            std::cerr << "Invalid map file format: Missing 'source' attribute in 'image' element." << std::endl;
            return false;
        }
        pTileset->QueryIntAttribute("firstgid", &firstgid);
        SDL_Surface* surface = graphics.loadImage(source.string());
        if (!surface) {
            std::cerr << "Failed to load image: " << source << std::endl;
            return false;
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(graphics.getRenderer(), surface);
        SDL_FreeSurface(surface);
        if (!tex) {
            std::cerr << "Failed to create texture from surface: " << source << std::endl;
            return false;
        }
        this->_tilesets.push_back(Tileset(tex, firstgid));

        pTileset = pTileset->NextSiblingElement("tileset");
    }
    return true;
}

bool Level::loadLayers(XMLElement* mapNode) {
    XMLElement* pLayer = mapNode->FirstChildElement("layer");
    while (pLayer) {
        XMLElement* pData = pLayer->FirstChildElement("data");
        while (pData) {
            XMLElement* pTile = pData->FirstChildElement("tile");
            int tileCounter = 0;
            while (pTile) {
                if (pTile->IntAttribute("gid") == 0) {
                    tileCounter++;
                    pTile = pTile->NextSiblingElement("tile");
                    continue;
                }

                int gid = pTile->IntAttribute("gid");
                Tileset tls;
                bool found = false;
                for (int i = 0; i < this->_tilesets.size(); i++) {
                    if (this->_tilesets[i].FirstGid <= gid) {
                        tls = this->_tilesets.at(i);
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    tileCounter++;
                    pTile = pTile->NextSiblingElement("tile");
                    continue;
                }

                int xx = 0;
                int yy = 0;
                xx = tileCounter % this->_size.x;
                xx *= this->_tileSize.x;
                yy += this->_tileSize.y * (tileCounter / this->_size.x);
                Vector2 finalTilePosition = Vector2(xx, yy);

                int tilesetWidth, tilesetHeight;
                SDL_QueryTexture(tls.Texture, NULL, NULL, &tilesetWidth, &tilesetHeight);
                int tsxx = gid % (tilesetWidth / this->_tileSize.x) - 1;
                tsxx *= this->_tileSize.x;
                int tsyy = 0;
                int amt = (gid / (tilesetWidth / this->_tileSize.y));
                tsyy = this->_tileSize.y * amt;
                Vector2 finalTilesetPosition = Vector2(tsxx, tsyy);

                Tile tile(tls.Texture, Vector2(this->_tileSize.x, this->_tileSize.y),
                           finalTilesetPosition, finalTilePosition);
                this->_tileList.push_back(tile);
                tileCounter++;

                pTile = pTile->NextSiblingElement("tile");
            }

            pData = pData->NextSiblingElement("data");
        }

        pLayer = pLayer->NextSiblingElement("layer");
    }
    return true;
}

void Level::loadCollisionRectangles(XMLElement* mapNode) {
    XMLElement* pObjectGroup = mapNode->FirstChildElement("objectgroup");
    while (pObjectGroup) {
        const char* name = pObjectGroup->Attribute("name");
        if (name && std::string(name) == "collisions") {
            XMLElement* pObject = pObjectGroup->FirstChildElement("object");
            while (pObject) {
                float x, y, width, height;
                x = pObject->FloatAttribute("x");
                y = pObject->FloatAttribute("y");
                width = pObject->FloatAttribute("width");
                height = pObject->FloatAttribute("height");

                this->_collisionRects.push_back(Rectangle(
                    std::ceil(x) * globals::SPRITE_SCALE,
                    std::ceil(y) * globals::SPRITE_SCALE,
                    std::ceil(width) * globals::SPRITE_SCALE,
                    std::ceil(height) * globals::SPRITE_SCALE
                ));

                pObject = pObject->NextSiblingElement("object");
            }
        }

        pObjectGroup = pObjectGroup->NextSiblingElement("objectgroup");
    }
}
void Level::update(int elapsedTime) {

}

void Level::draw(Graphics &graphics) {
	for (int i = 0; i < this->_tileList.size(); i++) {
		this->_tileList.at(i).draw(graphics);
	}
}

std::vector<Rectangle> Level::checkTileCollisions(const Rectangle &other)
{
    std::vector<Rectangle> others;
    for (int i = 0; i< this->_collisionRects.size(); i++) {
        if(this->_collisionRects.at(i).collidesWith(other) )
        {
            others.push_back(this->_collisionRects.at(i));
        }
    }
    return others;
}