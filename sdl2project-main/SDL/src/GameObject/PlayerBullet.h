#pragma once
#include "SpriteAnimation.h"
#include <functional>
#include <memory>
#include <SDL.h>

class PlayerBullet
{
public:
    PlayerBullet(Vector2 position, float speed, int direction, int damage);

    void Update(float dt);
    void Draw(SDL_Renderer* renderer);

    SDL_Rect* GetCollider();
    bool IsOutOfScreen();
    bool HasHitEnemy(const SDL_Rect* enemyCollider);

    void SetOnHitEnemyCallback(std::function<void(int)> callback) { m_onHitEnemy = callback; }
    int GetDamage() { return m_damage; }
    void SetOnScreenShakeCallback(std::function<void()> callback) { m_onScreenShake = callback; }
private:
    std::shared_ptr<SpriteAnimation> m_sprite;
    SDL_Rect m_collider;
    float m_speed;
    int m_direction; // 1 = phải, -1 = trái
    int m_damage = 10;
    bool m_destroyed;
    std::function<void()> m_onScreenShake;
    std::function<void(int)> m_onHitEnemy;

    float m_velocityY = 0.0f;
    float m_gravityForce = 600.0f;
};
