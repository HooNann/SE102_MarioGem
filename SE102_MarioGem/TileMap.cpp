#include "TileMap.h"
#include "Game.h"
#include "Textures.h"
#include "debug.h"
#include "Camera.h"
#include "json.hpp"

#include <fstream>
#include <algorithm>

using json = nlohmann::json;

CTileMap::CTileMap()
{
	width = 0;
	height = 0;
	tileWidth = 0;
	tileHeight = 0;
}

CTileMap::~CTileMap()
{
	mapLayers.clear();
	tilesets.clear();
}

void CTileMap::LoadJSON(LPCWSTR jsonPath, LPCWSTR basePath)
{
	ifstream f(jsonPath);
	if (!f.is_open())
	{
		DebugOut(L"[ERROR] Cannot open map JSON file: %s\n", jsonPath);
		return;
	}

	json j;
	f >> j;
	f.close();

	width = j["width"];
	height = j["height"];
	tileWidth = j["tilewidth"];
	tileHeight = j["tileheight"];

	DebugOut(L"[INFO] Map size: %d x %d tiles, tile size: %d x %d\n",
		width, height, tileWidth, tileHeight);


	for (auto& ts : j["tilesets"])
	{
		TilesetInfo info;
		info.firstGid = ts["firstgid"];

		if (ts.contains("image"))
		{
			info.columns = ts["columns"];
			info.tileCount = ts["tilecount"];
			info.tileWidth = ts["tilewidth"];
			info.tileHeight = ts["tileheight"];
			info.imageWidth = ts["imagewidth"];
			info.imageHeight = ts["imageheight"];

			string imgPath = ts["image"];
			wstring wImgPath(imgPath.begin(), imgPath.end());

			wstring fullPath = wstring(basePath) + L"\\" + wImgPath;
			info.imagePath = fullPath;

			info.texture = CGame::GetInstance()->LoadTexture(fullPath.c_str());

			if (info.texture == NULL)
			{
				DebugOut(L"[ERROR] Failed to load tileset texture: %s\n", fullPath.c_str());
			}
			else
			{
				DebugOut(L"[INFO] Loaded tileset: firstGid=%d, columns=%d, image=%s\n",
					info.firstGid, info.columns, fullPath.c_str());
			}
		}
		else if (ts.contains("source"))
		{
			string srcPath = ts["source"];

			wstring wSrcPath(srcPath.begin(), srcPath.end());
			wstring fullTsPath = wstring(basePath) + L"\\" + wSrcPath;

			ifstream tsFile(fullTsPath);
			if (!tsFile.is_open())
			{
				DebugOut(L"[WARNING] Cannot open external tileset: %s. "
					L"Please use 'Embed in Map' option in Tiled or export tileset as JSON (.tsj)\n",
					fullTsPath.c_str());
				continue;
			}

			json tsJson;
			tsFile >> tsJson;
			tsFile.close();

			info.columns = tsJson["columns"];
			info.tileCount = tsJson["tilecount"];
			info.tileWidth = tsJson["tilewidth"];
			info.tileHeight = tsJson["tileheight"];
			info.imageWidth = tsJson["imagewidth"];
			info.imageHeight = tsJson["imageheight"];

			string imgPath = tsJson["image"];
			wstring wImgPath(imgPath.begin(), imgPath.end());

			wstring fullImgPath = wstring(basePath) + L"\\" + wImgPath;
			info.imagePath = fullImgPath;

			info.texture = CGame::GetInstance()->LoadTexture(fullImgPath.c_str());

			if (info.texture == NULL)
			{
				DebugOut(L"[ERROR] Failed to load tileset texture: %s\n", fullImgPath.c_str());
			}
			else
			{
				DebugOut(L"[INFO] Loaded external tileset: firstGid=%d, columns=%d, image=%s\n",
					info.firstGid, info.columns, fullImgPath.c_str());
			}
		}

		tilesets.push_back(info);
	}

	sort(tilesets.begin(), tilesets.end(),
		[](const TilesetInfo& a, const TilesetInfo& b) {
			return a.firstGid < b.firstGid;
		});


	mapLayers.clear();
	for (auto& layer : j["layers"])
	{
		string layerType = layer["type"];

		if (layerType == "tilelayer")
		{
			string layerName = layer["name"];
			DebugOut(L"[INFO] Loading tile layer: %s\n",
				wstring(layerName.begin(), layerName.end()).c_str());

			TileLayerInfo layerInfo;
			layerInfo.name = layerName;
			layerInfo.isForeground = (layerName == "Foreground");

			for (auto& id : layer["data"])
			{
				layerInfo.data.push_back(id.get<unsigned int>());
			}

			mapLayers.push_back(layerInfo);
			DebugOut(L"[INFO] Tile layer loaded: %d tiles\n", (int)layerInfo.data.size());
		}
	}
}

TilesetInfo* CTileMap::GetTilesetByGid(int gid)
{
	for (int i = (int)tilesets.size() - 1; i >= 0; i--)
	{
		if (gid >= tilesets[i].firstGid)
			return &tilesets[i];
	}
	return nullptr;
}

void CTileMap::RenderLayers(bool foreground)
{
	if (mapLayers.empty() || tilesets.empty()) return;

	CCamera* camera = CCamera::GetInstance();
	float camX, camY;
	camera->GetCamPos(camX, camY);

	float cx = (float)floor(camX);
	float cy = (float)floor(camY);

	int screenWidth = (int)camera->GetWidth();
	int screenHeight = (int)camera->GetHeight();

	int colStart = max(0, (int)(cx / tileWidth));
	int colEnd = min(width - 1, (int)((cx + screenWidth) / tileWidth));
	int rowStart = max(0, (int)(cy / tileHeight));
	int rowEnd = min(height - 1, (int)((cy + screenHeight) / tileHeight));

	for (size_t i = 0; i < mapLayers.size(); i++)
	{
		if (mapLayers[i].isForeground != foreground) continue;

		for (int row = rowStart; row <= rowEnd; row++)
		{
			for (int col = colStart; col <= colEnd; col++)
			{
				int index = row * width + col;
				unsigned int tileData = mapLayers[i].data[index];

				if (tileData == 0) continue;

				bool flipX = (tileData & 0x80000000);
				bool flipY = (tileData & 0x40000000);
				
				int tileId = tileData & ~(0x80000000 | 0x40000000 | 0x20000000);

				TilesetInfo* ts = GetTilesetByGid(tileId);
				if (ts == nullptr || ts->texture == NULL) continue;

				int localId = tileId - ts->firstGid;

				int tsCol = localId % ts->columns;
				int tsRow = localId / ts->columns;

				RECT srcRect;
				srcRect.left = tsCol * ts->tileWidth;
				srcRect.top = tsRow * ts->tileHeight;
				srcRect.right = srcRect.left + ts->tileWidth - 1;
				srcRect.bottom = srcRect.top + ts->tileHeight - 1;

				float drawX = (float)floor((col * tileWidth) + tileWidth / 2.0f - cx);
				float drawY = (float)floor((row * tileHeight) + tileHeight / 2.0f - cy);

				int nx = flipX ? -1 : 1;
				int ny = flipY ? -1 : 1;

				CGame::GetInstance()->Draw(drawX, drawY, ts->texture, &srcRect, 1.0f, 0, 0, nx, ny);
			}
		}
	}
}

void CTileMap::Render()
{
	RenderBackground();
	RenderForeground();
}

void CTileMap::RenderBackground()
{
	RenderLayers(false);
}

void CTileMap::RenderForeground()
{
	RenderLayers(true);
}

