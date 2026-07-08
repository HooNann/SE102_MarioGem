#pragma once
#include "GameObject.h"
#include <string>


enum class GoombaState : int
{
	Walking = 100,
	Die = 200,
	Die_KnockOut = 300
};


constexpr float GOOMBA_GRAVITY = 0.002f;
constexpr float GOOMBA_WALKING_SPEED = 0.05f;


constexpr int GOOMBA_BBOX_WIDTH = 16;
constexpr int GOOMBA_BBOX_HEIGHT = 14;
constexpr int GOOMBA_BBOX_HEIGHT_DIE = 7;
constexpr int GOOMBA_DIE_TIMEOUT = 500;


constexpr int ID_ANI_GOOMBA_WALKING = 100001;
constexpr int ID_ANI_GOOMBA_DIE = 100002;
constexpr int ID_ANI_GOOMBA_DIE_KNOCKOUT = 100000;

class CGoomba : public CGameObject
{
protected:
	float ax;				
	float ay; 

	ULONGLONG die_start;

	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects);
	virtual void Render();

	virtual int IsCollidable() { return (state != static_cast<int>(GoombaState::Die) && state != static_cast<int>(GoombaState::Die_KnockOut)); };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public: 	
	CGoomba(float x, float y);
	void SetState(GoombaState state);
	virtual void SetState(int state) override { SetState(static_cast<GoombaState>(state)); }

	static LPGAMEOBJECT CreateFromTokens(const vector<string>& tokens);
};
