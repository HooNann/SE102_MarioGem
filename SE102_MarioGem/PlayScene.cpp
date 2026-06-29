#include "AssetIDs.h"
#include "json.hpp"

#include "Coin.h"
#include "Platform.h"
#include "PlayScene.h"
#include "Portal.h"
#include "Sprites.h"
#include "Textures.h"
#include "Utils.h"
#include "BoomBoom.h"


#include "PlaySceneKeyHandler.h"

#include "ObjectFactory.h"
#include <fstream>

using namespace std;

CPlayScene::CPlayScene(int id, LPCWSTR filePath):
	CScene(id, filePath)
{
	player = NULL;
	map = NULL;
	key_handler = new CPlaySceneKeyHandler(this);

	hud = NULL;
	timeRemaining = 300.0f;	
	hudWorld = "1-1";
}

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_ASSETS	1
#define SCENE_SECTION_OBJECTS	2
#define SCENE_SECTION_MAP		3

#define ASSETS_SECTION_UNKNOWN -1
#define ASSETS_SECTION_SPRITES 1
#define ASSETS_SECTION_ANIMATIONS 2
#define ASSETS_SECTION_SPRITES_JSON 3
#define ASSETS_SECTION_ANIMATIONS_JSON 4

#define MAX_SCENE_LINE 1024

void CPlayScene::_ParseSection_SPRITES(string line) {
  vector<string> tokens = split(line);

  if (tokens.size() < 6)
    return; // skip invalid lines

  int ID = atoi(tokens[0].c_str());
  int l = atoi(tokens[1].c_str());
  int t = atoi(tokens[2].c_str());
  int r = atoi(tokens[3].c_str());
  int b = atoi(tokens[4].c_str());
  int texID = atoi(tokens[5].c_str());

  LPTEXTURE tex = CTextures::GetInstance()->Get(texID);
  if (tex == NULL) {
    DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
    return;
  }

  CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void CPlayScene::_ParseSection_SPRITES_JSON(string line)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 2) return; // texID, json_path

	int texID = atoi(tokens[0].c_str());
	wstring path = ToWSTR(tokens[1]);

	LPTEXTURE tex = CTextures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return; 
	}

	ifstream f(path.c_str());
	if (!f.is_open())
	{
		DebugOut(L"[ERROR] Cannot open json file: %s\n", path.c_str());
		return;
	}

	nlohmann::json j;
	f >> j;
	f.close();

	for (auto& item : j["frames"].items()) 
	{
		int spriteId = stoi(item.key()); 
		int x = item.value()["frame"]["x"];
		int y = item.value()["frame"]["y"];
		int w = item.value()["frame"]["w"];
		int h = item.value()["frame"]["h"];

		int l = x;
		int t = y;
		int r = x + w - 1;
		int b = y + h - 1;

		CSprites::GetInstance()->Add(spriteId, l, t, r, b, tex);
	}
}

void CPlayScene::_ParseSection_ASSETS(string line)
{
	vector<string> tokens = split(line);

  if (tokens.size() < 1)
    return;

	wstring path = ToWSTR(tokens[0]);
	currentAssetFilePath = path;
	
	LoadAssets(path.c_str());
}

void CPlayScene::_ParseSection_ANIMATIONS(string line) {
  vector<string> tokens = split(line);

	if (tokens.size() < 3) return; 

  // DebugOut(L"--> %s\n",ToWSTR(line).c_str());

  LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i + 1 < (int)tokens.size(); i += 2)
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i+1].c_str());
		ani->Add(sprite_id, frame_time);
	}

  CAnimations::GetInstance()->Add(ani_id, ani);
}

void CPlayScene::_ParseSection_ANIMATIONS_JSON(string line)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 1) return;

	wstring path = ToWSTR(tokens[0]);
	ifstream f(path.c_str());
	if (!f.is_open())
	{
		DebugOut(L"[ERROR] Cannot open json file: %s\n", path.c_str());
		return;
	}

	nlohmann::json j;
	f >> j;
	f.close();

	for (auto& item : j["animations"].items())
	{
		int aniId = stoi(item.key());
		LPANIMATION ani = new CAnimation();

		if (item.value().is_array())
		{
			// Format 1: Mảng các object [{"sprite": 12001, "time": 100}, {"sprite": 12002, "time": 50}]
			for (auto& frame : item.value())
			{
				int spriteId = frame["sprite"];
				int frameTime = frame["time"];
				ani->Add(spriteId, frameTime);
			}
		}
		else if (item.value().is_object() && item.value().contains("frames"))
		{
			auto frames = item.value()["frames"];
			if (frames.size() > 0 && frames[0].is_object())
			{
				// Format 2: Object chứa frames là mảng object {"frames": [{"sprite": 12001, "time": 100}]}
				for (auto& frame : frames)
				{
					int spriteId = frame["sprite"];
					int frameTime = frame["time"];
					ani->Add(spriteId, frameTime);
				}
			}
			else
			{
				// Format 3: Cấu hình chung cho toàn bộ frame {"frames": [12001, 12002], "time": 100}
				int frameTime = item.value().value("time", 100);
				for (int spriteId : frames)
				{
					ani->Add(spriteId, frameTime);
				}
			}
		}

		CAnimations::GetInstance()->Add(aniId, ani);
	}
}

