//Enemy Bullet
#pragma once
#include "SpriteAnimation.h"
#include <memory>
#include <functional>

class Bullet
{
public:
    Bullet(Vector2 position, float speed, int direction, int damage);

    void Update(float dt);
    void Draw(SDL_Renderer* renderer);
    SDL_Rect* GetCollider();
    bool IsOutOfScreen();
    bool HasHitPlayer(const SDL_Rect* playerCollider);

    int GetDamage() const { return m_damage; }
    bool IsDestroyed() const { return m_destroyed; }

    void SetOnHitPlayerCallback(std::function<void(int damage)> callback) { m_onHitPlayer = callback; }
private:
    std::function<void(int damage)> m_onHitPlayer;
    std::shared_ptr<SpriteAnimation> m_sprite;
    SDL_Rect m_collider;
    float m_speed;
    int m_direction;
    int m_damage = 10;
    bool m_destroyed;
    float m_lifeTime = 0.0f;
};
