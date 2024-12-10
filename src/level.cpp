#include "level.h"
#include "global.h"
#include "graphics.h"
#include <SDL_rect.h>
#include <SDL_render.h>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <tinyxml2.h>

using namespace tinyxml2;

Level::Level(){}

Level::Level(std::string mapName, Vector2 spawnPoint, Graphics &graphics):
    _mapName(mapName),
    _spawnPoint(spawnPoint),
    _size(Vector2{0,0})
{
    this->loadMap(mapName, graphics);
}

Level::~Level(){}

void Level::loadMap(std::string mapName, Graphics &graphics)
{
    XMLDocument doc;
    std::stringstream ss1{};

    ss1 << "../content/maps/" << mapName << ".tmx\0";

    std::cout << ss1.str();
    doc.LoadFile(ss1.str().c_str());

    XMLElement* mapNode = doc.FirstChildElement("map");

    int width, height;
    mapNode->QueryIntAttribute("width", &width);
    mapNode->QueryIntAttribute("height", &height);
    this->_size = Vector2(width, height);

    int tileWidth, tileHeight;
    mapNode->QueryIntAttribute("tilewidth", &tileWidth);
    mapNode->QueryIntAttribute("tileheight", &tileHeight);
    this->_tileSize = Vector2(tileWidth, tileHeight);

    XMLElement* pTileset = mapNode->FirstChildElement("tileset");
    if(pTileset != nullptr)
    {
        while (pTileset) {
            int firstgid;
            const char* source = pTileset->FirstChildElement("image")->Attribute("source");
            char* path;
            std::stringstream ss;
            // ss << "../content/tilesets/" << source;
            ss << source;

            // std::cout << ss.str();
            pTileset->QueryIntAttribute("firstgid", &firstgid);
            SDL_Texture* tex = SDL_CreateTextureFromSurface(graphics.getRenderer(), graphics.loadImage(ss.str()));
            this->_tilesets.push_back(Tileset(tex, firstgid));

            pTileset = pTileset->NextSiblingElement("tileset");
        }
    }    

    XMLElement* pLayer = mapNode->FirstChildElement("layer");
    if(pLayer != nullptr)
    {
        while (pLayer) {
            XMLElement* pData = pLayer->FirstChildElement("data");
            if (pData != nullptr) {
                while (pData) {
                    XMLElement* pTile = pData->FirstChildElement("tile");
                    if (pTile != nullptr) {
                        int tileCounter = 0;
                        while (pTile) {
                            if (pTile->IntAttribute("gid") == 0) {
                                tileCounter++;
                                if (pTile->NextSiblingElement("tile")) {
                                    pTile = pTile->NextSiblingElement("tile");
                                    continue;
                                }else {
                                    break;
                                }
                            }

                            int gid = pTile->IntAttribute("gid");
                            Tileset tls;
                            for(int i = 0; i < this->_tilesets.size(); i++)
                            {
                                if(this->_tilesets[i].FirstGid <= gid)
                                {
                                    tls = this->_tilesets.at(i);
                                    break;
                                }
                            }

                            if (tls.FirstGid == -1) {
                                tileCounter++;
                                if (pTile->NextSiblingElement("tile")) {
                                    pTile = pTile->NextSiblingElement("tile");
                                    continue;
                                }else {
                                    break;
                                }
                            }

                            int xx = 0;
                            int yy = 0;

                            xx = tileCounter % width;
                            xx *= tileWidth;
                            yy += tileHeight * (tileCounter / width);
                            Vector2 finalTilePosition = Vector2(xx,yy);

                            int tilesetWidth, tilesetHeight;
                            SDL_QueryTexture(tls.Texture, 0, 0, &tilesetWidth, &tilesetHeight);
                            int tsxx = gid % (tilesetWidth / tileWidth) -1;
                            tsxx *= tileWidth;
                            int tsyy = 0;
                            int amt = (gid / (tilesetWidth / tileWidth));
                            tsyy = tileHeight * amt;
                            Vector2 finalTilesetPosition = Vector2(tsxx, tsyy);

                            Tile tile(tls.Texture, Vector2(tileWidth, tileHeight), finalTilesetPosition, finalTilePosition);
                            this->_tileList.push_back(tile);
                            tileCounter++;

                            pTile = pTile->NextSiblingElement("tile");
                        }
                    }

                    pData = pData->NextSiblingElement("data");
                }
            }

            pLayer = pLayer->NextSiblingElement("layer");
        }
    }

}

void Level::update(int elapsedTime)
{

}

void Level::draw(Graphics &graphics)
{
    // SDL_Rect sourceRect = {0, 0, 64, 64};
    // SDL_Rect destRect;
    // for(int x = 0; x < this->_size.x / 64; x++)
    // {
    //     for (int y = 0 ; y < this->_size.y / 64; y++) {
    //         destRect.x = x * 64 * globals::SPRITE_SCALE;
    //         destRect.y = y * 64 * globals::SPRITE_SCALE;
    //         destRect.w = 64 * globals::SPRITE_SCALE;
    //         destRect.h = 64 * globals::SPRITE_SCALE;
    //         graphics.blitSurface(this->_backgroundTexture, &sourceRect, &destRect);
    //     }
    // }

    for (int i = 0; i < this->_tileList.size(); i++) {
        this->_tileList.at(i).draw(graphics);
    }
}