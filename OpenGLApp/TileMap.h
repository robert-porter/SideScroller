#pragma once

#include "Renderer.h"
#include "FMPLoader.h"



const int __TOP				= 1;
const int __TOP__RIGHT		= 2;
const int __RIGHT			= 4;
const int __BOTTOM__RIGHT	= 8;
const int __BOTTOM			= 16;
const int __BOTTOM_LEFT		= 32;
const int __LEFT			= 64;
const int __LEFT_TOP		= 128;



struct Tile
{
	TextureID texture;
	bool blocked;
};

class TileMap
{
public:
	TileMap(void);
	~TileMap(void);

	int Load(char *, int);
	void Draw(Renderer &renderer);

private:

	int numCols, numRows;
	int numLayers;
	float tileWidth, tileHeight; // world size
	Tile *tiles;

	Vector2 offset; // starts at (0,0) and goes into the 1st quadrant + offset

	Tile *GetTile(int layer, int x, int y)
	{
		return &tiles[layer * numCols * numRows + y * numCols + x];
	}
};
