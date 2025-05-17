#pragma once
#include "SpriteAnimation.h"
#include "BaseObject.h"
#include <functional>

class BossBullet
{
public:
    BossBullet(Vector2 position, Vector2 direction, float speed, int damage);

    void Update(float dt);
    void Draw(SDL_Renderer* renderer);
    SDL_Rect* GetCollider();
    bool HasHitPlayer(const SDL_Rect* playerCollider);
    bool IsDestroyed() const { return m_destroyed; }

    void SetOnHitCallback(std::function<void(int)> callback) { m_onHitPlayer = callback; }

private:
    std::shared_ptr<SpriteAnimation> m_sprite;
    SDL_Rect m_collider;
    Vector2 m_direction;
    float m_speed;
    int m_damage;
    bool m_destroyed;
    float m_lifeTime = 0.0f;

    std::function<void(int)> m_onHitPlayer;
};
