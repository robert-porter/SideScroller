#include "TileMap.h"
#include "MathUtils.h"
#include "Camera.h"

TileMap::TileMap()
{
	tileWidth = 1.2f;
	tileHeight = 1.2f;
}

TileMap::~TileMap()
{

}

void TileMap::Draw(Renderer &renderer)
{

	Camera *pCam = renderer.GetCamera();


	int x0 = (pCam->position.x - pCam->halfSize.x - offset.x) / tileWidth;
	int	x1 = 2 + (pCam->position.x + pCam->halfSize.x - offset.x) / tileWidth;  // Why + 2 and not +1???
	int	y0 = (pCam->position.y - pCam->halfSize.y - offset.y) / tileHeight;
	int	y1 = 2 + (pCam->position.y - offset.y + pCam->halfSize.y) / tileHeight;


	x0 = Clamp(x0, 0, numCols);
	x1 = Clamp(x1, 0, numCols);
	y0 = Clamp(y0, 0, numRows);
	y1 = Clamp(y1, 0, numRows);

	for(int z = 0; z < numLayers; z++)
		for(int x = x0; x < x1; x++)
			for(int y = y0; y < y1; y++)
			{
				renderer.DrawTexturedQuad(GetTile(z, x, y)->texture, 
					Vector2(x*tileWidth, y*tileHeight) + offset, tileWidth, tileHeight, 0, Vector2(0,0), false, false, Color4f(1,1,1,1));
			}
}


int TileMap::Load(char *fileName, int textureStartIndex)
{
	FMPLoader loader;
	loader.Load(fileName, textureStartIndex);

	numCols = loader.GetWidth();
	numRows = loader.GetHeight();
	numLayers = loader.GetNumLayers();
	tiles = new Tile[numLayers * numCols * numRows];
	
	for(int z = 0; z < numLayers; z++)
	{
		loader.ChangeLayer(loader.GetLayersUsed(z));
		for(int x = 0; x < numCols; x++)
		{
			for(int y = 0; y < numRows; y++)
			{
				Tile *tile = GetTile(z, x, y);
				BLKSTR *block = loader.GetBlock(x, y);
				tile->texture = textureStartIndex+block->bg;
			}
		}
	}

	offset = -0.5f * Vector2(numCols * tileWidth, numRows * tileHeight);
	return 0;
}
