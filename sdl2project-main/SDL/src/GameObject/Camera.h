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

	// SETTER:
	void SetTarget(std::shared_ptr<BaseObject> target);
	void SetLevelDimension(int width, int height);
	void ResetCameraPosition();

	void Update(float deltaTime);
	void Shake(float duration, float magnitude);


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
};