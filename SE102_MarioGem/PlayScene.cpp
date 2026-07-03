#include <iostream>
#include <fstream>
#include "AssetIDs.h"
#include "json.hpp"

#include "PlayScene.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Portal.h"
#include "Pipe.h"
#include "Coin.h"
#include "Platform.h"
#include "CMarioState.h"
#include "CMarioPitDeadState.h"

#include "PlaySceneKeyHandler.h"

#include "ObjectFactory.h"
#include "Camera.h"
#include "SoundEvents.h"
#include "SoundSubject.h"
#include "EventManager.h"

using namespace std;

namespace
{
	constexpr float HUD_RESERVED_HEIGHT = 32.0f;
	constexpr int WORLD_MAP_SCENE_ID = 100;
	constexpr ULONGLONG COURSE_CLEAR_DURATION_MS = 4000;
	constexpr ULONGLONG DEATH_RETURN_DELAY_MS = 2000;

	void ConfigurePlaySceneCamera()
	{
		CGame* game = CGame::GetInstance();
		float cameraHeight = (float)game->GetBackBufferHeight() - HUD_RESERVED_HEIGHT;
		if (cameraHeight < 1.0f)
			cameraHeight = (float)game->GetBackBufferHeight();

		CCamera::GetInstance()->SetSize((float)game->GetBackBufferWidth(), cameraHeight);
	}

	bool IsPointInRect(float x, float y, float l, float t, float r, float b)
	{
		return x >= l && x <= r && y >= t && y <= b;
	}
}

CPlayScene::CPlayScene(int id, LPCWSTR filePath):
	CScene(id, filePath)
{
	player = NULL;
	map = NULL;
	key_handler = new CPlaySceneKeyHandler(this);

	hud = NULL;
	timeRemaining = 300.0f;	
	hudWorld = "1";

	map_width = 0.0f;
	map_height = 0.0f;
	activeCameraZoneIndex = -1;

	isCourseClear = false;
	courseClearStartTime = 0;
	courseClearReward = 0;
	isDeathTransitioning = false;
	isDeathResolved = false;
	deathStartTime = 0;

	isCameraBlockingLeftEdge = true;
	isCameraBlockingRightEdge = false;
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

void CPlayScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return; // skip invalid lines

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPTEXTURE tex = CTextures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
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

	if (tokens.size() < 1) return;

	wstring path = ToWSTR(tokens[0]);
	assetFilePaths.push_back(path);
	
	LoadAssets(path.c_str());
}

void CPlayScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; 

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

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
void CPlayScene::_ParseSection_OBJECTS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3)
		return;

	ObjectType type =
		static_cast<ObjectType>(
			atoi(tokens[0].c_str()));

	LPGAMEOBJECT obj =
		ObjectFactory::Create(type, tokens);

	if (obj == nullptr)
		return;

	if (type == ObjectType::Mario)
	{
		if (player != nullptr)
		{
			DebugOut(
				L"[ERROR] MARIO object was created before!\n");

			delete obj;
			return;
		}

		player = dynamic_cast<CMario*>(obj);

		DebugOut(
			L"[INFO] Player object has been created!\n");
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

	isCourseClear = false;
	courseClearStartTime = 0;
	courseClearReward = 0;
	isDeathTransitioning = false;
	isDeathResolved = false;
	deathStartTime = 0;
	activeCameraZoneIndex = -1;

	// Reset camera bounds to 0,0 in case this scene doesn't have a map
	ConfigurePlaySceneCamera();
	CCamera::GetInstance()->SetCameraBounds(0.0f, 0.0f, 0.0f, 0.0f);

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

	if (id == 6)
		CSoundSubject::GetInstance()->Notify(EVENT_MUSIC_FORTRESS);
	else
		CSoundSubject::GetInstance()->Notify(EVENT_MUSIC_OVERWORLD);

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

	map_width = (float)map->GetWidth() * map->GetTileWidth();
	map_height = (float)map->GetHeight() * map->GetTileHeight();
	
	cameraZones.clear();
	activeCameraZoneIndex = -1;
	deadZones.clear();
	CCamera::GetInstance()->SetCameraBounds(0, 0, map_width, map_height);

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
			// Tự động chia hình chữ nhật lớn thành nhiều block nhỏ (QuestionBlock, Brick, Coin)
			string typeStr = "";
			if (obj.contains("type") && obj["type"].is_string()) typeStr = obj["type"].get<string>();
			else if (obj.contains("class") && obj["class"].is_string()) typeStr = obj["class"].get<string>();

			float w = obj.value("width", 0.0f);
			float h = obj.value("height", 0.0f);

			if (typeStr == "CameraZone") {
				CameraZone z;
				z.l = obj.value("x", 0.0f);
				z.t = obj.value("y", 0.0f);
				z.r = z.l + w;
				z.b = z.t + h;
				cameraZones.push_back(z);
				continue; // Không tạo thành GameObject
			}

			if (typeStr == "DeadZone" || typeStr == "DeathZone") {
				DeadZone z;
				z.l = obj.value("x", 0.0f);
				z.t = obj.value("y", 0.0f);
				z.r = z.l + w;
				z.b = z.t + h;
				deadZones.push_back(z);
				continue; // Không tạo thành GameObject
			}

			if ((typeStr == "QuestionBlock" || typeStr == "Brick" || typeStr == "Coin") && w > 0 && h > 0) 
			{
				float startX = obj.value("x", 0.0f);
				float startY = obj.value("y", 0.0f);

				int cols = static_cast<int>(round(w / 16.0f));
				int rows = static_cast<int>(round(h / 16.0f));
				if (cols < 1) cols = 1;
				if (rows < 1) rows = 1;

				for (int r = 0; r < rows; ++r) {
					for (int c = 0; c < cols; ++c) {
						nlohmann::json singleObj = obj;
						// Tiled xuất tọa độ x,y là góc trái trên của hình chữ nhật
						// Ta cộng 8 pixel để dịch tọa độ vào chính giữa tâm khối 16x16
						singleObj["x"] = startX + c * 16.0f + 8.0f;
						singleObj["y"] = startY + r * 16.0f + 8.0f;
						singleObj["width"] = 16.0f;
						singleObj["height"] = 16.0f;
						
						LPGAMEOBJECT gameObj = ObjectFactory::CreateFromJSON(singleObj);
						if (gameObj) {
							float l, t, r_box, b;
							gameObj->GetBoundingBox(l, t, r_box, b);
							float h_box = b - t;
							if (h_box > 0) {
								float cx, cy;
								gameObj->GetPosition(cx, cy);
								float bottomOfCell = startY + (r + 1) * 16.0f;
								gameObj->SetPosition(cx, bottomOfCell - h_box / 2.0f);
							}
							objects.push_back(gameObj);
						}
					}
				}
				continue; // Đã xử lý xong nguyên mảng khối, bỏ qua việc khởi tạo object đơn
			}

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

bool CPlayScene::IsGameObjectInRegion(LPGAMEOBJECT obj, float r_left, float r_top, float r_right, float r_bottom)
{
	float l, t, r, b;
	obj->GetBoundingBox(l, t, r, b);

	// Some objects (like UI or special markers) might return 0 for all bounds. If so, fallback to position check.
	if (l == 0 && t == 0 && r == 0 && b == 0)
	{
		float ox, oy;
		obj->GetPosition(ox, oy);
		return (ox >= r_left && ox <= r_right && oy >= r_top && oy <= r_bottom);
	}

	return !(r < r_left || l > r_right || b < r_top || t > r_bottom);
}

CPipe* CPlayScene::GetOverlappingPipe(CMario* mario, PipeDirection entryDirection)
{
	if (mario == nullptr) return nullptr;
	if (entryDirection == PipeDirection::Down && !mario->IsOnPlatform()) return nullptr;

	float ml, mt, mr, mb;
	mario->GetBoundingBox(ml, mt, mr, mb);

	for (auto obj : objects)
	{
		CPipe* pipe = dynamic_cast<CPipe*>(obj);
		if (pipe == nullptr || pipe->IsDeleted()) continue;
		if (pipe->GetEntryDirection() != entryDirection) continue;

		float pl, pt, pr, pb;
		pipe->GetBoundingBox(pl, pt, pr, pb);

		bool overlap = !(mr < pl || ml > pr || mb < pt || mt > pb);
		if (overlap) return pipe;
	}

	return nullptr;
}

void CPlayScene::Update(DWORD dt)
{
	CEventManager::GetInstance()->Update(dt);
	ConfigurePlaySceneCamera();

	CCamera* camera = CCamera::GetInstance();
	float cx, cy;
	camera->GetCamPos(cx, cy);
	float screenWidth = camera->GetWidth();
	float screenHeight = camera->GetHeight();

	vector<LPGAMEOBJECT> activeObjects;
	float update_margin = 160.0f;
	float active_left = cx - update_margin;
	float active_top = cy - update_margin;
	float active_right = cx + screenWidth + update_margin;
	float active_bottom = cy + screenHeight + update_margin;

	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i] == player || IsGameObjectInRegion(objects[i], active_left, active_top, active_right, active_bottom))
		{
			activeObjects.push_back(objects[i]);
		}
	}

	vector<LPGAMEOBJECT> coObjects;
	for (size_t i = 0; i < activeObjects.size(); i++)
	{
		if (activeObjects[i] != player && !activeObjects[i]->IsDeleted())
			coObjects.push_back(activeObjects[i]);
	}

	for (size_t i = 0; i < activeObjects.size(); i++)
	{
		activeObjects[i]->Update(dt, &coObjects);
	}

	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return; 

	CMario* mario = dynamic_cast<CMario*>(player);
	if (!isCourseClear && mario != NULL && mario->currentState != NULL && mario->currentState->GetID() != MarioStateID::Dead)
	{
		float ml, mt, mr, mb;
		mario->GetBoundingBox(ml, mt, mr, mb);
		float footX = (ml + mr) / 2.0f;
		float footY = mb;

		for (auto& z : deadZones)
		{
			if (IsPointInRect(footX, footY, z.l, z.t, z.r, z.b))
			{
				mario->ChangeState(new CMarioPitDeadState());
				break;
			}
		}
	}

	if (!isCourseClear && mario != NULL && mario->currentState != NULL && mario->currentState->GetID() == MarioStateID::Dead)
	{
		if (!isDeathTransitioning)
		{
			isDeathTransitioning = true;
			deathStartTime = GetTickCount64();
			CSoundSubject::GetInstance()->Notify(EVENT_MUSIC_STOP);
			CSoundSubject::GetInstance()->Notify(EVENT_PLAYER_DOWN);
		}

		if (!isDeathResolved && GetTickCount64() - deathStartTime > DEATH_RETURN_DELAY_MS)
		{
			isDeathResolved = true;
			CGameData::GetInstance()->AddLife(-1);
			CGame::GetInstance()->InitiateSwitchScene(WORLD_MAP_SCENE_ID);
		}
	}

	// Update active camera zone, but keep the last zone when Mario leaves it.
	float px, py;
	player->GetPosition(px, py);
	
	for (int i = 0; i < (int)cameraZones.size(); i++) {
		CameraZone& z = cameraZones[i];
		if (px >= z.l && px <= z.r && py >= z.t && py <= z.b) {
			activeCameraZoneIndex = i;
			break;
		}
	}
	
	if (activeCameraZoneIndex >= 0 && activeCameraZoneIndex < (int)cameraZones.size()) {
		CameraZone& z = cameraZones[activeCameraZoneIndex];
		camera->SetCameraBounds(z.l, z.t, z.r, z.b);
	}
	else {
		camera->SetCameraBounds(0, 0, map_width, map_height);
	}

	// Update camera to follow mario
	if (!isCourseClear)
	{
		camera->SetTarget(player);
		camera->Update();
	}

	for (auto obj : spawnQueue)
		objects.push_back(obj);
	spawnQueue.clear();

	timeRemaining -= dt / 1000.0f;
	if (timeRemaining < 0) timeRemaining = 0;

	if (isCourseClear)
	{
		// Force Mario to keep walking right
		if (player != NULL)
		{
			CMario* m = dynamic_cast<CMario*>(player);
			if (m != NULL)
			{
				m->SetDirection(1);
				m->SetAccelerationX(0.0005f); 
			}
		}

		if (GetTickCount64() - courseClearStartTime > COURSE_CLEAR_DURATION_MS)
		{
			CGameData::GetInstance()->MarkSceneCleared(id);
			CGame::GetInstance()->InitiateSwitchScene(WORLD_MAP_SCENE_ID);
		}
	}

	PurgeDeletedObjects();
}

