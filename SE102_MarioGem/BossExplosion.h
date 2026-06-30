#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

class CBossExplosion : public CGameObject
{
    DWORD startTime;
public:
    CBossExplosion(float x, float y, float vx, float vy);

    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
    void Render();
    void GetBoundingBox(float& l, float& t, float& r, float& b);

    int IsBlocking() { return 0; }
    int IsCollidable() { return 0; }
};
