#pragma once

#include <list>

#include "Sprite2D.h"
#include "SpriteAnimation.h"
#include "Text.h"
#include "PlayerController.h"
#include "TileMap.h"
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
	void SetReferenceToTileMap(std::shared_ptr<TileMap> tileMap) {
		m_tileMap = tileMap;
	}
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
	std::shared_ptr<MouseButton> btnPause;

	// UI WHEN PAUSE GAME:
	std::shared_ptr<Sprite2D> musicForegroundBar, sfxForegroundBar;
	std::shared_ptr<SliderIcon> iconMusicSettings, iconSfxSettings, iconSensitivitySettings;
	bool pressButtonWhenPause, releaseButtonWhenPause;

	// UI WHEN LEVEL UP:
	std::shared_ptr<Sprite2D> selectionBorder, hoverBorder, underline, confirmButton;
	int selection = 0;
	bool pressButtonWhenLevelUp = false;

	// UI WHEN CHANGE MAP:
	std::shared_ptr<SpriteAnimation> animationMapTransition;
	bool mapTransitionFlag = true, mapTransitionNextRow = false;

	std::list<std::shared_ptr<MouseButton>> buttons;
	std::list<std::shared_ptr<Sprite2D>> uiWhenRunGame;

	std::list<std::shared_ptr<SliderIcon>> sliderIcons;
	std::list<std::shared_ptr<Sprite2D>> uiWhenPause;

	std::list<std::shared_ptr<Sprite2D>> uiWhenLevelUp;

	

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
	std::shared_ptr<TileMap> m_tileMap;

	bool m_isPlaying;
	float timer, spTimer, mpTimer;
};