void CPlayScene::Render()
{
	CGame* game = CGame::GetInstance();
	game->BeginViewportClip((int)HUD_RESERVED_HEIGHT);

	if (map != NULL)
		map->RenderBackground();

	CCamera* camera = CCamera::GetInstance();
	float cx, cy;
	camera->GetCamPos(cx, cy);
	float screenWidth = camera->GetWidth();
	float screenHeight = camera->GetHeight();

	float render_margin = 48.0f;
	float render_left = cx - render_margin;
	float render_top = cy - render_margin;
	float render_right = cx + screenWidth + render_margin;
	float render_bottom = cy + screenHeight + render_margin;

	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i] == player || IsGameObjectInRegion(objects[i], render_left, render_top, render_right, render_bottom))
		{
			objects[i]->Render();
		}
	}

	if (map != NULL)
		map->RenderForeground();

	game->EndViewportClip();

	if (hud != NULL)
		hud->Render((CMario*)player, (int)timeRemaining, hudWorld.c_str());

	if (isCourseClear && hud != NULL)
	{
		hud->RenderCourseClear(courseClearReward);
	}
}

void CPlayScene::TriggerCourseClear(int reward)
{
	if (!isCourseClear)
	{
		isCourseClear = true;
		courseClearStartTime = GetTickCount64();
		courseClearReward = reward;
		CSoundSubject::GetInstance()->Notify(EVENT_MUSIC_STOP);
		CSoundSubject::GetInstance()->Notify(EVENT_COURSE_CLEAR);
		
		// Change Mario state to Walk right
		if (player != NULL)
		{
			CMario* m = dynamic_cast<CMario*>(player);
			if (m != NULL)
			{
				m->SetDirection(1);
				m->SetAccelerationX(0.0005f); // MARIO_ACCEL_WALK_X
			}
		}
	}
}

