#include "ObjectFactory.h"
#include "Mario.h"
#include "Goomba.h"
#include "Coin.h"
#include "Portal.h"
#include "Platform.h"

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

    default:
        DebugOut(L"[ERROR] Invalid object type: %d\n", static_cast<int>(type));
        return nullptr;
    }
}