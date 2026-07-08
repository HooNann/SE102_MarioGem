#pragma once
#include "GameObject.h"
#include "Game.h"

class CCamera
{
private:
	static CCamera* __instance;

	float cam_x, cam_y;
	float width, height;
	float max_x, max_y;
	float min_x, min_y;

	LPGAMEOBJECT target;

	CCamera();

public:
	static CCamera* GetInstance();

	void SetTarget(LPGAMEOBJECT t) { target = t; }
	void SetSize(float w, float h) { width = w; height = h; }
	void SetCameraBounds(float min_x, float min_y, float max_x, float max_y) { 
		this->min_x = min_x; this->min_y = min_y; 
		this->max_x = max_x; this->max_y = max_y; 
	}

	void Update();

	float GetWidth() { return width; }
	float GetHeight() { return height; }
	void GetCamPos(float& x, float& y) { x = cam_x; y = cam_y; }
	void SetCamPos(float x, float y) { cam_x = x; cam_y = y; }
};
