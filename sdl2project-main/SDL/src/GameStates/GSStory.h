#pragma once
#include "GameStateBase.h"
#include "GameObject/Text.h"
class Sprite2D;
class SpriteAnimation;
class MouseButton;
class Sound;
class GSStory : public GameStateBase
{
public:
	GSStory();
	~GSStory();

	void	Init() override;
	void	Exit() override;

	void	Pause() override;
	void	Resume() override;

	void	HandleEvents() override;
	void	HandleKeyEvents(SDL_Event& e) override;
	void	HandleTouchEvents(SDL_Event& e) override;
	void	HandleMouseMoveEvents(SDL_Event& e) override;
	void	Update(float deltaTime) override;
	void	Draw(SDL_Renderer* renderer) override;

private:
	std::shared_ptr<SpriteAnimation> projector;
	std::shared_ptr<Text> m_textGameName;
	SDL_Color m_textColor;
	float	m_time;
	bool sentence1, sentence2;
};