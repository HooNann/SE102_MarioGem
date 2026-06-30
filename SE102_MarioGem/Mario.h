#pragma once
#include "GameObject.h"

#include "Animation.h"
#include "Animations.h"

#include "debug.h"
#include <vector>

constexpr float MARIO_WALKING_SPEED = 0.1f;
constexpr float MARIO_RUNNING_SPEED = 0.2f;
#define MARIO_PMETER_MAX			1000
#define MARIO_FLYING_TIME_MAX		4000

constexpr float MARIO_ACCEL_WALK_X = 0.0005f;
constexpr float MARIO_ACCEL_RUN_X = 0.0007f;

constexpr float MARIO_JUMP_SPEED_Y = 0.5f;
constexpr float MARIO_JUMP_RUN_SPEED_Y = 0.6f;

constexpr float MARIO_GRAVITY = 0.002f;

constexpr float MARIO_JUMP_DEFLECT_SPEED = 0.4f;

enum class MarioState : int
{
	Die = -10,
	Idle = 0,
	WalkingRight = 100,
	WalkingLeft = 200,
	Jump = 300,
	ReleaseJump = 301,
	RunningRight = 400,
	RunningLeft = 500,
	Sit = 600,
	SitRelease = 601,
	Fly = 700,
	Float = 800

};


#pragma region ANIMATION_ID

constexpr int ID_ANI_MARIO_IDLE_RIGHT = 400;
constexpr int ID_ANI_MARIO_IDLE_LEFT = 401;

constexpr int ID_ANI_MARIO_WALKING_RIGHT = 500;
constexpr int ID_ANI_MARIO_WALKING_LEFT = 501;

constexpr int ID_ANI_MARIO_RUNNING_RIGHT = 600;
constexpr int ID_ANI_MARIO_RUNNING_LEFT = 601;

constexpr int ID_ANI_MARIO_JUMP_WALK_RIGHT = 700;
constexpr int ID_ANI_MARIO_JUMP_WALK_LEFT = 701;

constexpr int ID_ANI_MARIO_JUMP_RUN_RIGHT = 800;
constexpr int ID_ANI_MARIO_JUMP_RUN_LEFT = 801;

constexpr int ID_ANI_MARIO_SIT_RIGHT = 900;
constexpr int ID_ANI_MARIO_SIT_LEFT = 901;

constexpr int ID_ANI_MARIO_BRACE_RIGHT = 1000;
constexpr int ID_ANI_MARIO_BRACE_LEFT = 1001;

constexpr int ID_ANI_MARIO_DIE = 999;

// SMALL MARIO
constexpr int ID_ANI_MARIO_SMALL_IDLE_RIGHT = 1100;
constexpr int ID_ANI_MARIO_SMALL_IDLE_LEFT = 1102;

constexpr int ID_ANI_MARIO_SMALL_WALKING_RIGHT = 1200;
constexpr int ID_ANI_MARIO_SMALL_WALKING_LEFT = 1201;

constexpr int ID_ANI_MARIO_SMALL_RUNNING_RIGHT = 1300;
constexpr int ID_ANI_MARIO_SMALL_RUNNING_LEFT = 1301;

constexpr int ID_ANI_MARIO_SMALL_BRACE_RIGHT = 1400;
constexpr int ID_ANI_MARIO_SMALL_BRACE_LEFT = 1401;

constexpr int ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT = 1500;
constexpr int ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT = 1501;

constexpr int ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT = 1600;
constexpr int ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT = 1601;

#pragma endregion

constexpr float GROUND_Y = 160.0f;

//FIRE MARIO
constexpr int ID_ANI_MARIO_FIRE_IDLE_RIGHT = 1700;
constexpr int ID_ANI_MARIO_FIRE_IDLE_LEFT = 1701;
constexpr int ID_ANI_MARIO_FIRE_WALKING_RIGHT = 1800;
constexpr int ID_ANI_MARIO_FIRE_WALKING_LEFT = 1801;
constexpr int ID_ANI_MARIO_FIRE_RUNNING_RIGHT = 1900;
constexpr int ID_ANI_MARIO_FIRE_RUNNING_LEFT = 1901;
constexpr int ID_ANI_MARIO_FIRE_JUMP_WALK_RIGHT = 2000;
constexpr int ID_ANI_MARIO_FIRE_JUMP_WALK_LEFT = 2001;
constexpr int ID_ANI_MARIO_FIRE_JUMP_RUN_RIGHT = 2100;
constexpr int ID_ANI_MARIO_FIRE_JUMP_RUN_LEFT = 2101;
constexpr int ID_ANI_MARIO_FIRE_BRACE_RIGHT = 2200;
constexpr int ID_ANI_MARIO_FIRE_BRACE_LEFT = 2201;
constexpr int ID_ANI_MARIO_FIRE_THROW_RIGHT = 2300;
constexpr int ID_ANI_MARIO_FIRE_THROW_LEFT = 2301;
constexpr int ID_ANI_MARIO_FIRE_SIT_RIGHT = 2400;
constexpr int ID_ANI_MARIO_FIRE_SIT_LEFT = 2401;


