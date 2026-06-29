#pragma once
#include "GameObject.h"
#include <string>
#include "AssetIDs.h"
#include <vector>
#include <functional>
#include <unordered_map>
#include "json.hpp"
using namespace std;

class ObjectFactory
{
public:
	static LPGAMEOBJECT Create(ObjectType type, const vector<string>& tokens);

	// Tạo object từ dữ liệu JSON (đọc từ Tiled Map Editor Object Layer)
	static LPGAMEOBJECT CreateFromJSON(const nlohmann::json& obj);
};