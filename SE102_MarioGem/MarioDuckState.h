#pragma once
#include "MarioState.h"

class CMarioDuckState : public CMarioState
{
public:
    MarioStateID GetID() override { return MarioStateID::Duck; }
    void Enter(CMario* mario) override;
    void Update(CMario* mario, DWORD dt) override;
    int GetAnimationId(CMario* mario) override;
    void Exit(CMario* mario) override;
};
