
#pragma once

enum class ObjectType : int {
	Mario = 0,
	Brick = 1,
	Goomba = 2,
	Koopas = 3,
	Coin = 4,
	Platform = 5,
	Portal = 50,
	Burner = 7,
	Blaster = 8,
	Canon = 9,
	CannonBall = 10,
	CollisionBox = 100,
	QuestionBlock = 101,
	GoalRoulette = 102,
	BoomBoom = 36,
	MapMario = 200,
	MapNode = 201,
	MapDecoration = 202, // Cây, lâu đài...
};

enum class SceneType : int {
	PlayScene = 1,
	WorldMapScene = 2
};

enum class MarioStateID
{
    Idle,
    Walk,
    Run,
    Skid,
    Jump,
    Fall,
    Float,
    Fly,
    Duck,
    Climb,
    Swim,
    Pipe,
    AutoMove,
    Dead
};

