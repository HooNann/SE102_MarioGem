#pragma once
#include "Brick.h"
#include <string>
#include "Game.h"
#include "GameObject.h"
#include "Goomba.h"
#include "Mario.h"
#include "Scene.h"
#include "Textures.h"
//#include "Koopas.h"
#include "TileMap.h"
#include "Hud.h"

// #include "Koopas.h"

class CPlayScene: public CScene
{
protected:
	// A play scene has to have player, right?
	LPGAMEOBJECT player;

	vector<LPGAMEOBJECT> objects;
	vector<LPGAMEOBJECT> spawnQueue;

	CTileMap* map;	// Tiled Map (background tiles)

	CHud* hud;
	float timeRemaining;
	std::string hudWorld;

  vector<LPGAMEOBJECT> objects;

	void _ParseSection_SPRITES_JSON(string line);
	void _ParseSection_ANIMATIONS_JSON(string line);

	wstring currentAssetFilePath;

	void _ParseSection_ASSETS(string line);
	void _ParseSection_OBJECTS(string line);

	void LoadAssets(LPCWSTR assetFile);

	// Load map và objects từ file JSON của Tiled Map Editor
	void LoadMapJSON(LPCWSTR jsonPath);
	
public: 
	CPlayScene(int id, LPCWSTR filePath);

  void LoadAssets(LPCWSTR assetFile);

public:
  CPlayScene(int id, LPCWSTR filePath);

  virtual void Load();
  virtual void Update(DWORD dt);
  virtual void Render();
  virtual void Unload();
	LPGAMEOBJECT GetPlayer() { return player; }
	void AddObject(LPGAMEOBJECT obj) { objects.push_back(obj); }

	void QueueSpawn(LPGAMEOBJECT obj) { spawnQueue.push_back(obj); }

	void Clear();
	void PurgeDeletedObjects();
	void ReloadAssets();

  LPGAMEOBJECT GetPlayer() { return player; }

  void Clear();
  void PurgeDeletedObjects();

  static bool IsGameObjectDeleted(const LPGAMEOBJECT &o);
};

typedef CPlayScene *LPPLAYSCENE;

