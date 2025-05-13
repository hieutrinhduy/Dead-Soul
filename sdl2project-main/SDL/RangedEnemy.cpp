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

void RangedEnemy::Init(float pointA, float pointB, float posY)
{
    idleTexture = ResourceManagers::GetInstance()->GetTexture("FlyEnemyIDLE.png");
    runTexture = ResourceManagers::GetInstance()->GetTexture("FlyEnemyFLYING.png");
    attackTexture = ResourceManagers::GetInstance()->GetTexture("FlyEnemyATTACK.png");
    hitTexture = ResourceManagers::GetInstance()->GetTexture("FlyEnemyHURT.png");
    deadTexture = ResourceManagers::GetInstance()->GetTexture("FlyEnemyDEATH.png");

    m_sprite = std::make_shared<SpriteAnimation>(runTexture, 1, 4, 1, 0.05f);
    m_sprite->SetSize(96 * 2, 64 * 2);
    m_sprite->Set2DPosition(pointA, posY);
    m_collider = { (int)m_sprite->GetPosition().x + 50, (int)m_sprite->GetPosition().y, m_sprite->GetWidth() / 2, m_sprite->GetHeight() };

    m_pointA = pointA;
    m_pointB = pointB;
    m_flip = SDL_FLIP_HORIZONTAL;

    m_hpBarBg = { 0, 0, m_barWidth, m_barHeight };
    m_hpBarFill = m_hpBarBg;
}

void RangedEnemy::SetGroundCollider(SDL_Rect* ground) { m_groundCollider = ground; }
void RangedEnemy::SetPlayerCollider(const SDL_Rect* player) { m_playerCollider = player; }
SDL_Rect* RangedEnemy::GetCollider() { return &m_collider; }

