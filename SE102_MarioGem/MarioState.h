#pragma once
#include <Windows.h>
#include "AssetIDs.h"
#include "Collision.h"

class CMario;

struct Input {
    int KeyCode;
};

class CMarioState
{
public:
    virtual MarioStateID GetID() = 0;
    
    virtual void Enter(CMario* mario) {}
    virtual void Update(CMario* mario, DWORD dt) = 0;
    virtual void Exit(CMario* mario) {}
    virtual void HandleInput(CMario* mario, const Input& input) {}
    
    virtual void OnCollisionWith(CMario* mario, LPCOLLISIONEVENT e);
    
    virtual int GetAnimationId(CMario* mario);
    
    virtual ~CMarioState() = default;
};
