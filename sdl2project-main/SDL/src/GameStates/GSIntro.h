#pragma once

#include "GameStateBase.h"
#include "SpriteAnimation.h"

class GSIntro : public GameStateBase
{
public:
	GSIntro();
	~GSIntro();

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
	//Uint32 callback(Uint32 interval, void*param);
	std::shared_ptr<SpriteAnimation> intro;
	bool newRow;
};