/*
        Parse a line in section [OBJECTS]
*/
void CPlayScene::_ParseSection_OBJECTS(string line) {
  vector<string> tokens = split(line);

  // skip invalid lines - an object specification has at least 3 tokens
  // (class/type, x, y)
  if (tokens.size() < 3)
    return;

  ObjectType type = static_cast<ObjectType>(atoi(tokens[0].c_str()));

  LPGAMEOBJECT obj = ObjectFactory::Create(type, tokens);

  if (obj == nullptr)
    return;

  if (type == ObjectType::Mario) {
    if (player != nullptr) {
      DebugOut(L"[ERROR] MARIO object was created before!\n");

      delete obj;
      return;
    }

    player = dynamic_cast<CMario *>(obj);

    DebugOut(L"[INFO] Player object has been created!\n");
  }

  objects.push_back(obj);
}

void CPlayScene::LoadAssets(LPCWSTR assetFile)
{
	DebugOut(L"[INFO] Start loading assets from : %s \n", assetFile);

	ifstream f;
	f.open(assetFile);

	int section = ASSETS_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line.empty()) continue;
		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[SPRITES]") { section = ASSETS_SECTION_SPRITES; continue; };
		if (line == "[ANIMATIONS]") { section = ASSETS_SECTION_ANIMATIONS; continue; };
		if (line == "[SPRITES_JSON]") { section = ASSETS_SECTION_SPRITES_JSON; continue; };
		if (line == "[ANIMATIONS_JSON]") { section = ASSETS_SECTION_ANIMATIONS_JSON; continue; };
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		//
		// data section
		//
		switch (section)
		{
		case ASSETS_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case ASSETS_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		case ASSETS_SECTION_SPRITES_JSON: _ParseSection_SPRITES_JSON(line); break;
		case ASSETS_SECTION_ANIMATIONS_JSON: _ParseSection_ANIMATIONS_JSON(line); break;
		}
	}

	f.close();

	DebugOut(L"[INFO] Done loading assets from %s\n", assetFile);
}

void CPlayScene::Load()
{
	DebugOut(L"[INFO] Start loading scene from : %s \n", sceneFilePath);

	ifstream f;
	f.open(sceneFilePath);

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;					

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line.empty()) continue;
		if (line[0] == '#') continue;	// skip comment lines	
		if (line == "[ASSETS]") { section = SCENE_SECTION_ASSETS; continue; };
		if (line == "[OBJECTS]") { section = SCENE_SECTION_OBJECTS; continue; };
		if (line == "[MAP]") { section = SCENE_SECTION_MAP; continue; };
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }	

		//
		// data section
		//
		switch (section)
		{ 
			case SCENE_SECTION_ASSETS: _ParseSection_ASSETS(line); break;
			case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
			case SCENE_SECTION_MAP:
			{
				// Dòng trong section [MAP] chứa đường dẫn tới file JSON của Tiled
				wstring mapPath = ToWSTR(line);
				LoadMapJSON(mapPath.c_str());
				break;
			}
		}
	}

	f.close();

	if (hud != NULL) delete hud;
	hud = new CHud();

	DebugOut(L"[INFO] Done loading scene  %s\n", sceneFilePath);
}


