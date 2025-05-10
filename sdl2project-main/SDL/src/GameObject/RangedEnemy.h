// RangedEnemy.h
#pragma once

#include "SpriteAnimation.h"
#include <memory>
#include <vector>
#include <functional>
#include "PlayerController.h"

enum class RangedEnemyState
{
    Patrol,
    Chase,
    Attack,
    Hit,
    Dead
};

class RangedEnemy
{
public:
    RangedEnemy();
    ~RangedEnemy();

    void Init(float pointA, float pointB, float posY);
    void Update(float dt);
    void Draw(SDL_Renderer* renderer);

    void SetGroundCollider(SDL_Rect* ground);
    void SetPlayerCollider(const SDL_Rect* player);
    SDL_Rect* GetCollider();

    void TakeDamage(int damage);
    void SetOnHitPlayerCallback(std::function<void(int damage)> callback) { m_onHitPlayer = callback; }
    void SetPlayerController(PlayerController* player) { m_playerController = player; }

private:
    PlayerController* m_playerController = nullptr;
    std::shared_ptr<SpriteAnimation> m_sprite;
    SDL_Rect m_collider;
    SDL_Rect* m_groundCollider = nullptr;
    const SDL_Rect* m_playerCollider = nullptr;

    float m_pointA = 0.0f;
    float m_pointB = 0.0f;
    float m_velocityY = 0.0f;
    float m_gravityForce = 150.0f;
    float m_speed = 200.0f;
    float m_airTime = 0.0f;
    int m_direction = 1;

    SDL_RendererFlip m_flip = SDL_FLIP_NONE;
    SDL_RendererFlip m_attackFlip = SDL_FLIP_NONE;

    RangedEnemyState m_state = RangedEnemyState::Patrol;
    bool m_isDead = false;
    bool m_isHit = false;
    bool m_isAttacking = false;
    float m_hitTimer = 0.0f;
    float m_attackTimer = 0.0f;
    float m_attackCooldown = 0.0f;
    float m_lostPlayerTimer = 0.0f;

    int m_maxHP = 100;
    int m_currentHP = 100;
    int m_damage = 1;
    std::function<void(int damage)> m_onHitPlayer;

    std::shared_ptr<TextureManager> idleTexture;
    std::shared_ptr<TextureManager> runTexture;
    std::shared_ptr<TextureManager> attackTexture;
    std::shared_ptr<TextureManager> hitTexture;
    std::shared_ptr<TextureManager> deadTexture;
    std::shared_ptr<TextureManager> bulletTexture;

    SDL_Rect m_hpBarBg;
    SDL_Rect m_hpBarFill;
    int m_barWidth = 50;
    int m_barHeight = 6;

    float m_deadTimer = 0.0f;
    bool m_finishDead = false;

    bool m_didFire = false;

    float m_attackRange = 500;
    bool m_didDamage = false;
    bool m_isGrounded = false;
    float m_detectRange = 700;

    bool m_recentlyReturnedFromChase = false;
    float m_recentlyReturnedTimer = 0.0f;

private:
    void ShootProjectile();
    bool DetectPlayer();
    void ChangeAnimation(std::shared_ptr<TextureManager> texture, int frameCount, float frameTime);
    bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b);
    bool GroundCheck();
    void Die(); 
    void Destroy();
};
