#include "Bullet.h"
#include "ResourceManagers.h"
#include "Camera.h"
#include "Renderer.h"
#include <iostream>

Bullet::Bullet(Vector2 position, float speed, int direction, int damage)
    : m_speed(speed), m_direction(direction), m_damage(damage), m_destroyed(false)
{
    auto bulletTexture = ResourceManagers::GetInstance()->GetTexture("projectile.png");
    m_sprite = std::make_shared<SpriteAnimation>(bulletTexture, 1, 1, 1, 0.1f);
    m_sprite->SetSize(128, 128);
    m_sprite->Set2DPosition(position.x, position.y);
    m_sprite->SetFlip((direction == 1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    m_collider = { (int)position.x, (int)position.y, 64, 64 };
}

void Bullet::Update(float dt)
{
    if (m_destroyed) return;

    m_lifeTime += dt;
    if (m_lifeTime >= 2) {
        m_destroyed = true;
        return;
    }

    Vector2 pos = m_sprite->Get2DPosition();
    pos.x += m_speed * m_direction * dt;
    m_sprite->Set2DPosition(pos.x, pos.y);
    m_collider.x = (int)pos.x;
    m_collider.y = (int)pos.y;

    m_sprite->Update(dt);
}


void Bullet::Draw(SDL_Renderer* renderer)
{
    if (!m_destroyed && m_sprite)
        m_sprite->Draw(renderer);
}

SDL_Rect* Bullet::GetCollider()
{
    return &m_collider;
}

bool Bullet::IsOutOfScreen()
{
    return m_destroyed;
}

bool Bullet::HasHitPlayer(const SDL_Rect* playerCollider)
{
    if (!playerCollider || m_destroyed) return false;

    if (SDL_HasIntersection(&m_collider, playerCollider)) {
        if (m_onHitPlayer) {
            m_onHitPlayer(m_damage);
        }
        m_destroyed = true;

        return true;
    }

    return false;
}