/*
*	Clear all objects from this scene
*/
void CPlayScene::Clear()
{
	vector<LPGAMEOBJECT>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		delete (*it);
	}
	objects.clear();
}

/*
	Unload scene

	TODO: Beside objects, we need to clean up sprites, animations and textures as well 

*/
void CPlayScene::Unload()
{
	CEventManager::GetInstance()->Clear();

	for (int i = 0; i < objects.size(); i++)
		delete objects[i];

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

	assetFilePaths.clear();

	DebugOut(L"[INFO] Scene %d unloaded! \n", id);
}

bool CPlayScene::IsGameObjectDeleted(const LPGAMEOBJECT& o) { return o == NULL; }

void CPlayScene::PurgeDeletedObjects()
{
	vector<LPGAMEOBJECT>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		LPGAMEOBJECT o = *it;
		if (o->IsDeleted())
		{
			delete o;
			*it = NULL;
		}
	}

	// NOTE: remove_if will swap all deleted items to the end of the vector
	// then simply trim the vector, this is much more efficient than deleting individual items
	objects.erase(
		std::remove_if(objects.begin(), objects.end(), CPlayScene::IsGameObjectDeleted),
		objects.end());
}

void CPlayScene::ReloadAssets()
{
	CAnimations::GetInstance()->Clear();
	CSprites::GetInstance()->Clear();
	if (assetFilePaths.size() > 0)
	{
		for (auto path : assetFilePaths)
		{
			LoadAssets(path.c_str());
		}
		DebugOut(L"[INFO] Assets reloaded successfully!\n");
	}
	else
	{
		DebugOut(L"[ERROR] No asset file path saved to reload!\n");
	}
}
