#pragma once
#include "MarioState.h"

class CMarioDeadState : public CMarioState
{
public:
    MarioStateID GetID() override { return MarioStateID::Dead; }
    void Enter(CMario* mario) override;
    void Update(CMario* mario, DWORD dt) override;
    int GetAnimationId(CMario* mario) override;
};
