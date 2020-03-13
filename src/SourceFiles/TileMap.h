#pragma once
#include "Texture.h"
#include "Tileson.h"
class TileMap
{
protected:
	int map_[9][16];
	std::pair<int, int> res_; //resolucion de la pantalla
	const int SIZE = 32;
	tson::Map tMap_;
	void drawTile(tson::Tile* t);
private:
	Texture* debugT_;
	Texture* tinkyT_; 
public:
	TileMap();
	~TileMap();
	void drawMap();
	void drawTMap();
	void loadTileson(string path);
};

