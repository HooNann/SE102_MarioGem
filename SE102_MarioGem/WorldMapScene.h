#pragma once
#include "Scene.h"
#include "MapNode.h"
#include "MapObject.h"
#include "MapMario.h"
#include "TileMap.h"
#include "Hud.h"
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

class CWorldMapScene : public CScene
{
protected:
	CMapMario* player;

	unordered_map<int, CMapNode*> nodes;
	vector<CMapObject*> mapObjects;
	
	CTileMap* map;

	CHud* hud;

	wstring currentAssetFilePath;
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_SPRITES_JSON(string line);
	void _ParseSection_ANIMATIONS_JSON(string line);
	void _ParseSection_ASSETS(string line);
	
	void LoadAssets(LPCWSTR assetFile);
	void LoadMapJSON(LPCWSTR jsonPath);

public:
	CWorldMapScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	CMapMario* GetPlayer() { return player; }
};

typedef CWorldMapScene* LPWORLDMAPSCENE;
