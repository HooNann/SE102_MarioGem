#include "WorldMapScene.h"
#include <iostream>
#include <fstream>
#include "json.hpp"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Animations.h"
#include "WorldMapKeyHandler.h"
#include "ObjectFactory.h"
#include "Mario.h"
#include "Camera.h"
#include "Game.h"
#include "GameData.h"
#include "debug.h"
#include "SoundEvents.h"
#include "SoundSubject.h"

using namespace std;
using json = nlohmann::json;

CWorldMapScene::CWorldMapScene(int id, LPCWSTR filePath) : CScene(id, filePath)
{
	player = NULL;
	map = NULL;
	hud = NULL;
	key_handler = new CWorldMapKeyHandler(this);
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

void CWorldMapScene::_ParseSection_SPRITES(string line)
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

void CWorldMapScene::_ParseSection_SPRITES_JSON(string line)
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

void CWorldMapScene::_ParseSection_ASSETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 1) return;

	wstring path = ToWSTR(tokens[0]);
	currentAssetFilePath = path;
	
	LoadAssets(path.c_str());
}

void CWorldMapScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; 

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

void CWorldMapScene::_ParseSection_ANIMATIONS_JSON(string line)
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
				for (auto& frame : frames)
				{
					int spriteId = frame["sprite"];
					int frameTime = frame["time"];
					ani->Add(spriteId, frameTime);
				}
			}
			else
			{
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

void CWorldMapScene::LoadAssets(LPCWSTR assetFile)
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

void CWorldMapScene::Load()
{
	DebugOut(L"[INFO] Start loading WorldMapScene from : %s \n", sceneFilePath);

	CCamera::GetInstance()->SetSize(
		(float)CGame::GetInstance()->GetBackBufferWidth(),
		(float)CGame::GetInstance()->GetBackBufferHeight());

	ifstream f;
	f.open(sceneFilePath);

	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line.empty()) continue;
		if (line[0] == '#') continue;

		if (line == "[ASSETS]") { section = SCENE_SECTION_ASSETS; continue; };
		if (line == "[MAP]") { section = SCENE_SECTION_MAP; continue; };
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }	

		//
		// data section
		//
		switch (section)
		{ 
			case SCENE_SECTION_ASSETS: _ParseSection_ASSETS(line); break;
			case SCENE_SECTION_MAP:
			{
				if (line.find(".json") != string::npos)
				{
					wstring mapPath = ToWSTR(line);
					LoadMapJSON(mapPath.c_str());
				}
				break;
			}
		}
	}

	f.close();

	if (hud != NULL) delete hud;
	hud = new CHud();

	CSoundSubject::GetInstance()->Notify(EVENT_MUSIC_WORLDMAP);

	DebugOut(L"[INFO] Done loading WorldMapScene %s\n", sceneFilePath);
}

