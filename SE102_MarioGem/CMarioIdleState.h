#pragma once
#include "CMarioState.h"

class CMarioIdleState : public CMarioState
{
public:
    MarioStateID GetID() override { return MarioStateID::Idle; }
    void Enter(CMario* mario) override;
    void Update(CMario* mario, DWORD dt) override;
};
