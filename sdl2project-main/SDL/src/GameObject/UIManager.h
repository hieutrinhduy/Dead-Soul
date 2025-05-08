#pragma once

#include <list>

#include "Sprite2D.h"
#include "Text.h"
#include "PlayerController.h"


class UIManager {
public:
	UIManager();
	~UIManager();

	void Draw(SDL_Renderer* renderer);

	// SETTER:
	void SetReferenceToPlayerController(std::shared_ptr<PlayerController> playerController) {
		m_playerController = playerController;
	}


private:
	std::shared_ptr<Sprite2D> hpBar, spBar, mpBar, expBar;
	std::shared_ptr<Sprite2D> hpBgBar, spBgBar, mpBgBar;
	float fullHpBarWidth, fullSpBarWidth, fullMpBarWidth;

	std::shared_ptr<Sprite2D> iconCoin;

	std::list<std::shared_ptr<Sprite2D>> items;
	std::list<std::shared_ptr<Sprite2D>> itemBgs;
	int m_inventorySlot;

	std::shared_ptr<Text> txtHP, txtSP, txtMP, txtEXP, txtCoin;

	std::list<std::shared_ptr<Sprite2D>> uiElements;
	std::list<std::shared_ptr<Text>> texts;

	std::shared_ptr<PlayerController> m_playerController;
};