#include "ScreenTransition.h"

#include <algorithm>
#include <cmath>

#include "Game.h"
#include "Textures.h"

CScreenTransition::CScreenTransition()
{
	type = ScreenTransitionType::None;
	duration = 0;
	elapsed = 0;
	active = false;
	blockInput = false;
	onFinished = nullptr;
}

float CScreenTransition::GetSteppedProgress(DWORD elapsedMs, DWORD durationMs, int steps) const
{
	if (durationMs == 0) return 1.0f;

	float progress = (float)elapsedMs / (float)durationMs;
	if (progress < 0.0f) progress = 0.0f;
	if (progress > 1.0f) progress = 1.0f;

	if (steps <= 0) return progress;
	if (progress >= 1.0f) return 1.0f;

	return floorf(progress * (float)steps) / (float)steps;
}

void CScreenTransition::Start(ScreenTransitionType transitionType, DWORD durationMs, bool shouldBlockInput, std::function<void()> finishedCallback)
{
	type = transitionType;
	duration = durationMs;
	elapsed = 0;
	active = true;
	blockInput = shouldBlockInput;
	onFinished = finishedCallback;
}

void CScreenTransition::StartFadeIn(DWORD durationMs, bool shouldBlockInput, std::function<void()> finishedCallback)
{
	Start(ScreenTransitionType::FadeIn, durationMs, shouldBlockInput, finishedCallback);
}

void CScreenTransition::StartFadeOut(DWORD durationMs, bool shouldBlockInput, std::function<void()> finishedCallback)
{
	Start(ScreenTransitionType::FadeOut, durationMs, shouldBlockInput, finishedCallback);
}

void CScreenTransition::StartIrisClose(bool shouldBlockInput, std::function<void()> finishedCallback)
{
	Start(
		ScreenTransitionType::IrisClose,
		TRANSITION_IRIS_CLOSE_DURATION_MS + TRANSITION_IRIS_HUD_WIPE_DURATION_MS,
		shouldBlockInput,
		finishedCallback);
}

void CScreenTransition::Update(DWORD dt)
{
	if (!active) return;

	elapsed += dt;
	if (elapsed < duration) return;

	std::function<void()> finishedCallback = onFinished;

	type = ScreenTransitionType::None;
	duration = 0;
	elapsed = 0;
	active = false;
	blockInput = false;
	onFinished = nullptr;

	if (finishedCallback != nullptr)
		finishedCallback();
}

void CScreenTransition::Render() const
{
	if (!active) return;

	switch (type)
	{
	case ScreenTransitionType::FadeIn:
	case ScreenTransitionType::FadeOut:
		RenderFade();
		break;
	case ScreenTransitionType::IrisClose:
		RenderIrisClose();
		break;
	default:
		break;
	}
}

void CScreenTransition::RenderFade() const
{
	CGame* game = CGame::GetInstance();
	float progress = GetSteppedProgress(elapsed, duration, TRANSITION_FADE_STEPS);
	float alpha = progress;

	if (type == ScreenTransitionType::FadeIn)
		alpha = 1.0f - progress;

	DrawBlackRect(
		game->GetBackBufferWidth() / 2.0f,
		game->GetBackBufferHeight() / 2.0f,
		(float)game->GetBackBufferWidth(),
		(float)game->GetBackBufferHeight(),
		alpha);
}

void CScreenTransition::RenderIrisClose() const
{
	CGame* game = CGame::GetInstance();
	float screenWidth = (float)game->GetBackBufferWidth();
	float screenHeight = (float)game->GetBackBufferHeight();
	float hudHeight = (float)TRANSITION_HUD_HEIGHT;
	if (hudHeight > screenHeight) hudHeight = screenHeight;

	float playfieldHeight = screenHeight - hudHeight;
	if (playfieldHeight < 0.0f) playfieldHeight = 0.0f;

	if (elapsed >= TRANSITION_IRIS_CLOSE_DURATION_MS)
	{
		DrawBlackRect(screenWidth / 2.0f, playfieldHeight / 2.0f, screenWidth, playfieldHeight);

		DWORD hudElapsed = elapsed - TRANSITION_IRIS_CLOSE_DURATION_MS;
		float hudProgress = GetSteppedProgress(hudElapsed, TRANSITION_IRIS_HUD_WIPE_DURATION_MS, TRANSITION_HUD_WIPE_STEPS);
		float coveredHudHeight = hudHeight * hudProgress;

		if (coveredHudHeight > 0.0f)
			DrawBlackRect(screenWidth / 2.0f, playfieldHeight + coveredHudHeight / 2.0f, screenWidth, coveredHudHeight);

		return;
	}

	float progress = GetSteppedProgress(elapsed, TRANSITION_IRIS_CLOSE_DURATION_MS, TRANSITION_IRIS_STEPS);
	if (progress >= 1.0f)
	{
		DrawBlackRect(screenWidth / 2.0f, playfieldHeight / 2.0f, screenWidth, playfieldHeight);
		return;
	}

	float visibleWidth = screenWidth * (1.0f - progress);
	float visibleHeight = playfieldHeight * (1.0f - progress);
	float centerX = screenWidth / 2.0f;
	float centerY = playfieldHeight / 2.0f;

	float visibleLeft = centerX - visibleWidth / 2.0f;
	float visibleTop = centerY - visibleHeight / 2.0f;
	float visibleRight = centerX + visibleWidth / 2.0f;
	float visibleBottom = centerY + visibleHeight / 2.0f;

	DrawBlackRect(screenWidth / 2.0f, visibleTop / 2.0f, screenWidth, visibleTop);
	DrawBlackRect(screenWidth / 2.0f, visibleBottom + (playfieldHeight - visibleBottom) / 2.0f, screenWidth, playfieldHeight - visibleBottom);
	DrawBlackRect(visibleLeft / 2.0f, centerY, visibleLeft, visibleHeight);
	DrawBlackRect(visibleRight + (screenWidth - visibleRight) / 2.0f, centerY, screenWidth - visibleRight, visibleHeight);
}

void CScreenTransition::DrawBlackRect(float centerX, float centerY, float width, float height, float alpha) const
{
	if (width <= 0.0f || height <= 0.0f || alpha <= 0.0f) return;

	CGame* game = CGame::GetInstance();
	LPTEXTURE blackTexture = CTextures::GetInstance()->Get(TRANSITION_BLACK_TEXTURE_ID);
	if (blackTexture == NULL) return;

	game->Draw(centerX, centerY, blackTexture, (RECT*)NULL, alpha, (int)ceilf(width), (int)ceilf(height));
}
