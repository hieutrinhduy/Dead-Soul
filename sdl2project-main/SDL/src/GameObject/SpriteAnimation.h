#pragma once

#include "BaseObject.h"
#include <SDL_render.h>

class TextureManager;

class SpriteAnimation : public BaseObject
{
protected:
	int	m_currentAction; // start from 1
	int	m_frameCount;    // start from 1
	int m_numAction;     // start from 1
	float m_frameTime;

	int	m_currentFrame;
	float  m_currentTicks;
	Uint32 m_lastUpdate;

	int			m_iHeight;
	int			m_iWidth;
	float		m_numFrames;
	float		m_currentTime;
	int			m_animSpeed;


public:
	SpriteAnimation(std::shared_ptr<TextureManager> texture, int currentAction, int frameCount, int numAction, float  frameTime);
	~SpriteAnimation();

	void Init() override;
	void Draw(SDL_Renderer* renderer, SDL_Rect* clip = NULL) override;
	void Update(float deltatime) override;

	// GETTER:
	Vector2	Get2DPosition();
	int GetCurrentAction();
	int GetWidth();
	int GetHeight();
	int GetCurrentFrame();

	// SETTER:
	void Set2DPosition(float x, float y);
	void SetRotation(double angle);
	void SetFlip(SDL_RendererFlip flip);
	void SetSize(int width, int height);
	void SetCurrentAction(int action);
	void SetFrameTime(float time);
};