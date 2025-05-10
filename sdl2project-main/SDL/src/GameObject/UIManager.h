#pragma once

#include <list>

#include "Sprite2D.h"
#include "Text.h"
#include "PlayerController.h"


class UIManager {
public:
	UIManager(std::shared_ptr<int> timeScale);
	~UIManager();

	void Update(float deltaTime);
	void Draw(SDL_Renderer* renderer);

	// SETTER:
	void SetReferenceToPlayerController(std::shared_ptr<PlayerController> playerController) {
		m_playerController = playerController;
	}
	void ResetTimer() { timer = 0; };

	// sub-thread:
	void UpdateStats();
	void ClearThread();
	bool m_finishThread;

private:
	#pragma region UI
	std::shared_ptr<Sprite2D> loadingBackground;

	std::shared_ptr<Sprite2D> hpBar, spBar, mpBar, expBar;
	std::shared_ptr<Sprite2D> hpBgBar, spBgBar, mpBgBar, expBgBar;
	float fullHpBarWidth, fullSpBarWidth, fullMpBarWidth;

	std::shared_ptr<Sprite2D> iconCoin;

	std::list<std::shared_ptr<Sprite2D>> items;
	std::list<std::shared_ptr<Sprite2D>> itemBgs;
	int m_inventorySlot;

	std::shared_ptr<Text> txtLoading, txtHP, txtSP, txtMP, txtEXP, txtCoin;

	std::list<std::shared_ptr<Sprite2D>> uiElements;
	std::list<std::shared_ptr<Text>> texts;

	SDL_Color hpTextColor, expTextColor;
	#pragma endregion

	SDL_Renderer* m_renderer;
	std::shared_ptr<int> m_timeScale;
	std::shared_ptr<PlayerController> m_playerController;

	bool m_isPlaying;
	float timer;
};