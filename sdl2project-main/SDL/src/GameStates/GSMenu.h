#pragma once

#include "GameStateBase.h"
#include "SpriteAnimation.h"
#include "Camera.h"
#include"Text.h"
#include "Sound.h"

class MouseButton;
class GSMenu : public GameStateBase
{
public:
	GSMenu();
	~GSMenu();

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
	std::shared_ptr<SpriteAnimation> m_background;
	std::list<std::shared_ptr<MouseButton>>	m_listButton;
	std::shared_ptr<Text> m_textGameName;
	std::shared_ptr<MouseButton> btnPlay, btnCredit, btnSettings, btnExit;
	
	float widthScaling, heightScaling;

	int buttonIndex;
	int btnPlayLeft, btnPlayRight, btnPlayTop, btnPlayBottom;
	int btnCreditLeft, btnCreditRight, btnCreditTop, btnCreditBottom;
	int btnSettingsLeft, btnSettingsRight, btnSettingsTop, btnSettingsBottom;
};