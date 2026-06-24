#include "ObjectFactory.h"
#include "Mario.h"
#include "Goomba.h"
#include "Coin.h"
#include "Portal.h"
#include "Platform.h"
#include "CollisionBox.h"
#include "Burner.h"
#include "Blaster.h"
#include "Canon.h"
#include "CannonBall.h"

using json = nlohmann::json;

LPGAMEOBJECT ObjectFactory::Create(ObjectType type, const vector<string>& tokens)
{
    switch (type)
    {
    case ObjectType::Mario:
        return CMario::CreateFromTokens(tokens);

    case ObjectType::Goomba:
        return CGoomba::CreateFromTokens(tokens);

    case ObjectType::Coin:
        return CCoin::CreateFromTokens(tokens);

    case ObjectType::Portal:
        return CPortal::CreateFromTokens(tokens);

    case ObjectType::Platform:
        return CPlatform::CreateFromTokens(tokens);

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


static int GetIntProperty(const json& obj, const string& name, int defaultVal = 0)
{
    if (!obj.contains("properties")) return defaultVal;
    for (auto& prop : obj["properties"])
    {
        if (prop["name"] == name)
            return prop["value"].get<int>();
    }
    return defaultVal;
}

static float GetFloatProperty(const json& obj, const string& name, float defaultVal = 0.0f)
{
    if (!obj.contains("properties")) return defaultVal;
    for (auto& prop : obj["properties"])
    {
        if (prop["name"] == name)
            return prop["value"].get<float>();
    }
    return defaultVal;
}

static string GetStringProperty(const json& obj, const string& name, const string& defaultVal = "")
{
    if (!obj.contains("properties")) return defaultVal;
    for (auto& prop : obj["properties"])
    {
        if (prop["name"] == name)
            return prop["value"].get<string>();
    }
    return defaultVal;
}

LPGAMEOBJECT ObjectFactory::CreateFromJSON(const json& obj)
{
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
    bool isNumber = !typeStr.empty() && (isdigit(typeStr[0]) || typeStr[0] == '-');
    if (isNumber)
    {
        objectType = static_cast<ObjectType>(atoi(typeStr.c_str()));
    }
    else
    {
        // Parse dạng tên chuỗi
        if (typeStr == "Mario")
            objectType = ObjectType::Mario;
        else if (typeStr == "Brick")
            objectType = ObjectType::Brick;
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
        else
        {
            DebugOut(L"[WARNING] Unknown object type in JSON: %s\n",
                wstring(typeStr.begin(), typeStr.end()).c_str());
            return nullptr;
        }
    }

    // Tạo object dựa trên type
    switch (objectType)
    {
    case ObjectType::Mario:
        return new CMario(x, y);

    case ObjectType::Goomba:
        return new CGoomba(x, y);

    case ObjectType::Coin:
        return new CCoin(x, y);

    case ObjectType::Portal:
    {
        // Portal cần thêm scene_id từ Custom Properties trong Tiled
        int sceneId = GetIntProperty(obj, "TargetSceneID", 1);
        return new CPortal(x, y, x + w, y + h, sceneId);
    }

    case ObjectType::CollisionBox:
    {
        float cx = x + w / 2.0f;
        float cy = y + h / 2.0f;
        return new CCollisionBox(cx, cy, w, h);
    }

    case ObjectType::Platform:
    {
        // Platform đọc thêm các Custom Properties từ Tiled
        float cellWidth = GetFloatProperty(obj, "CellWidth", 16.0f);
        float cellHeight = GetFloatProperty(obj, "CellHeight", 16.0f);
        int length = GetIntProperty(obj, "Length", 1);
        int spriteBegin = GetIntProperty(obj, "SpriteBegin", 0);
        int spriteMiddle = GetIntProperty(obj, "SpriteMiddle", 0);
        int spriteEnd = GetIntProperty(obj, "SpriteEnd", 0);
        return new CPlatform(x, y, cellWidth, cellHeight, length,
            spriteBegin, spriteMiddle, spriteEnd);
    }

    case ObjectType::Burner:
        return new CBurner(x, y);

    case ObjectType::Blaster:
        return new CBlaster(x, y);

    case ObjectType::Canon:
    {
        int dir = GetIntProperty(obj, "direction", 1);
        return new CCanon(x, y, dir);
    }

    default:
        DebugOut(L"[ERROR] Unhandled object type in JSON: %d\n", static_cast<int>(objectType));
        return nullptr;
    }
}