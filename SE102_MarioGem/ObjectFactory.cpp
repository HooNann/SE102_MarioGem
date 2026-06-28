#include "ObjectFactory.h"
#include "Coin.h"
#include "Goomba.h"
#include "Mario.h"
#include "Platform.h"
#include "Portal.h"
#include "Brick.h"
#include "BoomBoom.h"
#include "debug.h"


LPGAMEOBJECT ObjectFactory::Create(ObjectType type,
                                   const vector<string> &tokens) {
  switch (type) {
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

  case ObjectType::Brick:
    return CBrick::CreateFromTokens(tokens);

  case ObjectType::BoomBoom:
    return CBoomBoom::CreateFromTokens(tokens);

  default:
    DebugOut(L"[ERROR] Invalid object type: %d\n", static_cast<int>(type));
    return nullptr;
  }
}