#pragma once
#include "Brick.h"
#include "Game.h"
#include "GameObject.h"
#include "Goomba.h"
#include "Mario.h"
#include "Scene.h"
#include "Textures.h"

// #include "Koopas.h"

class CPlayScene : public CScene {
protected:
  // A play scene has to have player, right?
  LPGAMEOBJECT player;


  vector<LPGAMEOBJECT> objects;

  void _ParseSection_SPRITES(string line);
  void _ParseSection_ANIMATIONS(string line);

  void _ParseSection_ASSETS(string line);
  void _ParseSection_OBJECTS(string line);
  void _ParseSection_MAP(string line);

  void LoadAssets(LPCWSTR assetFile);

public:
  CPlayScene(int id, LPCWSTR filePath);

  virtual void Load();
  virtual void Update(DWORD dt);
  virtual void Render();
  virtual void Unload();

  LPGAMEOBJECT GetPlayer() { return player; }

  void Clear();
  void PurgeDeletedObjects();

  static bool IsGameObjectDeleted(const LPGAMEOBJECT &o);
};

typedef CPlayScene *LPPLAYSCENE;
