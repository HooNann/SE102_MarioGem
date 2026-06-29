#include "WorldMapScene.h"
#include <iostream>
#include <fstream>
#include "json.hpp"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "WorldMapKeyHandler.h"
#include "ObjectFactory.h"
#include "Game.h"
#include "debug.h"

using namespace std;
using json = nlohmann::json;

CWorldMapScene::CWorldMapScene(int id, LPCWSTR filePath) : CScene(id, filePath)
{
	player = NULL;
	map = NULL;
	key_handler = new CWorldMapKeyHandler(this);
}

void CWorldMapScene::Load()
{
	DebugOut(L"[INFO] Start loading WorldMapScene from : %s \n", sceneFilePath);

	ifstream f;
	f.open(sceneFilePath);

	char str[1024];
	while (f.getline(str, 1024))
	{
		string line(str);

		if (line.empty()) continue;
		if (line[0] == '#') continue;

		if (line.rfind("[ASSETS]", 0) == 0)
		{
			// Xử lý assets nếu cần
			continue;
		}
		if (line.rfind("[MAP]", 0) == 0)
		{
			continue; // Phần sau [MAP] sẽ là đường dẫn map
		}
		
		// Giả định dòng dưới [MAP] là JSON path
		if (line.find(".json") != string::npos)
		{
			wstring mapPath = ToWSTR(line);
			LoadMapJSON(mapPath.c_str());
		}
	}

	f.close();
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
			LPGAMEOBJECT gameObj = ObjectFactory::CreateFromJSON(obj);
			if (gameObj == nullptr) continue;

			CMapNode* node = dynamic_cast<CMapNode*>(gameObj);
			if (node != nullptr)
			{
				nodes[node->nodeId] = node;
				
				// Đọc custom properties
				if (obj.contains("properties"))
				{
					for (auto& prop : obj["properties"])
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
		// Tạm thời gán node đầu tiên tìm thấy
		player->currentNode = nodes.begin()->second; 
		float nx, ny;
		player->currentNode->GetPosition(nx, ny);
		player->SetPosition(nx, ny);
	}
}

void CWorldMapScene::Update(DWORD dt)
{
	float cx = 0;
	float cy = -8.0f; 

	if (map != NULL) 
	{
		float map_width = (float)(map->GetWidth() * map->GetTileWidth());
		float screen_width = (float)CGame::GetInstance()->GetBackBufferWidth();
		cx = (map_width - screen_width) / 2.0f;
	}

	CGame::GetInstance()->SetCamPos(cx, cy);

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
}

void CWorldMapScene::Unload()
{
	if (player) { delete player; player = NULL; }
	for (auto obj : mapObjects) { if (obj) delete obj; }
	mapObjects.clear();
	nodes.clear(); // Các CMapNode đã được đẩy vào mapObjects rồi nên sẽ bị delete trong vòng lặp trên
	
	if (map != NULL) { delete map; map = NULL; }
}
