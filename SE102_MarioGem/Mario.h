#pragma once
#include "GameObject.h"
#include "GameData.h"

#include "Animation.h"
#include "Animations.h"

#include "debug.h"
#include <vector>

constexpr float MARIO_WALKING_SPEED = 0.1f;
constexpr float MARIO_RUNNING_SPEED = 0.2f;
constexpr float MARIO_MAX_SPEED = 0.3f;

#define MARIO_PMETER_MAX			1000
#define MARIO_FLYING_TIME_MAX		4000

constexpr float MARIO_ACCEL_WALK_X = 0.0005f;
constexpr float MARIO_ACCEL_RUN_X = 0.0007f;
constexpr float MARIO_ACCEL_SKID_X = 0.0007f;

constexpr float MARIO_JUMP_SPEED_Y = 0.45f;
constexpr float MARIO_JUMP_RUN_SPEED_Y = 0.55f;
constexpr float MARIO_JUMP_MAX_SPEED_Y = 0.65f;

constexpr float MARIO_GRAVITY = 0.002f;
constexpr float MARIO_GRAVITY_JUMP = 0.001f;
constexpr float MARIO_FLOAT_SPEED_Y = 0.05f;

constexpr float MARIO_JUMP_DEFLECT_SPEED = 0.4f;

#pragma region ANIMATION_ID

// SMALL MARIO
constexpr int ID_ANI_MARIO_SMALL_IDLE = 2;
constexpr int ID_ANI_MARIO_SMALL_WALK = 3;
constexpr int ID_ANI_MARIO_SMALL_SKID = 4;
constexpr int ID_ANI_MARIO_SMALL_RUN = 5;
constexpr int ID_ANI_MARIO_SMALL_JUMP = 6;
constexpr int ID_ANI_MARIO_SMALL_DIE = 8;
constexpr int ID_ANI_MARIO_SMALL_CROUCH = 2; // fallback

// BIG MARIO
constexpr int ID_ANI_MARIO_BIG_IDLE = 18;
constexpr int ID_ANI_MARIO_BIG_WALK = 20;
constexpr int ID_ANI_MARIO_BIG_SKID = 21;
constexpr int ID_ANI_MARIO_BIG_RUN = 22;
constexpr int ID_ANI_MARIO_BIG_HIGH_SPEED = 23;
constexpr int ID_ANI_MARIO_BIG_JUMP = 24;
constexpr int ID_ANI_MARIO_BIG_FALL = 25;
constexpr int ID_ANI_MARIO_BIG_CROUCH = 27;

// FIRE MARIO
constexpr int ID_ANI_MARIO_FIRE_IDLE = 81;
constexpr int ID_ANI_MARIO_FIRE_WALK = 82;
constexpr int ID_ANI_MARIO_FIRE_SKID = 83;
constexpr int ID_ANI_MARIO_FIRE_THROW = 84;
constexpr int ID_ANI_MARIO_FIRE_RUN = 86;
constexpr int ID_ANI_MARIO_FIRE_HIGH_SPEED = 87;
constexpr int ID_ANI_MARIO_FIRE_JUMP = 88;
constexpr int ID_ANI_MARIO_FIRE_FALL = 89;
constexpr int ID_ANI_MARIO_FIRE_CROUCH = 91;

// RACCOON MARIO
constexpr int ID_ANI_MARIO_RACCOON_IDLE = 59;
constexpr int ID_ANI_MARIO_RACCOON_WALK = 60;
constexpr int ID_ANI_MARIO_RACCOON_SKID = 61;
constexpr int ID_ANI_MARIO_RACCOON_FLOAT = 62;
constexpr int ID_ANI_MARIO_RACCOON_JUMP = 63;
constexpr int ID_ANI_MARIO_RACCOON_FALL = 64;
constexpr int ID_ANI_MARIO_RACCOON_RUN = 65; // speed up
constexpr int ID_ANI_MARIO_RACCOON_FLY = 66;
constexpr int ID_ANI_MARIO_RACCOON_CROUCH = 68;

