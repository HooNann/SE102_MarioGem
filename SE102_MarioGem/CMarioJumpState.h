#pragma once
#include "CMarioState.h"

class CMarioJumpState : public CMarioState
{
public:
    MarioStateID GetID() override { return MarioStateID::Jump; }
    void Enter(CMario* mario) override;
    void Update(CMario* mario, DWORD dt) override;
};
