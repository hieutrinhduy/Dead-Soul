#pragma once

#include "GameStateBase.h"
#include "TileMap.h"

#include "UIManager.h"
#include "PlayerController.h" 
#include "EnemyController.h"
#include "EvilCreature.h"
#include "GhostWarrior.h"
#include "GhostWarriorRed.h"
#include "RangedEnemy.h"
#include "Boss1.h"
#include "SecretBoss.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "SecretLevel.h"
#include "ChangeLevelGate.h"
#include "SecretGate.h"
#include "Shop.h"

class Sprite2D;
class SpriteAnimation;
class MouseButton;
class Sound;

enum class LevelID {
	Level1,
	Level2,
	Level3,
	SecretLevel
};

class GSPlay : public GameStateBase
{
public:
	GSPlay();
	~GSPlay();

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

	// sub-thread:
	void DisplayLoadingView();

	int m_KeyPress;
	bool playerIsAppear = true;
	void ChangeLevel(LevelID newLevel);

	Vector2 GetGateSize() { return m_ChangeLevelGate->GetGateSize(); };
	Vector2 GetGatePos() { return m_ChangeLevelGate->GetGatePos(); };

	Vector2 GetSecretGateSize() 
	{ 
		if(m_SecretGate)
			return m_SecretGate->GetGateSize();
		else
		{
			std::cout << "This Level Doesnt have Secret Gate" << std::endl;
		}
	}
	Vector2 GetSecretGatePos() { 
		if (m_SecretGate) {
			return m_SecretGate->GetGatePos();
		}
		else
		{
			std::cout << "This Level Doesnt have Secret Gate" << std::endl;
		}
	}

	Vector2 GetFinalGatePos(){ return m_ChangeLevelGate->GetFinalGatePos(); }
	Vector2 GetFinalSecretGatePos() { return m_SecretGate->GetFinalGatePos(); }

	Vector2 GetShopSize() { return m_shop->GetShopSize(); };
	Vector2 GetShopPos() { return m_shop->GetShopPos(); };

private:
	std::shared_ptr<int> timeScale;

	std::shared_ptr<UIManager> m_uiManager;
	std::shared_ptr<PlayerController> m_playerController;

	std::shared_ptr<Boss1> m_boss1;
	std::shared_ptr<ScecretBoss> m_boss2;
	std::shared_ptr<EnemyController> m_enemyController;
	std::shared_ptr<EnemyController> m_enemyController2;
	std::shared_ptr<EnemyController> m_enemyController3;
	std::shared_ptr<EvilCreature> m_evilCreature;
	std::shared_ptr<GhostWarrior> m_ghostWarrior;
	std::shared_ptr<GhostWarriorRed> m_ghostWarriorRed;
	std::shared_ptr<RangedEnemy> m_rangedEnemy;

	std::shared_ptr<ChangeLevelGate> m_ChangeLevelGate;
	std::shared_ptr<SecretGate> m_SecretGate;
	std::shared_ptr<Shop> m_shop;

	std::shared_ptr<Sprite2D>	m_background;
	std::list<std::shared_ptr<MouseButton>>	m_listButton;
	std::list<std::shared_ptr<SpriteAnimation>>	m_listAnimation;
	std::shared_ptr<SpriteAnimation> obj;
	std::shared_ptr<SpriteAnimation> enemy;
	std::shared_ptr<MouseButton> button;
	std::shared_ptr<TileMap> m_tileMap;
	std::vector<SDL_Rect*> enemyColliders;
	bool m_tileMapLoaded = false;

	float time = 0.0f;
	float m_Velocity = 10.0f;

	std::vector<std::function<void(int)>> m_hitEnemyCallbacks; //Luu hit callback cua tung enemy

	std::shared_ptr<Level1> m_level1;
	std::shared_ptr<Level2> m_level2;
	std::shared_ptr<Level3> m_level3;
	std::shared_ptr<SecretLevel> m_secretLevel;
	LevelID m_currentLevel;

	// === Thêm 5 hàm private mới cho quản lý Update ===
	void UpdateAttackTimer(float deltaTime);
	void UpdateMovement(float deltaTime);
	void UpdateGravity(float deltaTime);
	void UpdateGroundCheck();
	void UpdateAnimation(float deltaTime);
};
