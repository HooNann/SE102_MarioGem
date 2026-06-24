#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scene.h"
#include "GameObject.h"
#include "Brick.h"
#include "Mario.h"
#include "Goomba.h"
//#include "Koopas.h"
#include "TileMap.h"


class CPlayScene: public CScene
{
protected:
	// A play scene has to have player, right?
	LPGAMEOBJECT player;

	vector<LPGAMEOBJECT> objects;
	vector<LPGAMEOBJECT> spawnQueue;

	CMap* map;	// Tiled Map (background tiles)

	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);

	void _ParseSection_ASSETS(string line);
	void _ParseSection_OBJECTS(string line);

	void LoadAssets(LPCWSTR assetFile);

	// Load map và objects từ file JSON của Tiled Map Editor
	void LoadMapJSON(LPCWSTR jsonPath);
	
public: 
	CPlayScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	LPGAMEOBJECT GetPlayer() { return player; }
	void AddObject(LPGAMEOBJECT obj) { objects.push_back(obj); }

	void QueueSpawn(LPGAMEOBJECT obj) { spawnQueue.push_back(obj); }

	void QueueSpawn(LPGAMEOBJECT obj) { spawnQueue.push_back(obj); }

	void Clear();
	void PurgeDeletedObjects();

	static bool IsGameObjectDeleted(const LPGAMEOBJECT& o);
};

typedef CPlayScene* LPPLAYSCENE;