void CPlayScene::LoadMapJSON(LPCWSTR jsonPath)
{
	DebugOut(L"[INFO] Start loading Tiled map from: %s\n", jsonPath);

	wstring fullPath(jsonPath);
	wstring basePath = L".";
	size_t lastSlash = fullPath.find_last_of(L"\\/");
	if (lastSlash != wstring::npos)
		basePath = fullPath.substr(0, lastSlash);

	// Tạo và load CTileMap (tile layer)
	if (map != NULL) delete map;
	map = new CTileMap();
	map->LoadJSON(jsonPath, basePath.c_str());

	ifstream f(jsonPath);
	if (!f.is_open())
	{
		DebugOut(L"[ERROR] Cannot reopen map JSON for objects: %s\n", jsonPath);
		return;
	}

	nlohmann::json j;
	f >> j;
	f.close();

	for (auto& layer : j["layers"])
	{
		string layerType = layer["type"];
		if (layerType != "objectgroup") continue;

		string layerName = layer["name"];
		DebugOut(L"[INFO] Loading object layer: %s\n",
			wstring(layerName.begin(), layerName.end()).c_str());

		for (auto& obj : layer["objects"])
		{
			LPGAMEOBJECT gameObj = ObjectFactory::CreateFromJSON(obj);

			if (gameObj == nullptr) continue;

			CMario* mario = dynamic_cast<CMario*>(gameObj);
			if (mario != nullptr)
			{
				if (player != nullptr)
				{
					DebugOut(L"[ERROR] MARIO object was created before!\n");
					delete gameObj;
					continue;
				}
				player = mario;
				DebugOut(L"[INFO] Player object created from Tiled map!\n");
			}

			objects.push_back(gameObj);
		}
	}

	DebugOut(L"[INFO] Done loading Tiled map: %s\n", jsonPath);
}

void CPlayScene::Update(DWORD dt)
{
	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

  vector<LPGAMEOBJECT> coObjects;
  for (size_t i = 1; i < objects.size(); i++) {
    coObjects.push_back(objects[i]);
  }

  for (size_t i = 0; i < objects.size(); i++) {
    objects[i]->Update(dt, &coObjects);
  }

  // skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
  if (player == NULL)
    return;

  // Update camera to follow mario
  float cx, cy;
  player->GetPosition(cx, cy);

  CGame *game = CGame::GetInstance();
  cx -= game->GetBackBufferWidth() / 2;
  cy -= game->GetBackBufferHeight() / 2;

  if (cx < 0)
    cx = 0;

  // Giới hạn Camera không cho trôi qua bức tường cuối cùng
  // Bạn có thể thay đổi số 250.0f này thành toạ độ X của bức tường bên phải của bạn!
  float maxCamX = 250.0f; 
  if (cx > maxCamX)
    cx = maxCamX;

	for (auto obj : spawnQueue)
		objects.push_back(obj);
	spawnQueue.clear();

	timeRemaining -= dt / 1000.0f;
	if (timeRemaining < 0) timeRemaining = 0;

	PurgeDeletedObjects();
}

void CPlayScene::Render()
{
	if (map != NULL)
		map->Render();

	for (int i = 0; i < objects.size(); i++)
		objects[i]->Render();

	if (hud != NULL && player != NULL)
		hud->Render((CMario*)player, (int)timeRemaining, hudWorld.c_str());
}

/*
 *	Clear all objects from this scene
 */
void CPlayScene::Clear() {
  vector<LPGAMEOBJECT>::iterator it;
  for (it = objects.begin(); it != objects.end(); it++) {
    delete (*it);
  }
  objects.clear();
}

/*
        Unload scene

        TODO: Beside objects, we need to clean up sprites, animations and
   textures as well

*/
void CPlayScene::Unload() {
  Clear();
  player = NULL;

	objects.clear();

	// Free any objects queued to spawn but not yet added (e.g. Canon fires on same frame as scene switch)
	for (auto obj : spawnQueue) delete obj;
	spawnQueue.clear();

	player = NULL;

	if (map != NULL)
	{
		delete map;
		map = NULL;
	}

	if (hud != NULL)
	{
		delete hud;
		hud = NULL;
	}

	DebugOut(L"[INFO] Scene %d unloaded! \n", id);
}

void CPlayScene::PurgeDeletedObjects() {
  vector<LPGAMEOBJECT>::iterator it;
  for (it = objects.begin(); it != objects.end(); it++) {
    LPGAMEOBJECT o = *it;
    if (o->IsDeleted()) {
      delete o;
      *it = NULL;
    }
  }

  // NOTE: remove_if will swap all deleted items to the end of the vector
  // then simply trim the vector, this is much more efficient than deleting
  // individual items
  objects.erase(std::remove_if(objects.begin(), objects.end(),
                               CPlayScene::IsGameObjectDeleted),
                objects.end());
}

void CPlayScene::ReloadAssets()
{
	CAnimations::GetInstance()->Clear();
	CSprites::GetInstance()->Clear();
	if (!currentAssetFilePath.empty())
	{
		LoadAssets(currentAssetFilePath.c_str());
		DebugOut(L"[INFO] Assets reloaded successfully!\n");
	}
	else
	{
		DebugOut(L"[ERROR] No asset file path saved to reload!\n");
	}
}
