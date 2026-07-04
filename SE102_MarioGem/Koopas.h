#pragma once
#include "GameObject.h"
#include <string>

enum class KoopasState : int
{
	Walking = 100,
	ShellIdle = 200,
	ShellMoving = 300,
	Die_KnockOut = 400
};

constexpr float KOOPAS_GRAVITY = 0.002f;
constexpr float KOOPAS_WALKING_SPEED = 0.035f;
constexpr float KOOPAS_SHELL_SPEED = 0.18f;

constexpr int KOOPAS_BBOX_WIDTH = 16;
constexpr int KOOPAS_BBOX_HEIGHT = 26;
constexpr int KOOPAS_SHELL_BBOX_HEIGHT = 16;

constexpr int ID_ANI_KOOPAS_WALKING = 100027;
constexpr int ID_ANI_KOOPAS_SHELL_IDLE = 100028;
constexpr int ID_ANI_KOOPAS_SHELL_MOVING = 100030;
constexpr int ID_ANI_KOOPAS_DIE_KNOCKOUT = 100028;

class CKoopas : public CGameObject
{
protected:
	float ax;
	float ay;

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	virtual int IsCollidable() { return state != static_cast<int>(KoopasState::Die_KnockOut); }
	virtual int IsBlocking() { return 0; }
	virtual void OnNoCollision(DWORD dt);
	virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
	CKoopas(float x, float y);

	bool IsShellIdle() const { return state == static_cast<int>(KoopasState::ShellIdle); }
	bool IsShellMoving() const { return state == static_cast<int>(KoopasState::ShellMoving); }
	bool IsWalking() const { return state == static_cast<int>(KoopasState::Walking); }
	bool IsDangerousToMario() const { return IsWalking() || IsShellMoving(); }

	void Kick(int direction);
	void SetState(KoopasState state);
	virtual void SetState(int state) override { SetState(static_cast<KoopasState>(state)); }

	static LPGAMEOBJECT CreateFromTokens(const vector<string>& tokens);
};
