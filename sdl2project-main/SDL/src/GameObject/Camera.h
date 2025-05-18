#pragma once
#include "CMath.h"
#include "SDL.h"

#include "Define.h"
#include"BaseObject.h"

class Camera : public CSingleton<Camera>
{
public:
	Camera() { m_ViewBox = { 0, 0, SCREEN_WIDTH, SCREEN_HEIDHT }; }

	// GETTER:
	SDL_Rect GetViewBox();
	Vector2 GetPosition();
	int GetLevelWidth();
	int GetLevelHeight();
	bool GetLerpingRoutineState(){ return m_isLerpingRoutine; };

	// SETTER:
	void SetTarget(std::shared_ptr<BaseObject> target);
	void SetLevelDimension(int width, int height);
	void ResetCameraPosition();

	void Update(float deltaTime);
	void Shake(float duration, float magnitude);

	void LerpToPosition(float targetX, float targetY, float duration);
	void SetManualPosition(float centerX, float centerY);
	bool IsLerping() const;

	void SetLerpingRoutineState(bool isLerping) {
		m_isLerpingRoutine = isLerping;
	}
private:
	std::shared_ptr<BaseObject> m_target;
	Vector2 m_Position;
	SDL_Rect m_ViewBox;
	float m_Smooth = 5.0f;
	int m_levelWidth;
	int m_levelHeight;

	float m_shakeTimer = 0.0f;
	float m_shakeMagnitude = 0.0f;
	Vector2 m_originalPosition;

	// Tween camera đến vị trí cụ thể
	bool m_isLerpingToGate = false;
	float m_lerpStartX, m_lerpStartY;
	float m_lerpTargetX, m_lerpTargetY;
	float m_lerpTime = 0.0f;
	float m_lerpDuration = 1.0f;

	bool m_isLerpingRoutine = false;
};