#pragma endregion

#define MARIO_BIG_BBOX_WIDTH  14
#define MARIO_BIG_BBOX_HEIGHT 26
#define MARIO_BIG_SITTING_BBOX_WIDTH  14
#define MARIO_BIG_SITTING_BBOX_HEIGHT 16

#define MARIO_SIT_HEIGHT_ADJUST ((MARIO_BIG_BBOX_HEIGHT-MARIO_BIG_SITTING_BBOX_HEIGHT)/2)

#define MARIO_SMALL_BBOX_WIDTH  13
#define MARIO_SMALL_BBOX_HEIGHT 14


#define MARIO_UNTOUCHABLE_TIME 2500
#define MARIO_THROWING_FIRE_TIME   180

class CMarioState; // Forward declaration

class CMario : public CGameObject
{
public:
	CMarioState* currentState = nullptr;

	BOOLEAN isSitting;
	float maxVx;
	float ax;				// acceleration on x 
	float ay;				// acceleration on y 

	int untouchable; 
	ULONGLONG untouchable_start;
	BOOLEAN isOnPlatform;

	BOOLEAN isThrowingFire;
	ULONGLONG throwingFireStartTime = 0;

	int pMeter;					
	ULONGLONG flyStartTime;





public:
	CMario(float x, float y) : CGameObject(x, y)
	{
		isSitting = false;
		maxVx = 0.0f;
		ax = 0.0f;
		ay = MARIO_GRAVITY; 


		untouchable = 0;
		untouchable_start = -1;
		isOnPlatform = false;
	}

	MarioLevel GetLevel() { return CGameData::GetInstance()->GetLevel(); }
	void SetLevel(MarioLevel l);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();
	void ChangeState(CMarioState* newState);

    float GetVelocityX() const { return vx; }
    void SetVelocityX(float v) { vx = v; }
    float GetVelocityY() const { return vy; }
    void SetVelocityY(float v) { vy = v; }
    
    float GetAccelerationX() const { return ax; }
    void SetAccelerationX(float a) { ax = a; }
    
    float GetAccelerationY() const { return ay; }
    void SetAccelerationY(float a) { ay = a; }

    int GetPMeter() const { return pMeter; }
    void ResetPMeter() { pMeter = 0; }
    void DecreasePMeter(int amount) { pMeter -= amount; if (pMeter < 0) pMeter = 0; }
    
    ULONGLONG GetFlyStartTime() const { return flyStartTime; }
    void StartFlying() { flyStartTime = GetTickCount64(); }
    
    float GetMaxVelocityX() const { return maxVx; }
    void SetMaxVelocityX(float m) { maxVx = m; }
    
    int GetDirection() const { return nx; }
    void SetDirection(int dir) { nx = dir; }
    
    float GetY() const { return y; }
    void SetY(float newY) { y = newY; }

    bool IsSitting() const { return isSitting; }
    void SetSitting(bool sit) { isSitting = sit; }

    bool IsOnPlatform() const { return isOnPlatform; }
    void SetOnPlatform(bool p) { isOnPlatform = p; }


	void ShootFireBall();
	void UpdateThrowingFireTime(DWORD dt);
	bool IsThrowingFire() { return isThrowingFire; };

	void HandlePMeter(DWORD dt);
	int GetCoins() { return CGameData::GetInstance()->GetCoin(); }
	int GetScore() { return CGameData::GetInstance()->GetScore(); }
	int GetLives() { return CGameData::GetInstance()->GetLives(); }
	void AddScore(int amount) { CGameData::GetInstance()->AddScore(amount); }

	int IsCollidable();
	int IsBlocking();

	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);

	// void SetLevel(int l); // Removed, merged with SetLevel(MarioLevel l)
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount64(); }

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);

	static LPGAMEOBJECT CreateFromTokens(const vector<string>& tokens);
};
