#include "ObjectFactory.h"
#include "Blaster.h"
#include "BoomBoom.h"
#include "Brick.h"
#include "Burner.h"
#include "CannonBall.h"
#include "Canon.h"
#include "Coin.h"
#include "CollisionBox.h"
#include "Goomba.h"
#include "Koopas.h"
#include "MapMario.h"
#include "MapNode.h"
#include "MapObject.h"
#include "Mario.h"
#include "Pipe.h"
#include "Platform.h"
#include "Portal.h"
#include "QuestionBlock.h"
#include "RedVenus.h"
#include "GoalRoulette.h"


using json = nlohmann::json;

LPGAMEOBJECT ObjectFactory::Create(ObjectType type,
	const vector<string>& tokens) {
	switch (type) {
	case ObjectType::Mario:
		return CMario::CreateFromTokens(tokens);

	case ObjectType::Brick:
		return CBrick::CreateFromTokens(tokens);

	case ObjectType::BoomBoom:
		return CBoomBoom::CreateFromTokens(tokens);

	case ObjectType::Goomba:
		return CGoomba::CreateFromTokens(tokens);

	case ObjectType::Koopas:
		return CKoopas::CreateFromTokens(tokens);

	case ObjectType::Coin:
		return CCoin::CreateFromTokens(tokens);

	case ObjectType::Platform:
	{
		float x = (float)atof(tokens[1].c_str());
		float y = (float)atof(tokens[2].c_str());
		int length = atoi(tokens[5].c_str());
		float width = length * 16.0f; // Each block is 16px
		float height = 16.0f;
		float cx = x + width / 2.0f;
		float cy = y + height / 2.0f;
		return new CPlatform(cx, cy, width, height);
	}

	case ObjectType::Portal:
		return CPortal::CreateFromTokens(tokens);

	case ObjectType::Pipe:
		return CPipe::CreateFromTokens(tokens);

	case ObjectType::Burner:
		return CBurner::CreateFromTokens(tokens);

	case ObjectType::Blaster:
		return CBlaster::CreateFromTokens(tokens);

	case ObjectType::Canon:
		return CCanon::CreateFromTokens(tokens);

	case ObjectType::RedVenus:
		return CRedVenus::CreateFromTokens(tokens);

	case ObjectType::GoalRoulette:
		// Not supported via old text file method yet
		return nullptr;

	default:
		DebugOut(L"[ERROR] Invalid object type: %d\n", static_cast<int>(type));
		return nullptr;
	}
}

static int GetIntProperty(const json& obj, const string& name,
	int defaultVal = 0) {
	if (!obj.contains("properties"))
		return defaultVal;
	for (auto& prop : obj["properties"]) {
		if (prop["name"] == name)
			return prop["value"].get<int>();
	}
	return defaultVal;
}

static float GetFloatProperty(const json& obj, const string& name,
	float defaultVal = 0.0f) {
	if (!obj.contains("properties"))
		return defaultVal;
	for (auto& prop : obj["properties"]) {
		if (prop["name"] == name)
			return prop["value"].get<float>();
	}
	return defaultVal;
}

static string GetStringProperty(const json& obj, const string& name,
	const string& defaultVal = "") {
	if (!obj.contains("properties"))
		return defaultVal;
	for (auto& prop : obj["properties"]) {
		if (prop["name"] == name)
			return prop["value"].get<string>();
	}
	return defaultVal;
}

