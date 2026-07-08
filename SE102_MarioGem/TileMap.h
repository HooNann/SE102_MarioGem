#pragma once

#include <vector>
#include <string>
#include "Texture.h"

using namespace std;

struct TilesetInfo
{
	int firstGid;
	int tileWidth;
	int tileHeight;
	int columns;
	int tileCount;
	int imageWidth;
	int imageHeight;
	wstring imagePath;
	LPTEXTURE texture;
};

struct TileLayerInfo
{
	string name;
	vector<unsigned int> data;
	bool isForeground;
};

class CTileMap
{
	int width;
	int height;
	int tileWidth;
	int tileHeight;

	vector<TileLayerInfo> mapLayers;
	vector<TilesetInfo> tilesets;

	void RenderLayers(bool foreground);

public:
	CTileMap();
	~CTileMap();

	void LoadJSON(LPCWSTR jsonPath, LPCWSTR basePath);

	void Render();
	void RenderBackground();
	void RenderForeground();

	int GetWidth() { return width; }
	int GetHeight() { return height; }
	int GetTileWidth() { return tileWidth; }
	int GetTileHeight() { return tileHeight; }

	TilesetInfo* GetTilesetByGid(int gid);
};
