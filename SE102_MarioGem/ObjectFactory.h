#include "GameObject.h"
#include <string>
#include "AssetIDs.h"
#include <vector>
#include <functional>
#include <unordered_map>
using namespace std;

class ObjectFactory
{
public:
	static LPGAMEOBJECT Create(ObjectType type, const vector<string>& tokens);
};