void CWorldMapScene::LoadMapJSON(LPCWSTR jsonPath)
{
	DebugOut(L"[INFO] Start loading Tiled map for WorldMap: %s\n", jsonPath);

	wstring fullPath(jsonPath);
	wstring basePath = L".";
	size_t lastSlash = fullPath.find_last_of(L"\\/");
	if (lastSlash != wstring::npos)
		basePath = fullPath.substr(0, lastSlash);

	if (map != NULL) delete map;
	map = new CTileMap();
	map->LoadJSON(jsonPath, basePath.c_str());

	ifstream f(jsonPath);
	if (!f.is_open()) return;

	json j;
	f >> j;
	f.close();

	// Bước 1: Sinh ra tất cả Node và Object
	for (auto& layer : j["layers"])
	{
		string layerType = layer["type"];
		if (layerType != "objectgroup") continue;

		for (auto& obj : layer["objects"])
		{
			string typeStr = "";
			if (obj.contains("type") && obj["type"].is_string()) typeStr = obj["type"].get<string>();
			else if (obj.contains("class") && obj["class"].is_string()) typeStr = obj["class"].get<string>();

			float w = obj.value("width", 0.0f);
			float h = obj.value("height", 0.0f);

			vector<nlohmann::json> objectsToProcess;

			if ((typeStr == "MapNode" || typeStr == "MapDecoration" || typeStr == "MapMario") && w > 0 && h > 0)
			{
				float startX = obj.value("x", 0.0f);
				float startY = obj.value("y", 0.0f);

				int cols = round(w / 16.0f);
				int rows = round(h / 16.0f);
				if (cols < 1) cols = 1;
				if (rows < 1) rows = 1;

				for (int r = 0; r < rows; ++r) {
					for (int c = 0; c < cols; ++c) {
						nlohmann::json singleObj = obj;
						singleObj["x"] = startX + c * 16.0f + 8.0f;
						singleObj["y"] = startY + r * 16.0f + 8.0f;
						singleObj["width"] = 0.0f; // Set to 0 so ObjectFactory processes it as a Point
						singleObj["height"] = 0.0f;
						singleObj["cell_bottom"] = startY + (r + 1) * 16.0f;
						objectsToProcess.push_back(singleObj);
					}
				}
			}
			else
			{
				objectsToProcess.push_back(obj);
			}

			for (auto& singleObj : objectsToProcess)
			{
				LPGAMEOBJECT gameObj = ObjectFactory::CreateFromJSON(singleObj);
				if (gameObj == nullptr) continue;

				if (singleObj.contains("cell_bottom"))
				{
					float l, t, r_box, b;
					gameObj->GetBoundingBox(l, t, r_box, b);
					float h_box = b - t;
					if (h_box > 0)
					{
						float cx, cy;
						gameObj->GetPosition(cx, cy);
						float cell_bottom = singleObj["cell_bottom"];
						gameObj->SetPosition(cx, cell_bottom - h_box / 2.0f);
					}
				}

				CMapNode* node = dynamic_cast<CMapNode*>(gameObj);
				if (node != nullptr)
				{
					nodes[node->nodeId] = node;
					
					// Đọc custom properties
					if (singleObj.contains("properties"))
					{
						for (auto& prop : singleObj["properties"])
						{
							string name = prop["name"];
							if (name == "up_id") node->up_id = prop["value"];
							else if (name == "down_id") node->down_id = prop["value"];
							else if (name == "left_id") node->left_id = prop["value"];
							else if (name == "right_id") node->right_id = prop["value"];
							else if (name == "scene_id") node->sceneId = prop["value"];
						}
					}
				}

				CMapMario* mario = dynamic_cast<CMapMario*>(gameObj);
				if (mario != nullptr)
				{
					player = mario;
				}
				else
				{
					mapObjects.push_back(dynamic_cast<CMapObject*>(gameObj));
				}
			}
		}
	}

	// Bước 2: Nối con trỏ cho các Node
	for (auto& pair : nodes)
	{
		CMapNode* node = pair.second;
		if (node->up_id != -1 && nodes.count(node->up_id)) node->upNode = nodes[node->up_id];
		if (node->down_id != -1 && nodes.count(node->down_id)) node->downNode = nodes[node->down_id];
		if (node->left_id != -1 && nodes.count(node->left_id)) node->leftNode = nodes[node->left_id];
		if (node->right_id != -1 && nodes.count(node->right_id)) node->rightNode = nodes[node->right_id];
	}

	// Gắn Mario vào Node bắt đầu (giả sử node gần nhất hoặc node có id chỉ định)
	if (player != nullptr && !nodes.empty())
	{
		int gameDataNodeId = CGameData::GetInstance()->GetCurrentNodeId();
		
		if (gameDataNodeId != 0 && nodes.count(gameDataNodeId) > 0)
		{
			player->currentNode = nodes[gameDataNodeId];
		}
		else if (player->start_node_id != -1 && nodes.count(player->start_node_id) > 0)
		{
			player->currentNode = nodes[player->start_node_id];
		}
		else
		{
			// Tạm thời gán node đầu tiên tìm thấy nếu không tìm thấy node chỉ định
			player->currentNode = nodes.begin()->second; 
		}
		
		float nx, ny;
		player->currentNode->GetPosition(nx, ny);
		
		float nl, nt, nr, nb;
		player->currentNode->GetBoundingBox(nl, nt, nr, nb);
		
		float ml, mt, mr, mb;
		player->GetBoundingBox(ml, mt, mr, mb);
		float mario_h = mb - mt;

		if (mario_h > 0) {
			ny = nb - mario_h / 2.0f;
		}

		player->SetPosition(nx, ny);
	}
}

void CWorldMapScene::Update(DWORD dt)
{
	float cx = 0;
	float cy = 0.0f; 

	if (map != NULL)
	{
		float map_width = (float)(map->GetWidth() * map->GetTileWidth());
		float map_height = (float)(map->GetHeight() * map->GetTileHeight());
		float screen_width = (float)CGame::GetInstance()->GetBackBufferWidth();
		float screen_height = (float)CGame::GetInstance()->GetBackBufferHeight();
		cx = (map_width - screen_width) / 2.0f;
		cy = map_height - screen_height;

		CCamera::GetInstance()->SetCamPos(cx, cy);
	}

	if (player) player->Update(dt);
	for (auto obj : mapObjects)
	{
		if (obj) obj->Update(dt);
	}
}

void CWorldMapScene::Render()
{
	if (map != NULL) map->Render();
	
	// Không render các Node trừ khi debug
	for (auto obj : mapObjects)
	{
		if (obj) obj->Render();
	}

	if (player) player->Render();

	if (hud != NULL) hud->RenderWorldMap("1");
}

void CWorldMapScene::Unload()
{
	if (player) { delete player; player = NULL; }
	for (auto obj : mapObjects) { if (obj) delete obj; }
	mapObjects.clear();
	nodes.clear(); // Các CMapNode đã được đẩy vào mapObjects rồi nên sẽ bị delete trong vòng lặp trên
	
	if (map != NULL) { delete map; map = NULL; }

	if (hud != NULL) { delete hud; hud = NULL; }
}
