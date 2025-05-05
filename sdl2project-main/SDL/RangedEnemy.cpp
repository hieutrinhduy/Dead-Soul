#include "RangedEnemy.h"
#include "ResourceManagers.h"
#include "Camera.h"
#include "Renderer.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include "Bullet.h"

int rangedEnemyAttackIndex = 0;
extern std::vector<std::shared_ptr<Bullet>> g_bullets;

RangedEnemy::RangedEnemy() {}
RangedEnemy::~RangedEnemy() {}

void RangedEnemy::Init(float pointA, float pointB)
{
    idleTexture = ResourceManagers::GetInstance()->GetTexture("FlyEnemyIDLE.png");
    runTexture = ResourceManagers::GetInstance()->GetTexture("FlyEnemyFLYING.png");
    attackTexture = ResourceManagers::GetInstance()->GetTexture("FlyEnemyATTACK.png");
    hitTexture = ResourceManagers::GetInstance()->GetTexture("FlyEnemyHURT.png");
    deadTexture = ResourceManagers::GetInstance()->GetTexture("FlyEnemyDEATH.png");

    m_sprite = std::make_shared<SpriteAnimation>(runTexture, 1, 4, 1, 0.05f);
    m_sprite->SetSize(96 * 2, 64 * 2);
    m_sprite->Set2DPosition(pointA, -500);
    m_collider = { (int)m_sprite->GetPosition().x + 50, (int)m_sprite->GetPosition().y, m_sprite->GetWidth() / 2, m_sprite->GetHeight() };

    m_pointA = pointA;
    m_pointB = pointB;
    m_flip = SDL_FLIP_NONE;

    m_hpBarBg = { 0, 0, m_barWidth, m_barHeight };
    m_hpBarFill = m_hpBarBg;
}

void RangedEnemy::SetGroundCollider(SDL_Rect* ground) { m_groundCollider = ground; }
void RangedEnemy::SetPlayerCollider(const SDL_Rect* player) { m_playerCollider = player; }
SDL_Rect* RangedEnemy::GetCollider() { return &m_collider; }

void RangedEnemy::TakeDamage(int damage)
{
    if (m_isDead) return;
    m_currentHP -= damage;
    m_hpBarFill.w = static_cast<int>(m_barWidth * std::clamp((float)m_currentHP / m_maxHP, 0.0f, 1.0f));
    if (m_currentHP <= 0) { m_currentHP = 0; Die(); }
    else {
        m_isHit = true;
        m_hitTimer = 0.2f;
        ChangeAnimation(hitTexture, 4, 0.05f);
    }
}

void RangedEnemy::Die()
{
    m_isDead = true;
    m_deadTimer = 0.69f;
    m_finishDead = false;
    ChangeAnimation(deadTexture, 7, 0.1f);
}

void RangedEnemy::Destroy()
{
    m_collider = { 0, 0, 0, 0 };
    m_sprite = nullptr;
}

