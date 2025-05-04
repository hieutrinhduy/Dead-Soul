#pragma once
#include "SpriteAnimation.h"
#include <memory>
#include <functional>

enum class EnemyState
{
    Patrol,
    Chase,
    Attack,
    Hit,
    Dead
};

class EnemyController
{
public:
    EnemyController();
    ~EnemyController();

    void Init(float pointA, float pointB);
    void Update(float deltaTime);
    void Draw(SDL_Renderer* renderer);

    void SetGroundCollider(SDL_Rect* ground);
    void SetPlayerCollider(const SDL_Rect* player);
    SDL_Rect* GetEnemyCollider();

    void TakeDamage(int damage);

    // Gọi callback khi tấn công trúng Player
    void SetOnHitPlayerCallback(std::function<void(int damage)> callback) { m_onHitPlayer = callback; }

private:
    // === Core ===
    std::shared_ptr<SpriteAnimation> m_enemy;
    SDL_Rect m_enemyCollider;
    SDL_Rect* m_groundCollider;
    const SDL_Rect* m_playerCollider;

    float m_pointA;
    float m_pointB;

    float m_velocityY;
    float m_gravityForce;
    float m_speed;
    float m_airTime;
    int m_direction;
    SDL_RendererFlip m_enemyFlip;
    SDL_RendererFlip m_attackFlip;

    bool m_isGrounded;

    // === State Machine ===
    EnemyState m_currentState;
    bool m_isDead;
    bool m_isHit;
    bool m_isAttacking;
    float m_hitTimer;
    float m_attackTimer;
    float m_attackCooldown;
    float m_lostPlayerTimer;

    // === Combat ===
    int m_maxHP;
    int m_currentHP;
    int m_damage;
    std::function<void(int damage)> m_onHitPlayer;

    // === Animation Resources ===
    std::shared_ptr<TextureManager> idleTexture;
    std::shared_ptr<TextureManager> runTexture;
    std::shared_ptr<TextureManager> attack1Texture;
    std::shared_ptr<TextureManager> attack2Texture;
    std::shared_ptr<TextureManager> hitTexture;
    std::shared_ptr<TextureManager> deadTexture;

    // === Debug Raycast ===
    bool m_drawRay;
    Vector2 m_lastRayStart;
    Vector2 m_lastRayEnd;

    float m_deadTimer;
    bool m_finishDead;

    SDL_Rect m_attackBox;
    bool m_didDamage = false;

    SDL_Rect m_hpBarBg;
    SDL_Rect m_hpBarFill;
    int m_barWidth = 50;
    int m_barHeight = 6;

private:
    bool DetectPlayer();
    void ChangeAnimation(std::shared_ptr<TextureManager> texture, int frameCount, float frameTime);
    bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b);
    bool GroundCheck();
    void Die();
    void DestroyEnemy();
};
