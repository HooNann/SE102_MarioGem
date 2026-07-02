#pragma once
#include "CMarioState.h"

class CMarioWalkState : public CMarioState
{
public:
    MarioStateID GetID() override { return MarioStateID::Walk; }
    void Enter(CMario* mario) override;
    void Update(CMario* mario, DWORD dt) override;
};