void RangedEnemy::Update(float dt)
{
    if (m_isDead) {
        if (!m_finishDead) {
            m_deadTimer -= dt;
            if (m_deadTimer <= 0.0f) { m_finishDead = true; Destroy(); }
        }
        if (m_sprite) m_sprite->Update(dt);
        return;
    }

    if (m_isHit) {
        m_hitTimer -= dt;
        if (m_hitTimer <= 0.0f) {
            m_isHit = false;
            ChangeAnimation(runTexture, 4, 0.05f);
        }
        m_sprite->Update(dt);
        return;
    }

    float posX = m_sprite->Get2DPosition().x;
    float posY = m_sprite->Get2DPosition().y;

    switch (m_state) {
    case RangedEnemyState::Patrol:
        if (posX < m_pointA) { m_direction = 1; m_flip = SDL_FLIP_HORIZONTAL; }
        else if (posX > m_pointB) { m_direction = -1; m_flip = SDL_FLIP_NONE; }
        posX += m_direction * m_speed * dt;
        if (m_direction == 1 && posX > m_pointB) { m_direction = -1; m_flip = SDL_FLIP_NONE; }
        else if (m_direction == -1 && posX < m_pointA) { m_direction = 1; m_flip = SDL_FLIP_HORIZONTAL; }
        if (DetectPlayer()) { m_state = RangedEnemyState::Chase; m_lostPlayerTimer = 0.0f; }
        break;
    case RangedEnemyState::Chase:
        if (!DetectPlayer()) {
            m_lostPlayerTimer += dt;
            if (m_lostPlayerTimer >= 0.1f) {
                m_state = RangedEnemyState::Patrol;
                ChangeAnimation(runTexture, 4, 0.05f);
                break;
            }
        }
        else m_lostPlayerTimer = 0.0f;

        if (m_playerCollider) {
            float dx = m_playerCollider->x - posX;
            m_flip = (dx > 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

            if (!m_isAttacking && abs(dx) <= m_attackRange) {
                m_state = RangedEnemyState::Attack;
                m_attackFlip = m_flip;
                m_isAttacking = true;
                ChangeAnimation(attackTexture, 8, 0.1f);
                m_attackTimer = 0.75f;
            }
        }
        break;
    case RangedEnemyState::Attack:
        m_attackTimer -= dt;
        if (m_sprite && !m_didDamage && m_sprite->GetCurrentFrame() == 5 && !m_isHit) {
            m_didDamage = true;
            ShootProjectile();
        }
        if (m_attackTimer <= 0.0f) {
            m_isAttacking = false;
            m_didDamage = false;
            m_state = DetectPlayer() ? RangedEnemyState::Chase : RangedEnemyState::Patrol;
            ChangeAnimation(runTexture, 4, 0.05f);
        }
        break;
    }

    m_sprite->Set2DPosition(posX, posY);
    m_sprite->SetFlip((m_state == RangedEnemyState::Attack) ? m_attackFlip : m_flip);
    m_collider.x = (int)posX;

    if (!m_isGrounded) m_airTime += dt;
    else m_airTime = 0.0f;

    m_velocityY += m_gravityForce * (1.0f + m_airTime * 10.0f) * dt;
    if (m_velocityY > 800) m_velocityY = 800;
    posY += m_velocityY * dt;

    m_isGrounded = GroundCheck();
    if (m_isGrounded) {
        m_velocityY = 0;
        posY = m_groundCollider->y - m_sprite->GetHeight() + 2;
    }

    m_sprite->Set2DPosition(posX, posY);
    m_collider = { (int)posX + 50, (int)posY, m_sprite->GetWidth() / 2, m_sprite->GetHeight() };
    m_hpBarBg.x = m_collider.x + m_collider.w / 2 - m_barWidth / 2;
    m_hpBarBg.y = m_collider.y - 10;
    m_hpBarFill = m_hpBarBg;
    m_hpBarFill.w = static_cast<int>(m_barWidth * std::clamp((float)m_currentHP / m_maxHP, 0.0f, 1.0f));

    m_sprite->Update(dt);
}

void RangedEnemy::Draw(SDL_Renderer* renderer)
{
    if (m_isDead && m_finishDead) return;
    if (m_sprite) m_sprite->Draw(renderer);

    Vector2 camPos = Camera::GetInstance()->GetPosition();
    SDL_Rect drawBg = m_hpBarBg;

    int rightPos = 32;
    if (m_flip == SDL_FLIP_HORIZONTAL) {
        rightPos -= 32 * 2;
    }
    drawBg.x -= static_cast<int>(camPos.x) + rightPos;
    drawBg.y -= static_cast<int>(camPos.y);

    SDL_Rect drawFill = m_hpBarFill;
    drawFill.x -= static_cast<int>(camPos.x) + rightPos;
    drawFill.y -= static_cast<int>(camPos.y);

    // Vẽ nền (đen)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &drawBg);

    // Vẽ phần máu còn lại (đỏ)
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &drawFill);
}

bool RangedEnemy::DetectPlayer()
{
    if (!m_playerCollider) return false;
    float ex = m_collider.x + m_collider.w / 2.0f;
    float px = m_playerCollider->x + m_playerCollider->w / 2.0f;
    return std::abs(ex - px) < m_detectRange;
}

void RangedEnemy::ChangeAnimation(std::shared_ptr<TextureManager> texture, int frameCount, float frameTime)
{
    if (!m_sprite) return;
    Vector2 pos = m_sprite->Get2DPosition();
    m_sprite = std::make_shared<SpriteAnimation>(texture, 1, frameCount, 1, frameTime);
    m_sprite->SetSize(96 * 2, 64 * 2);
    m_sprite->Set2DPosition(pos.x, pos.y);
    m_sprite->SetFlip(m_flip);
}

bool RangedEnemy::CheckCollision(const SDL_Rect& a, const SDL_Rect& b) { return SDL_HasIntersection(&a, &b); }
bool RangedEnemy::GroundCheck() { return m_groundCollider && CheckCollision(m_collider, *m_groundCollider); }

void RangedEnemy::ShootProjectile()
{
    Vector2 pos = m_sprite->Get2DPosition();
    int direction = (m_attackFlip == SDL_FLIP_HORIZONTAL) ? 1 : -1;
    pos.x += (direction == 1) ? 64 : -32;

    auto bullet = std::make_shared<Bullet>(pos, 500.0f, direction, m_damage);

    bullet->SetOnHitPlayerCallback([this](int damage) {
        if (m_playerController)
            m_playerController->TakeDamage(damage);
        });

    g_bullets.push_back(bullet);
}

