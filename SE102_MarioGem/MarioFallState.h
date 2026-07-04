#pragma once
#include "MarioState.h"

class CMarioFallState : public CMarioState
{
public:
    MarioStateID GetID() override { return MarioStateID::Fall; }
    void Enter(CMario* mario) override;
    void Update(CMario* mario, DWORD dt) override;
    int GetAnimationId(CMario* mario) override;
    void OnCollisionWith(CMario* mario, LPCOLLISIONEVENT e) override;
};
