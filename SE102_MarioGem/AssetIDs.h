
#pragma once

enum class ObjectType : int {
	Mario = 0,
	Brick = 1,
	Goomba = 2,
	Koopas = 3,
	Coin = 4,
	Platform = 5,
	Portal = 50,
	Pipe = 51,
	Burner = 7,
	Blaster = 8,
	Canon = 9,
	CannonBall = 10,
	RedVenus = 11,
	CollisionBox = 100,
	QuestionBlock = 101,
	GoalRoulette = 102,
	Flower = 103,
	Leaf = 104,
	Mushroom = 105,
	BoomBoom = 36,
	MapMario = 200,
	MapNode = 201,
	MapDecoration = 202, // Cây, lâu đài...
};

enum class SceneType : int {
	PlayScene = 1,
	WorldMapScene = 2,
	SoundTestScene = 3,
	IntroScene = 4
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