LPGAMEOBJECT ObjectFactory::CreateFromJSON(const json& obj) {
	// Đọc thuộc tính "type" hoặc "class"
	string typeStr = "";
	if (obj.contains("type") && obj["type"].is_string())
		typeStr = obj["type"].get<string>();
	else if (obj.contains("class") && obj["class"].is_string())
		typeStr = obj["class"].get<string>();

	// Đọc tọa độ
	float x = obj.value("x", 0.0f);
	float y = obj.value("y", 0.0f);
	float w = obj.value("width", 0.0f);
	float h = obj.value("height", 0.0f);

	// Xác định ObjectType từ chuỗi type
	// Hỗ trợ cả dạng số ("0", "2") và dạng tên ("Mario", "Goomba")
	ObjectType objectType;

	// Thử parse dạng số trước
	bool isNumber =
		!typeStr.empty() && (isdigit(typeStr[0]) || typeStr[0] == '-');
	if (isNumber) {
		objectType = static_cast<ObjectType>(atoi(typeStr.c_str()));
	}
	else {
		// Parse dạng tên chuỗi
		if (typeStr == "Mario")
			objectType = ObjectType::Mario;
		else if (typeStr == "Brick")
			objectType = ObjectType::Brick;
		else if (typeStr == "BoomBoom")
			objectType = ObjectType::BoomBoom;
		else if (typeStr == "Goomba")
			objectType = ObjectType::Goomba;
		else if (typeStr == "Koopas")
			objectType = ObjectType::Koopas;
		else if (typeStr == "Coin")
			objectType = ObjectType::Coin;
		else if (typeStr == "Platform")
			objectType = ObjectType::Platform;
		else if (typeStr == "Portal")
			objectType = ObjectType::Portal;
		else if (typeStr == "Pipe")
			objectType = ObjectType::Pipe;
		else if (typeStr == "CollisionBox")
			objectType = ObjectType::CollisionBox;
		else if (typeStr == "Burner")
			objectType = ObjectType::Burner;
		else if (typeStr == "Blaster")
			objectType = ObjectType::Blaster;
		else if (typeStr == "Canon")
			objectType = ObjectType::Canon;
		else if (typeStr == "RedVenus" || typeStr == "PiranhaPlant")
			objectType = ObjectType::RedVenus;
		else if (typeStr == "MapMario")
			objectType = ObjectType::MapMario;
		else if (typeStr == "MapNode")
			objectType = ObjectType::MapNode;
		else if (typeStr == "MapDecoration")
			objectType = ObjectType::MapDecoration;
		else if (typeStr == "QuestionBlock")
			objectType = ObjectType::QuestionBlock;
		else if (typeStr == "GoalRoulette")
			objectType = ObjectType::GoalRoulette;
		else {
			DebugOut(L"[WARNING] Unknown object type in JSON: %s\n",
				wstring(typeStr.begin(), typeStr.end()).c_str());
			return nullptr;
		}
	}

	// Tạo object dựa trên type
	switch (objectType) {
	case ObjectType::Mario:
		return new CMario(x, y);

	case ObjectType::Brick:
		return new CBrick(x, y);

	case ObjectType::BoomBoom:
		return new CBoomBoom(x, y);

	case ObjectType::Goomba:
		return new CGoomba(x, y);

	case ObjectType::Koopas:
		return new CKoopas(x, y);

	case ObjectType::Coin:
		return new CCoin(x, y);

	case ObjectType::Portal: {
		// Portal cần thêm scene_id từ Custom Properties trong Tiled
		int sceneId = GetIntProperty(obj, "target_scene_id", 1);
		return new CPortal(x, y, x + w, y + h, sceneId);
	}

	case ObjectType::Pipe: {
		float targetX = GetFloatProperty(obj, "target_x", x + w / 2.0f);
		float targetY = GetFloatProperty(obj, "target_y", y);
		string entryDirection = GetStringProperty(obj, "entry_direction", "down");
		string exitDirection = GetStringProperty(obj, "exit_direction", "up");
		return new CPipe(x, y, x + w, y + h, targetX, targetY,
			CPipe::ParseDirection(entryDirection),
			CPipe::ParseDirection(exitDirection));
	}

	case ObjectType::CollisionBox: {
		float cx = x + w / 2.0f;
		float cy = y + h / 2.0f;
		return new CCollisionBox(cx, cy, w, h);
	}

	case ObjectType::Platform: {
		// Platform giờ đây chỉ lấy chiều dài và chiều rộng giống hệt CollisionBox
		float cx = x + w / 2.0f;
		float cy = y + h / 2.0f;
		return new CPlatform(cx, cy, w, h);
	}

	case ObjectType::Burner:
		return new CBurner(x, y);

	case ObjectType::QuestionBlock: {
		int item_type = GetIntProperty(obj, "item_type", -1);
		return new CQuestionBlock(x, y, item_type);
	}

	case ObjectType::GoalRoulette: {
		// Rectangle in Tiled map gives x, y (top left) and width, height.
		float cx = x + w / 2.0f;
		float cy = y + h / 2.0f;
		return new CGoalRoulette(cx, cy);
	}

	case ObjectType::Blaster:
		return new CBlaster(x, y);

	case ObjectType::Canon: {
		int dir = GetIntProperty(obj, "direction", 1);
		return new CCanon(x, y, dir);
	}

	case ObjectType::RedVenus:
		return new CRedVenus(x, y);

	case ObjectType::MapMario: {
		int aniId = GetIntProperty(obj, "aniId", -1);
		int start_node_id = GetIntProperty(obj, "start_node_id", -1);
		return new CMapMario(x, y, aniId, start_node_id);
	}

	case ObjectType::MapNode: {
		int id = obj.value("id", -1);
		int sceneId = GetIntProperty(obj, "scene_id", -1);
		int aniActive = GetIntProperty(obj, "ani_active", -1);
		int aniCleared = GetIntProperty(obj, "ani_cleared", -1);
		return new CMapNode(id, x, y, sceneId, aniActive, aniCleared);
	}

	case ObjectType::MapDecoration: {
		int aniId = GetIntProperty(obj, "aniId", -1);
		return new CMapObject(x, y, aniId);
	}

	default:
		DebugOut(L"[ERROR] Unhandled object type in JSON: %d\n",
			static_cast<int>(objectType));
		return nullptr;
	}
}