//RACCOON MARIO 
constexpr int ID_ANI_MARIO_RACCOON_IDLE_RIGHT = 2500;
constexpr int ID_ANI_MARIO_RACCOON_IDLE_LEFT = 2501;
constexpr int ID_ANI_MARIO_RACCOON_WALKING_RIGHT = 2600;
constexpr int ID_ANI_MARIO_RACCOON_WALKING_LEFT = 2601;
constexpr int ID_ANI_MARIO_RACCOON_RUNNING_RIGHT = 2700;
constexpr int ID_ANI_MARIO_RACCOON_RUNNING_LEFT = 2701;
constexpr int ID_ANI_MARIO_RACCOON_JUMP_WALK_RIGHT = 2800;
constexpr int ID_ANI_MARIO_RACCOON_JUMP_WALK_LEFT = 2801;
constexpr int ID_ANI_MARIO_RACCOON_JUMP_RUN_RIGHT = 2900;
constexpr int ID_ANI_MARIO_RACCOON_JUMP_RUN_LEFT = 2901;
constexpr int ID_ANI_MARIO_RACCOON_BRACE_RIGHT = 3000;
constexpr int ID_ANI_MARIO_RACCOON_BRACE_LEFT = 3001;
constexpr int ID_ANI_MARIO_RACCOON_SIT_RIGHT = 3200;
constexpr int ID_ANI_MARIO_RACCOON_SIT_LEFT = 3201;
constexpr int ID_ANI_MARIO_RACCOON_FLY_RIGHT = 3100;
constexpr int ID_ANI_MARIO_RACCOON_FLY_LEFT = 3101;

//RACCOON MARIO

//#define	MARIO_LEVEL_SMALL	1
//#define	MARIO_LEVEL_BIG		2
enum class MarioLevel : int
{
	Small = 1,
	Big = 2,
	Fire = 3,
	Raccoon = 4
};

#define MARIO_BIG_BBOX_WIDTH  14
#define MARIO_BIG_BBOX_HEIGHT 24
#define MARIO_BIG_SITTING_BBOX_WIDTH  14
#define MARIO_BIG_SITTING_BBOX_HEIGHT 16

#define MARIO_SIT_HEIGHT_ADJUST ((MARIO_BIG_BBOX_HEIGHT-MARIO_BIG_SITTING_BBOX_HEIGHT)/2)

#define MARIO_SMALL_BBOX_WIDTH  13
#define MARIO_SMALL_BBOX_HEIGHT 12


#define MARIO_UNTOUCHABLE_TIME 2500
#define MARIO_THROWING_FIRE_TIME   180

class CMario : public CGameObject
{
	BOOLEAN isSitting;
	float maxVx;
	float ax;				// acceleration on x 
	float ay;				// acceleration on y 

	MarioLevel level;
	int untouchable; 
	ULONGLONG untouchable_start;
	BOOLEAN isOnPlatform;
	int coin;
	int score;
	int lives;

	BOOLEAN isThrowingFire;
	DWORD throwingFireStartTime;

	int pMeter;					
	DWORD flyStartTime;

	void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
	void OnCollisionWithCoin(LPCOLLISIONEVENT e);
	void OnCollisionWithPortal(LPCOLLISIONEVENT e);
	void OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e);
	void OnCollisionWithEnemy(LPCOLLISIONEVENT e);
	void OnCollisionWithTrap(LPCOLLISIONEVENT e);
	void OnCollisionWithItem(LPCOLLISIONEVENT e);

	int GetAniIdBig();
	int GetAniIdSmall();
	int GetAniIdFire();
	int GetAniIdRaccoon();


public:
	CMario(float x, float y) : CGameObject(x, y)
	{
		isSitting = false;
		maxVx = 0.0f;
		ax = 0.0f;
		ay = MARIO_GRAVITY; 


		level = MarioLevel::Big;
		untouchable = 0;
		untouchable_start = -1;
		isOnPlatform = false;
		coin = 0;
		score = 0;
		lives = 4;
	}

	MarioLevel GetLevel() { return level; }
	void SetLevel(MarioLevel l) { level = l; }
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();
	void SetState(MarioState state);
	void SetState(int state) override { SetState(static_cast<MarioState>(state)); }

	void ShootFireBall();
	void UpdateThrowingFireTime(DWORD dt);
	bool IsThrowingFire() { return isThrowingFire; };

	void HandlePMeter(DWORD dt);
	void FlyUp();
	void FloatDown();
	int GetPMeter() { return pMeter; }
	int GetCoins() { return coin; }
	int GetScore() { return score; }
	int GetLives() { return lives; }
	void AddScore(int amount) { score += amount; }

	int IsCollidable()
	{ 
		return (state != static_cast<int>(MarioState::Die)); 
	}

	int IsBlocking() { return (state != static_cast<int>(MarioState::Die) && untouchable==0); }

	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);
	void OnCollisionWithBurner(LPCOLLISIONEVENT e);
	void OnCollisionWithBlaster(LPCOLLISIONEVENT e);
	void OnCollisionWithCannonBall(LPCOLLISIONEVENT e);

	void SetLevel(int l);
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount64(); }

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);

	static LPGAMEOBJECT CreateFromTokens(const vector<string>& tokens);
};
