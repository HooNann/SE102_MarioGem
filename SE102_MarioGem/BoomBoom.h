#pragma once
#include "GameObject.h"

#define BOOMBOOM_GRAVITY 0.002f
#define BOOMBOOM_WALKING_SPEED 0.05f

#define BOOMBOOM_BBOX_WIDTH 28
#define BOOMBOOM_BBOX_HEIGHT 24
#define BOOMBOOM_BBOX_HEIGHT_HIDING 20
#define BOOMBOOM_BBOX_HEIGHT_HURT 14
#define BOOMBOOM_BBOX_HEIGHT_DIE 14

#define BOOMBOOM_STATE_WALKING 100
#define BOOMBOOM_STATE_DIE 200
#define BOOMBOOM_STATE_HIDING 300
#define BOOMBOOM_STATE_HURT 400

#define BOOMBOOM_HIDING_TIME 2000
#define BOOMBOOM_HURT_TIME 1500

#define BOOMBOOM_JUMP_SPEED 0.35f
#define BOOMBOOM_JUMP_COOLDOWN 1000

class CBoomBoom : public CGameObject
{
protected:
	float ax;
	float ay;

	int hp;
	int untouchable;
	ULONGLONG untouchable_start;
	
	bool isOnPlatform;
	ULONGLONG last_jump_time;
	size_t victoryTrackId;

	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects);
	virtual void Render();

	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);

	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
	CBoomBoom(float x, float y);
	virtual void SetState(int state);
	void TakeDamage();

	static LPGAMEOBJECT CreateFromTokens(const vector<string>& tokens);
};
