#pragma once
#include "GameStateBase.h"
#include "TileMap.h"
#include "PlayerController.h" 
#include "EnemyController.h"

class Sprite2D;
class SpriteAnimation;
class MouseButton;
class Sound;

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
	void	Draw(SDL_Renderer* renderer);

	int m_KeyPress;

private:
	std::shared_ptr<PlayerController> m_playerController;
	std::shared_ptr<EnemyController> m_enemyController;
	std::shared_ptr<EnemyController> m_enemyController2;
	std::shared_ptr<EnemyController> m_enemyController3;

	std::vector<std::shared_ptr<Sprite2D>> m_backgroundList;
	std::shared_ptr<Sprite2D>	m_background;
	std::list<std::shared_ptr<MouseButton>>	m_listButton;
	std::list<std::shared_ptr<SpriteAnimation>>	m_listAnimation;
	std::shared_ptr<SpriteAnimation> obj;
	std::shared_ptr<SpriteAnimation> enemy;
	std::shared_ptr<MouseButton> button;
	std::shared_ptr<TileMap> m_tileMap;
	bool m_tileMapLoaded = false;

	float time = 0.0f;
	float m_Velocity = 10.0f;

	// === Thêm 5 hàm private mới cho quản lý Update ===
	void UpdateAttackTimer(float deltaTime);
	void UpdateMovement(float deltaTime);
	void UpdateGravity(float deltaTime);
	void UpdateGroundCheck();
	void UpdateAnimation(float deltaTime);
};
