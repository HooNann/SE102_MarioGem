#pragma once
#include "CMarioState.h"

class CMarioFallState : public CMarioState
{
public:
    MarioStateID GetID() override { return MarioStateID::Fall; }
    void Enter(CMario* mario) override;
    void Update(CMario* mario, DWORD dt) override;
};
