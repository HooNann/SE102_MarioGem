#pragma once
#include "Scene.h"
#include "MapNode.h"
#include "MapObject.h"
#include "MapMario.h"
#include "TileMap.h"
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
	
	CTileMap* map; // Hình nền (TileMap)

	void _ParseSection_ASSETS(string line);
	
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
