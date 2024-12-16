#include "level.h"
#include "graphics.h"
#include "globals.h"
#include "rectangle.h"
#include "slope.h"
#include "tinyxml2.h"
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <iostream>
// #include <sstream>
// #include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include "fstools.h"
#include "utils.h"

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
    if (!mapNode) {
        return false;
    }

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
                Tileset tls{};
                for(auto &tileset : this->_tilesets)
                {
                    if(tileset.FirstGid <= gid)
                    {
                        tls = tileset;
                        break;
                    }
                }

                if (tls.FirstGid == -1) {
                    tileCounter++;
                    pTile = pTile->NextSiblingElement("tile");
                    continue;
                }

                int xx = tileCounter % this->_size.x * this->_tileSize.x;
                int yy = this->_tileSize.y * (tileCounter / this->_size.x); // 修正了除法的分母
                Vector2 finalTilePosition = Vector2(xx, yy);

                int tilesetWidth, tilesetHeight;
                if (SDL_QueryTexture(tls.Texture, NULL, NULL, &tilesetWidth, &tilesetHeight) != 0) {
                    // 处理 SDL_QueryTexture 失败的情况
                    return false;
                }

                int tsxx = (gid % (tilesetWidth / this->_tileSize.x) - 1) * this->_tileSize.x;
                int tsyy = (gid / (tilesetWidth / this->_tileSize.y)) * this->_tileSize.y;
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

void Level::loadObjects(XMLElement* objectGroup, std::function<void(float, float, float, float, XMLElement*)> callback) {
    XMLElement* pObject = objectGroup->FirstChildElement("object");
    while (pObject) {
        float x, y, width, height;
        if (pObject->QueryFloatAttribute("x", &x) == XML_SUCCESS &&
            pObject->QueryFloatAttribute("y", &y) == XML_SUCCESS &&
            pObject->QueryFloatAttribute("width", &width) == XML_SUCCESS &&
            pObject->QueryFloatAttribute("height", &height) == XML_SUCCESS) {
            callback(x, y, width, height, pObject);
        }
        pObject = pObject->NextSiblingElement("object");
    }
}
// fucking crazy currying
// std::function<std::function<std::function<std::function<std::function<void(XMLElement*)>(float)>(float)>(float)>(float)> curryCallback(std::function<void(float, float, float, float, XMLElement*)> callback) {
//     return [callback](float x) {
//         return [callback, x](float y) {
//             return [callback, x, y](float width) {
//                 return [callback, x, y, width](float height) {
//                     return [callback, x, y, width, height](XMLElement* pObject) {
//                         callback(x, y, width, height, pObject);
//                     };
//                 };
//             };
//         };
//     };
// }

void Level::loadCollisionRectangles(XMLElement* mapNode) {
    XMLElement* pObjectGroup = mapNode->FirstChildElement("objectgroup");
    while (pObjectGroup) {
        const char* name = pObjectGroup->Attribute("name");
        std::string groupName = name ? name : "";

        if (groupName == "collisions") {
            loadObjects(pObjectGroup, [this](float x, float y, float width, float height, XMLElement* pObject) {
                this->_collisionRects.push_back(Rectangle(
                    std::ceil(x) * globals::SPRITE_SCALE,
                    std::ceil(y) * globals::SPRITE_SCALE,
                    std::ceil(width) * globals::SPRITE_SCALE,
                    std::ceil(height) * globals::SPRITE_SCALE
                ));
            });
        }else if (groupName == "slopes") {
            XMLElement* pObject = pObjectGroup->FirstChildElement("object");
            while (pObject) {
                std::vector<Vector2> points;
                Vector2 p1 = Vector2(std::ceil(pObject->FloatAttribute("x")), std::ceil(pObject->FloatAttribute("y")));

                XMLElement* pPolyline = pObject->FirstChildElement("polyline");
                std::vector<std::string> pairs{};
                const std::string pointString = pPolyline->Attribute("points");
                Utils::split(pointString, pairs, ' ');

                for (auto& pair : pairs) {
                    std::vector<std::string> xy{};
                    Utils::split(pair, xy, ',');
                    if (xy.size() == 2) { // Ensure we have both x and y coordinates
                        Vector2 p2 = Vector2(std::ceil(std::stof(xy[0])), std::ceil(std::stof(xy[1])));
                        points.push_back(p2);
                    }
                }

                // Ensure there are at least two points to form a slope
                if (points.size() >= 2) {
                    for (size_t i = 0; i < points.size() - 1; ++i) {
                        Vector2 start = Vector2(
                            (p1.x + points[i].x) * globals::SPRITE_SCALE,
                            (p1.y + points[i].y) * globals::SPRITE_SCALE
                        );
                        Vector2 end = Vector2(
                            (p1.x + points[i + 1].x) * globals::SPRITE_SCALE,
                            (p1.y + points[i + 1].y) * globals::SPRITE_SCALE
                        );

                        this->_slopes.push_back(Slope(start, end));
                    }
                }

                pObject = pObject->NextSiblingElement("object");
            }
        }else if (groupName == "spawn points") {
            loadObjects(pObjectGroup, [this](float x, float y, float, float ,XMLElement* pObject) {
                
                const char* name = pObject->Attribute("name");
                if (name && std::string(name) == "player") {
                    this->_spawnPoint = Vector2(std::ceil(x) * static_cast<int>(globals::SPRITE_SCALE),
                                                std::ceil(y) * static_cast<int>(globals::SPRITE_SCALE));
                }
            });
        }

        pObjectGroup = pObjectGroup->NextSiblingElement("objectgroup");
    }
}


void Level::update(int elapsedTime) {

}

void Level::draw(Graphics &graphics) {
	for (auto & tile : _tileList) {
		tile.draw(graphics);
	}
}

std::vector<Slope> Level::checkSlopeCollisions(const Rectangle &other)
{
    std::vector<Slope>  others;
    for (auto& slope : _slopes)
    {
        if (slope.collidesWith(other)) {
            others.push_back(slope);
        }
    }
    return others;
}

std::vector<Rectangle> Level::checkTileCollisions(const Rectangle &other)
{
    std::vector<Rectangle> others;
    for (auto& rect : _collisionRects ) {
        if(rect.collidesWith(other))
        {
            others.push_back(rect);
        }
    }
    return others;
}

const Vector2 Level::getSpawnPoint() const
{
    return this->_spawnPoint;
}