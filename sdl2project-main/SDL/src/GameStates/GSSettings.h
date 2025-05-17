#pragma once

#include "GameStateBase.h"

#include "Text.h"
#include "MouseButton.h"
#include "SliderIcon.h"


class GSSettings : public GameStateBase
{
public:
	GSSettings();
	~GSSettings();


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
	std::list<std::shared_ptr<Sprite2D>> elements;
	std::shared_ptr<Sprite2D> musicForegroundBar, sfxForegroundBar;
	std::shared_ptr<SliderIcon> iconMusicSettings, iconSfxSettings, iconMouseSettings;

	std::list<std::shared_ptr<MouseButton>>	m_listButton;
	std::list< std::shared_ptr<SliderIcon>> m_listSliderIcon;

	float widthScaling, heightScaling;

	// 'milestone' is y-coordinate of vertical grid line of settings panel:
	int milestone, bar1Top, bar2Top;

	int musicVolume, sfxVolume, mousewheelSensitivity;
	bool releaseMouseButton = false;
	int fullVolumeBarWidth;

	// limit the movement of the icons along the X-axis
	int iconStartPoint, iconEndPoint;
};
