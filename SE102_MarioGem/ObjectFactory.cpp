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
#include "MapMario.h"
#include "MapNode.h"
#include "MapObject.h"
#include "Mario.h"
#include "Platform.h"
#include "Portal.h"
#include "QuestionBlock.h"


using json = nlohmann::json;

LPGAMEOBJECT ObjectFactory::Create(ObjectType type,
                                   const vector<string> &tokens) {
  switch (type) {
  case ObjectType::Mario:
    return CMario::CreateFromTokens(tokens);

  case ObjectType::Brick:
    return CBrick::CreateFromTokens(tokens);

  case ObjectType::BoomBoom:
    return CBoomBoom::CreateFromTokens(tokens);

  case ObjectType::Goomba:
    return CGoomba::CreateFromTokens(tokens);

  case ObjectType::Coin:
    return CCoin::CreateFromTokens(tokens);

  case ObjectType::Portal:
    return CPortal::CreateFromTokens(tokens);

  case ObjectType::Burner:
    return CBurner::CreateFromTokens(tokens);

  case ObjectType::Blaster:
    return CBlaster::CreateFromTokens(tokens);

  case ObjectType::Canon:
    return CCanon::CreateFromTokens(tokens);

  default:
    DebugOut(L"[ERROR] Invalid object type: %d\n", static_cast<int>(type));
    return nullptr;
  }
}

static int GetIntProperty(const json &obj, const string &name,
                          int defaultVal = 0) {
  if (!obj.contains("properties"))
    return defaultVal;
  for (auto &prop : obj["properties"]) {
    if (prop["name"] == name)
      return prop["value"].get<int>();
  }
  return defaultVal;
}

static float GetFloatProperty(const json &obj, const string &name,
                              float defaultVal = 0.0f) {
  if (!obj.contains("properties"))
    return defaultVal;
  for (auto &prop : obj["properties"]) {
    if (prop["name"] == name)
      return prop["value"].get<float>();
  }
  return defaultVal;
}

static string GetStringProperty(const json &obj, const string &name,
                                const string &defaultVal = "") {
  if (!obj.contains("properties"))
    return defaultVal;
  for (auto &prop : obj["properties"]) {
    if (prop["name"] == name)
      return prop["value"].get<string>();
  }
  return defaultVal;
}

LPGAMEOBJECT ObjectFactory::CreateFromJSON(const json &obj) {
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
  } else {
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
    else if (typeStr == "CollisionBox")
      objectType = ObjectType::CollisionBox;
    else if (typeStr == "Burner")
      objectType = ObjectType::Burner;
    else if (typeStr == "Blaster")
      objectType = ObjectType::Blaster;
    else if (typeStr == "Canon")
      objectType = ObjectType::Canon;
    else if (typeStr == "MapMario")
      objectType = ObjectType::MapMario;
    else if (typeStr == "MapNode")
      objectType = ObjectType::MapNode;
    else if (typeStr == "MapDecoration")
      objectType = ObjectType::MapDecoration;
	else if (typeStr == "QuestionBlock")
		objectType = ObjectType::QuestionBlock;
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

  case ObjectType::Coin:
    return new CCoin(x, y);

  case ObjectType::Portal: {
    // Portal cần thêm scene_id từ Custom Properties trong Tiled
    int sceneId = GetIntProperty(obj, "TargetSceneID", 1);
    return new CPortal(x, y, x + w, y + h, sceneId);
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

  case ObjectType::Blaster:
    return new CBlaster(x, y);

  case ObjectType::Canon: {
    int dir = GetIntProperty(obj, "direction", 1);
    return new CCanon(x, y, dir);
  }

  case ObjectType::MapMario: {
    int aniId = GetIntProperty(obj, "aniId", -1);
    int start_node_id = GetIntProperty(obj, "start_node_id", -1);
    float ox = (w > 0) ? w / 2.0f : 8.0f;
    float oy = (h > 0) ? h / 2.0f : 8.0f;
    return new CMapMario(x + ox, y + oy, aniId, start_node_id);
  }

  case ObjectType::MapNode: {
    int id = obj.value("id", -1);
    int sceneId = GetIntProperty(obj, "scene_id", -1);
    float ox = (w > 0) ? w / 2.0f : 8.0f;
    float oy = (h > 0) ? h / 2.0f : 8.0f;
    return new CMapNode(id, x + ox, y + oy, sceneId);
  }

  case ObjectType::MapDecoration: {
    int aniId = GetIntProperty(obj, "aniId", -1);
    float ox = (w > 0) ? w / 2.0f : 8.0f;
    float oy = (h > 0) ? h / 2.0f : 8.0f;
    return new CMapObject(x + ox, y + oy, aniId);
  }

  default:
    DebugOut(L"[ERROR] Unhandled object type in JSON: %d\n",
             static_cast<int>(objectType));
    return nullptr;
  }
}