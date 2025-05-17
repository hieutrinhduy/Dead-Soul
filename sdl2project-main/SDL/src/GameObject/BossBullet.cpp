#include "BossBullet.h"
#include "ResourceManagers.h"
#include "Camera.h"
#include <cmath>
#include <iostream>

BossBullet::BossBullet(Vector2 position, Vector2 direction, float speed, int damage)
    : m_direction(direction), m_speed(speed), m_damage(damage), m_destroyed(false)
{
    auto bulletTexture = ResourceManagers::GetInstance()->GetTexture("projectile.png");
    m_sprite = std::make_shared<SpriteAnimation>(bulletTexture, 1, 1, 1, 0.1f);
    m_sprite->SetSize(128, 128);
    m_sprite->Set2DPosition(position.x, position.y);
    float angleDeg = atan2f(m_direction.y, m_direction.x) * 180.0f / M_PI + 180;
    m_sprite->SetRotation(angleDeg);

    m_collider = { (int)position.x, (int)position.y, 64, 64 };
}

void BossBullet::Update(float dt)
{
    if (m_destroyed) return;

    m_lifeTime += dt;
    if (m_lifeTime >= 2.0f) {
        m_destroyed = true;
        return;
    }

    Vector2 pos = m_sprite->Get2DPosition();
    pos.x += m_direction.x * m_speed * dt;
    pos.y += m_direction.y * m_speed * dt;

    m_sprite->Set2DPosition(pos.x, pos.y);
    m_collider.x = (int)pos.x;
    m_collider.y = (int)pos.y;

    m_sprite->Update(dt);
}

void BossBullet::Draw(SDL_Renderer* renderer)
{
    if (!m_destroyed && m_sprite)
        m_sprite->Draw(renderer);
}

SDL_Rect* BossBullet::GetCollider()
{
    return &m_collider;
}

bool BossBullet::HasHitPlayer(const SDL_Rect* playerCollider)
{
    if (!playerCollider || m_destroyed) return false;

    if (SDL_HasIntersection(&m_collider, playerCollider)) {
        if (m_onHitPlayer) m_onHitPlayer(m_damage);
        m_destroyed = true;
        return true;
    }

    return false;
}
