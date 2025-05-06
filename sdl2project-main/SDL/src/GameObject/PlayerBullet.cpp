#include "PlayerBullet.h"
#include "ResourceManagers.h"
#include "Camera.h"
#include "Renderer.h"
#include <iostream>

PlayerBullet::PlayerBullet(Vector2 position, float speed, int direction, int damage)
    : m_speed(speed), m_direction(direction), m_damage(damage), m_destroyed(false)
{
    auto bulletTexture = ResourceManagers::GetInstance()->GetTexture("projectile.png");
    m_sprite = std::make_shared<SpriteAnimation>(bulletTexture, 1, 1, 1, 0.1f);
    m_sprite->SetSize(128, 128);
    m_sprite->Set2DPosition(position.x, position.y);
    m_sprite->SetFlip((direction == -1) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);

    m_collider = { (int)position.x, (int)position.y, 128, 128 };

    std::cout << "New Bullet" << std::endl; 
    m_damage = 10;
}

void PlayerBullet::Update(float dt)
{
    if (m_destroyed) return;

    Vector2 pos = m_sprite->Get2DPosition();
    pos.x += m_speed * m_direction * dt;
    m_sprite->Set2DPosition(pos.x, pos.y);

    m_collider.x = (int)pos.x;
    m_collider.y = (int)pos.y;

    m_sprite->Update(dt);
}

void PlayerBullet::Draw(SDL_Renderer* renderer)
{
    if (!m_destroyed && m_sprite)
        m_sprite->Draw(renderer);
}

SDL_Rect* PlayerBullet::GetCollider()
{
    return &m_collider;
}

bool PlayerBullet::IsOutOfScreen()
{
    return false;
    //return m_destroyed || m_collider.x < -200 || m_collider.x > 2200; // giới hạn màn hình
}

bool PlayerBullet::HasHitEnemy(const SDL_Rect* enemyCollider)
{
    if (!enemyCollider || m_destroyed) return false;

    if (SDL_HasIntersection(&m_collider, enemyCollider)) {
        m_destroyed = true;

        if (m_onScreenShake)
            m_onScreenShake();

        if (m_onHitEnemy)
            m_onHitEnemy(m_damage);

        return true;
    }

    return false;
}

