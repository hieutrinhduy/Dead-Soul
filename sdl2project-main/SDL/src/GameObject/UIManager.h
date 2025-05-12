#pragma once

#include <list>

#include "Sprite2D.h"
#include "Text.h"
#include "PlayerController.h"
#include "MouseButton.h"
#include "SliderIcon.h"


class UIManager {
public:
	UIManager(std::shared_ptr<int> timeScale);
	~UIManager();

	void HandleTouchEvents(SDL_Event& e);
	void HandleMouseEvents(SDL_Event& e);

	void Update(float deltaTime);
	void Draw(SDL_Renderer* renderer);

	// SETTER:
	void SetReferenceToPlayerController(std::shared_ptr<PlayerController> playerController) {
		m_playerController = playerController;
	};
	void ResetTimer() { timer = 0; };

	// sub-thread:
	void UpdateStats();
	void ClearThread();
	bool m_finishThread;

	bool FloatEquals(float a, float b, float epsilon = 1e-5f);


private:
	#pragma region UI
	// UI WHEN LOAD GAME:
	std::shared_ptr<Sprite2D> loadingBackground;
	std::shared_ptr<Text> txtLoading;

	// UI WHEN RUN GAME:
	std::shared_ptr<Sprite2D> hpBar, spBar, mpBar, expBar;
	std::shared_ptr<Sprite2D> hpBgBar, spBgBar, mpBgBar, expBgBar;
	float fullHpBarWidth, fullSpBarWidth, fullMpBarWidth;
	std::shared_ptr<Sprite2D> iconCoin;
	std::shared_ptr<Sprite2D> selectedSlot;
	int m_inventorySlot;
	std::shared_ptr<Text> txtHP, txtSP, txtMP, txtEXP, txtCoin;

	// UI WHEN PAUSE GAME:
	std::shared_ptr<Sprite2D> musicForegroundBar, sfxForegroundBar;
	std::shared_ptr<SliderIcon> iconMusicSettings, iconSfxSettings, iconSensitivitySettings;
	bool pressButtonWhenPause, releaseButtonWhenPause;

	std::list<std::shared_ptr<MouseButton>> buttons;
	std::list<std::shared_ptr<Sprite2D>> uiWhenRunGame;

	std::list<std::shared_ptr<SliderIcon>> sliderIcons;
	std::list<std::shared_ptr<Sprite2D>> uiWhenPause;

	SDL_Color hpTextColor, expTextColor;
	#pragma endregion

	// Player's Stats:
	float hp, maxHP, sp, maxSP, mp, maxMP, exp, maxEXP;
	float hpLoss, spLoss, mpLoss, expGained;
	float hpChangePerFrame, spChangePerFrame, mpChangePerFrame, expChangePerFrame;
	std::string hpStat, spStat, mpStat, expStat;


	int mousewheelSensitivity, changeInWheelValue;


	SDL_Renderer* m_renderer;
	std::shared_ptr<int> m_timeScale;
	std::shared_ptr<PlayerController> m_playerController;

	bool m_isPlaying;
	float timer, spTimer, mpTimer;
};