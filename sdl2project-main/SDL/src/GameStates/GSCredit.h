#pragma once

#include "GameStateBase.h"

#include "Text.h"
#include "MouseButton.h"


class GSCredit : public GameStateBase
{
public:
	GSCredit();
	~GSCredit();


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
	std::shared_ptr<Sprite2D> sceneBackground;
	std::shared_ptr<Text> narration1, narration2;
	std::list<std::shared_ptr<MouseButton>>	m_listButton;

	SDL_Color narrationColor;
	float widthScaling, heightScaling;
};
