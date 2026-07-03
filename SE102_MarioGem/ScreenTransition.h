#pragma once

#include <Windows.h>
#include <functional>

constexpr int TRANSITION_BLACK_TEXTURE_ID = 31;

constexpr DWORD TRANSITION_FADE_IN_DURATION_MS = 450;
constexpr DWORD TRANSITION_FADE_OUT_DURATION_MS = 450;
constexpr int TRANSITION_FADE_STEPS = 8;

constexpr DWORD TRANSITION_IRIS_CLOSE_DURATION_MS = 650;
constexpr DWORD TRANSITION_IRIS_HUD_WIPE_DURATION_MS = 180;
constexpr int TRANSITION_IRIS_STEPS = 12;
constexpr int TRANSITION_HUD_WIPE_STEPS = 4;
constexpr int TRANSITION_HUD_HEIGHT = 32;

enum class ScreenTransitionType
{
	None,
	FadeIn,
	FadeOut,
	IrisClose
};

class CScreenTransition
{
	ScreenTransitionType type;
	DWORD duration;
	DWORD elapsed;
	bool active;
	bool blockInput;
	std::function<void()> onFinished;

	float GetSteppedProgress(DWORD elapsedMs, DWORD durationMs, int steps) const;
	void Start(ScreenTransitionType type, DWORD durationMs, bool blockInput, std::function<void()> onFinished);
	void RenderFade() const;
	void RenderIrisClose() const;
	void DrawBlackRect(float centerX, float centerY, float width, float height, float alpha = 1.0f) const;

public:
	CScreenTransition();

	void StartFadeIn(DWORD durationMs = TRANSITION_FADE_IN_DURATION_MS, bool blockInput = true, std::function<void()> onFinished = nullptr);
	void StartFadeOut(DWORD durationMs = TRANSITION_FADE_OUT_DURATION_MS, bool blockInput = true, std::function<void()> onFinished = nullptr);
	void StartIrisClose(bool blockInput = true, std::function<void()> onFinished = nullptr);

	void Update(DWORD dt);
	void Render() const;

	bool IsActive() const { return active; }
	bool IsBlockingInput() const { return active && blockInput; }
};
