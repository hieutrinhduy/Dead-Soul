#include "PlayerBullet.h"
#include "ResourceManagers.h"
#include "Camera.h"
#include "Renderer.h"
#include <iostream>
#include "Sound.h"

PlayerBullet::PlayerBullet(Vector2 position, float speed, int direction, int damage)
    : m_speed(speed), m_direction(direction), m_damage(damage), m_destroyed(false)
{
    auto bulletTexture = ResourceManagers::GetInstance()->GetTexture("projectile.png");
    m_sprite = std::make_shared<SpriteAnimation>(bulletTexture, 1, 1, 1, 0.1f);
    m_sprite->SetSize(128, 128);
    m_sprite->Set2DPosition(position.x, position.y);
    m_sprite->SetFlip((direction == -1) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);

    m_collider = { (int)position.x, (int)position.y, 128, 128 };

    m_damage = damage;
    Sound::GetInstance()->LoadSfx("sfx_bow_hit.wav");
}

void PlayerBullet::Update(float dt)
{
    if (m_destroyed) return;

    m_lifeTime += dt;
    if (m_lifeTime >= 2.0f) {
        m_destroyed = true;
        return;
    }

    Vector2 pos = m_sprite->Get2DPosition();

    pos.x += m_speed * m_direction * dt;

    m_velocityY += m_gravityForce * dt;
    pos.y += m_velocityY * dt;

    m_sprite->Set2DPosition(pos.x, pos.y);
    m_collider.x = static_cast<int>(pos.x);
    m_collider.y = static_cast<int>(pos.y);
    float angle = atan2(m_velocityY, m_speed * m_direction);
    if (m_direction == -1) {
        m_sprite->SetRotation(angle * 180  / M_PI + 180);  
    }
    else
    {
        m_sprite->SetRotation(angle * 180 / M_PI);
    }
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

    if (!enemyCollider || m_destroyed) {
        return false;
    } 

    if (SDL_HasIntersection(&m_collider, enemyCollider)) {
        m_destroyed = true;
        Sound::GetInstance()->PlaySfx("sfx_bow_hit.wav");
        if (m_onScreenShake)
            m_onScreenShake();

        if (m_onHitEnemy)
            m_onHitEnemy(m_damage);

        return true;
    }

    return false;
}