void RangedEnemy::TakeDamage(int damage, int knockbackDir)
{
    if (m_isDead) return;

    std::cout << "[EnemyController] Bị trúng đòn: -" << damage << " HP\n";

    m_currentHP -= damage;
    float hpPercent = std::clamp((float)m_currentHP / m_maxHP, 0.0f, 1.0f);
    m_hpBarFill.w = static_cast<int>(m_barWidth * hpPercent);
    m_isAttacking = false;
    m_state = RangedEnemyState::Patrol;
    if (m_currentHP <= 0) {
        m_currentHP = 0;
        std::cout << "[EnemyController] ĐÃ CHẾT\n";
        Die();
    }
    else {
        m_isHit = true;
        m_hitTimer = 0.1f;
        ChangeAnimation(hitTexture, 4, 0.05f);

        // Knockback setup
        m_isKnockback = true;
        m_knockbackTimer = m_knockbackDuration;
        m_knockbackDir = knockbackDir;
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

    if (m_isKnockback) {
        m_knockbackTimer -= dt;

        float knockbackAmount = m_knockbackDir * m_knockbackSpeed * dt;
        float nextX = m_sprite->Get2DPosition().x + knockbackAmount;

        // Giới hạn trong khoảng pointA - pointB
        if (nextX < m_pointA) nextX = m_pointA;
        if (nextX > m_pointB) nextX = m_pointB;

        m_sprite->Set2DPosition(nextX, m_sprite->Get2DPosition().y);
        m_collider.x = (int)nextX + 50;

        m_hpBarBg.x = m_collider.x + m_collider.w / 2 - m_barWidth / 2;
        m_hpBarBg.y = m_collider.y - 10;
        m_hpBarFill = m_hpBarBg;
        float hpPercent = std::clamp((float)m_currentHP / m_maxHP, 0.0f, 1.0f);
        m_hpBarFill.w = static_cast<int>(m_barWidth * hpPercent);

        if (m_knockbackTimer <= 0.0f) {
            m_isKnockback = false;
        }

        m_sprite->Update(dt);
        if (!m_isHit) {
            return;
        }
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
        if (posX < m_pointA) {
            m_direction = 1;
            m_flip = SDL_FLIP_HORIZONTAL;
        }
        else if (posX > m_pointB) {
            m_direction = -1;
            m_flip = SDL_FLIP_NONE;
        }

        posX += m_direction * m_speed * dt;

        if (m_direction == 1 && posX > m_pointB) {
            m_direction = -1;
            m_flip = SDL_FLIP_NONE;
        }
        else if (m_direction == -1 && posX < m_pointA) {
            m_direction = 1;
            m_flip = SDL_FLIP_HORIZONTAL;
        }

        if (!m_recentlyReturnedFromChase && DetectPlayer()) {
            m_state = RangedEnemyState::Chase;
            m_lostPlayerTimer = 0.0f;
        }
        break;

    case RangedEnemyState::Chase:
        if (DetectPlayer()) {
            m_lostPlayerTimer = 0.0f;
        }
        else {
            m_lostPlayerTimer += dt;
            if (m_lostPlayerTimer >= 0.1f) {
                m_state = RangedEnemyState::Patrol;
                ChangeAnimation(runTexture, 4, 0.05f);
                m_recentlyReturnedFromChase = true;
                break;
            }
        }

        if (m_playerCollider) {
            float dx = m_playerCollider->x - posX;
            float moveDir = (dx > 0) ? 1.0f : -1.0f;

            float nextPosX = posX + moveDir * m_speed * 2 * dt;
            if (nextPosX < m_pointA || nextPosX > m_pointB) {
                m_state = RangedEnemyState::Patrol;
                ChangeAnimation(runTexture, 4, 0.05f);
                m_recentlyReturnedFromChase = true;

                if (posX < m_pointA + 10) {
                    m_direction = 1;
                    m_flip = SDL_FLIP_HORIZONTAL;
                }
                else if (posX > m_pointB - 10) {
                    m_direction = -1;
                    m_flip = SDL_FLIP_NONE;
                }
                break;
            }

            posX = nextPosX;

            float enemyCenterX = posX + m_sprite->GetWidth() / 2;
            float playerCenterX = m_playerCollider->x + m_playerCollider->w / 2;
            float dx2 = playerCenterX - enemyCenterX;

            if (!m_isAttacking && std::abs(dx2) < m_attackRange) {
                m_state = RangedEnemyState::Attack;
                m_attackFlip = (dx2 > 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                m_isAttacking = true;
                ChangeAnimation(attackTexture, 8, 0.1f);
                m_attackTimer = 0.75f;
            }

            // Flip nếu không đang tấn công
            if (!m_isAttacking)
                m_flip = (moveDir > 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
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
            if (DetectPlayer()) {
                m_state = RangedEnemyState::Chase;
                ChangeAnimation(runTexture, 4, 0.05f);
                m_flip = (m_playerCollider->x > m_sprite->Get2DPosition().x) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            }
            else {
                m_state = RangedEnemyState::Patrol;
                ChangeAnimation(runTexture, 4, 0.05f);
                m_flip = (m_direction == 1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            }
        }
        break;
    }

    if (m_lostPlayerTimer > 0.0f)
        m_lostPlayerTimer -= dt;

    if (m_recentlyReturnedFromChase) {
        m_recentlyReturnedTimer += dt;
        if (m_recentlyReturnedTimer >= 0.3f) {
            m_recentlyReturnedFromChase = false;
            m_recentlyReturnedTimer = 0.0f;
        }
    }

    m_sprite->Set2DPosition(posX, posY);
    m_sprite->SetFlip((m_state == RangedEnemyState::Attack) ? m_attackFlip : m_flip);
    m_collider.x = (int)posX;

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
    if (m_flip == SDL_FLIP_NONE) {
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
    if (m_playerCollider->w == 1) return false;
    float ex = m_collider.x + m_collider.w / 2.0f;
    float px = m_playerCollider->x + m_playerCollider->w / 2.0f;
    float enemyCenterY = m_collider.y + m_collider.h / 2.0f;
    float playerCenterY = m_playerCollider->y + m_playerCollider->h / 2.0f;
    float distanceY = std::abs(enemyCenterY - playerCenterY);
    if (distanceY > 50) return false;
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

    bullet->SetOnHitPlayerCallback([this, direction](int damage) {
        if (m_playerController)
            m_playerController->TakeDamage(damage, direction);
        });

    g_bullets.push_back(bullet);
}

