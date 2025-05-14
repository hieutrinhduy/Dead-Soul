#pragma once
#include "SpriteAnimation.h"
#include <memory>
#include <functional>

enum class SecretBossState
{
    Patrol,
    Chase,
    Attack,
    Hit,
    Dead
};

enum class BulletPatternType {
    Circle,
    Spiral,
    Wave,
    Random,
    CrossX
};

class ScecretBoss
{
public:
    ScecretBoss();
    ~ScecretBoss();

    void Init(float pointA, float pointB, float posY);
    void Update(float deltaTime);
    void Draw(SDL_Renderer* renderer);

    void SetGroundCollider(SDL_Rect* ground);
    void SetPlayerCollider(const SDL_Rect* player);
    SDL_Rect* GetEnemyCollider();

    void TakeDamage(int damage);

    // Gọi callback khi tấn công trúng Player
    void SetOnHitPlayerCallback(std::function<void(int damage)> callback) { m_onHitPlayer = callback; }

    void ShootInAngle(float angleRad);
    void FireBulletPattern(int waveIndex);

    void SetOnDeathCallback(std::function<void()> callback);
    void SetSpawnExpCallback(std::function<void(Vector2)> callback);

private:
    // === Core ===
    std::shared_ptr<SpriteAnimation> m_enemy;
    SDL_Rect m_enemyCollider;
    SDL_Rect* m_groundCollider;
    const SDL_Rect* m_playerCollider;
    std::function<void()> m_onDeathCallback;
    std::function<void(Vector2)> m_spawnExpCallback;

    float m_exp_orb_number = 15;

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
    SecretBossState m_currentState;
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
    std::shared_ptr<TextureManager> flyTexture;
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

    bool m_recentlyReturnedFromChase = false;
    float m_recentlyReturnedTimer = 0.0f;

    //Variable for fly
    bool m_isFlying = false;
    bool m_shootingStarted = false;
    float m_flySpeed = -400.0f;
    float m_shootTimer = 0.0f;
    int m_shotsFired = 0;
    const int m_totalShots = 6;
    float m_originalY = 0.0f;
    bool m_returning = false;
    bool m_shootPhase2 = false;// đợt bắn thứ 2
    float m_betweenShotsDelay = 1;
    bool m_bulletHellStarted = false;
    float m_bulletHellTimer = 0.0f;
    int m_bulletHellWaves = 0;
    const int m_maxBulletHellWaves = 5;
    BulletPatternType m_currentPattern = BulletPatternType::Circle;

private:
    bool DetectPlayer();
    void ChangeAnimation(std::shared_ptr<TextureManager> texture, int frameCount, float frameTime);
    bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b);
    bool GroundCheck();
    void Die();
    void DestroyEnemy();

};
