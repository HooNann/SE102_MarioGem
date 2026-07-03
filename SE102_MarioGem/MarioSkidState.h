#pragma once
#include "MarioState.h"

class CMarioSkidState : public CMarioState
{
public:
    MarioStateID GetID() override { return MarioStateID::Skid; }
    void Enter(CMario* mario) override;
    void Update(CMario* mario, DWORD dt) override;
    int GetAnimationId(CMario* mario) override;
};
