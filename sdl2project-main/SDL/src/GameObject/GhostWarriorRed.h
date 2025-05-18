#pragma once

#include "SpriteAnimation.h"
#include <memory>
#include <functional>

enum class GhostWarriorRedState
{
    Patrol,
    Chase,
    Attack,
    Hit,
    Dead
};

class GhostWarriorRed
{
public:
    GhostWarriorRed();
    ~GhostWarriorRed();

    void Init(float pointA, float pointB, float posY);
    void Update(float deltaTime);
    void Draw(SDL_Renderer* renderer);

    void SetGroundCollider(SDL_Rect* ground);
    void SetPlayerCollider(const SDL_Rect* player);
    SDL_Rect* GetCollider();

    void TakeDamage(int damage, int knockbackDir);
    void SetOnHitPlayerCallback(std::function<void(int damage)> callback) { m_onHitPlayer = callback; }

    void SetOnDeathCallback(std::function<void()> callback);
    void SetSpawnExpCallback(std::function<void(Vector2)> callback);
private:
    // === Core ===
    std::shared_ptr<SpriteAnimation> m_sprite;
    SDL_Rect m_collider;
    SDL_Rect* m_groundCollider = nullptr;
    const SDL_Rect* m_playerCollider = nullptr;

    float m_exp_orb_number = 3;

    float m_pointA = 0.0f;
    float m_pointB = 0.0f;

    float m_velocityY = 0.0f;
    float m_gravityForce = 150.0f;
    float m_speed = 200.0f;
    float m_airTime = 0.0f;
    int m_direction = 1;
    SDL_RendererFlip m_flip = SDL_FLIP_NONE;
    SDL_RendererFlip m_attackFlip = SDL_FLIP_NONE;
    std::function<void()> m_onDeathCallback;
    std::function<void(Vector2)> m_spawnExpCallback;

    bool m_isGrounded = false;

    // === State Machine ===
    GhostWarriorRedState m_state = GhostWarriorRedState::Patrol;
    bool m_isDead = false;
    bool m_isHit = false;
    bool m_isAttacking = false;
    float m_hitTimer = 0.0f;
    float m_attackTimer = 0.0f;
    float m_attackCooldown = 0.0f;
    float m_lostPlayerTimer = 0.0f;

    // === Combat ===
    int m_maxHP = 100;
    int m_currentHP = 100;
    int m_damage = 30;
    std::function<void(int damage)> m_onHitPlayer;

    // === Animation Resources ===
    std::shared_ptr<TextureManager> idleTexture;
    std::shared_ptr<TextureManager> runTexture;
    std::shared_ptr<TextureManager> attack1Texture;
    std::shared_ptr<TextureManager> attack2Texture;
    std::shared_ptr<TextureManager> hitTexture;
    std::shared_ptr<TextureManager> deadTexture;

    // === Attack ===
    SDL_Rect m_attackBox;
    bool m_didDamage = false;

    // === Health Bar ===
    SDL_Rect m_hpBarBg;
    SDL_Rect m_hpBarFill;
    int m_barWidth = 50;
    int m_barHeight = 6;

    // === Death ===
    float m_deadTimer = 0.0f;
    bool m_finishDead = false;

    // === Debug Ray ===
    bool m_drawRay = false;
    Vector2 m_lastRayStart;
    Vector2 m_lastRayEnd;

    bool m_recentlyReturnedFromChase = false;
    float m_recentlyReturnedTimer = 0.0f;

    //KnockBack
    bool m_isKnockback = false;
    float m_knockbackTimer = 0.0f;
    float m_knockbackDuration = 0.2f;
    float m_knockbackSpeed = 350.0f;
    int m_knockbackDir = 0; // -1 trái, 1 phải

private:
    bool DetectPlayer();
    void ChangeAnimation(std::shared_ptr<TextureManager> texture, int frameCount, float frameTime);
    bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b);
    bool GroundCheck();
    void Die();
    void Destroy();
};
