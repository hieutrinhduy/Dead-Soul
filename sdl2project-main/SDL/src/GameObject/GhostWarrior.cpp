#include "GhostWarrior.h"
#include "ResourceManagers.h"
#include "Camera.h"
#include "Renderer.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include "Sound.h"

int ghostWarriorAttackIndex = 0;

GhostWarrior::GhostWarrior() {}
GhostWarrior::~GhostWarrior() {}

void GhostWarrior::Init(float pointA, float pointB, float posY)
{
    idleTexture = ResourceManagers::GetInstance()->GetTexture("GhostWarrior-IdleFly.png");
    runTexture = ResourceManagers::GetInstance()->GetTexture("GhostWarrior-Fly.png");
    attack1Texture = ResourceManagers::GetInstance()->GetTexture("GhostWarrior-Attack1.png");
    attack2Texture = ResourceManagers::GetInstance()->GetTexture("GhostWarrior-Attack2.png");
    hitTexture = ResourceManagers::GetInstance()->GetTexture("GhostWarrior-Hurt.png");
    deadTexture = ResourceManagers::GetInstance()->GetTexture("GhostWarrior-Die.png");

    m_sprite = std::make_shared<SpriteAnimation>(runTexture, 1, 8, 1, 0.05f);
    m_sprite->SetSize(96 * 2, 64 * 2);
    m_sprite->Set2DPosition(pointA, posY);
    m_collider = { (int)m_sprite->GetPosition().x + 50, (int)m_sprite->GetPosition().y, m_sprite->GetWidth() / 2, m_sprite->GetHeight() };

    m_pointA = pointA;
    m_pointB = pointB;
    m_flip = SDL_FLIP_HORIZONTAL;

    m_hpBarBg = { 0, 0, m_barWidth, m_barHeight };
    m_hpBarFill = m_hpBarBg;
    Sound::GetInstance()->LoadSfx("/Enemy/sfx_enemy_hit.wav");
    Sound::GetInstance()->LoadSfx("/Enemy/sfx_enemy_die.wav");
}

void GhostWarrior::SetGroundCollider(SDL_Rect* ground) { m_groundCollider = ground; }
void GhostWarrior::SetPlayerCollider(const SDL_Rect* player) { m_playerCollider = player; }
SDL_Rect* GhostWarrior::GetCollider() { return &m_collider; }

void GhostWarrior::TakeDamage(int damage, int knockbackDir)
{
    if (m_isDead) return;

    m_currentHP -= damage;
    float hpPercent = std::clamp((float)m_currentHP / m_maxHP, 0.0f, 1.0f);
    m_hpBarFill.w = static_cast<int>(m_barWidth * hpPercent);
    m_isAttacking = false;
    m_state = GhostWarriorState::Patrol;
    if (m_currentHP <= 0) {
        m_currentHP = 0;
        Sound::GetInstance()->PlaySfx("/Enemy/sfx_enemy_die.wav");
        Die();
    }
    else {
        m_isHit = true;
        m_hitTimer = 0.1f;
        ChangeAnimation(hitTexture, 5, 0.05f);
        Sound::GetInstance()->PlaySfx("/Enemy/sfx_enemy_hit.wav");
        // Knockback setup
        m_isKnockback = true;
        m_knockbackTimer = m_knockbackDuration;
        m_knockbackDir = knockbackDir;
    }
}

void GhostWarrior::Die()
{
    m_isDead = true;
    m_deadTimer = 1.3f;
    m_finishDead = false;
    ChangeAnimation(deadTexture, 13, 0.1f);

    if (m_onDeathCallback) {
        m_onDeathCallback();
    }

    if (m_spawnExpCallback) {
        Vector2 basePos = m_sprite->Get2DPosition();
        basePos.x += 80;
        basePos.y += 100;

        for (int i = 0; i < m_exp_orb_number; ++i) {
            Vector2 offset = {
                basePos.x + (rand() % 310 - 150),
                basePos.y
            };
            m_spawnExpCallback(offset);
        }
    }
}

void GhostWarrior::Destroy()
{
    m_sprite = nullptr;
}

void GhostWarrior::Update(float dt)
{
    if (m_isDead) {
        m_collider = { 0, 0, 0, 0 };
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
            ChangeAnimation(runTexture, 8, 0.05f);
        }
        m_sprite->Update(dt);
        return;
    }

    float posX = m_sprite->Get2DPosition().x;
    float posY = m_sprite->Get2DPosition().y;

    switch (m_state) {
    case GhostWarriorState::Patrol:
        if (posX < m_pointA) { m_direction = 1; m_flip = SDL_FLIP_HORIZONTAL; }
        else if (posX > m_pointB) { m_direction = -1; m_flip = SDL_FLIP_NONE; }

        posX += m_direction * m_speed * dt;

        if (m_direction == 1 && posX > m_pointB) {
            m_direction = -1; m_flip = SDL_FLIP_NONE;
        }
        else if (m_direction == -1 && posX < m_pointA) {
            m_direction = 1; m_flip = SDL_FLIP_HORIZONTAL;
        }

        if (!m_recentlyReturnedFromChase && DetectPlayer()) {
            m_state = GhostWarriorState::Chase;
            m_lostPlayerTimer = 0.0f;
        }
        break;

    case GhostWarriorState::Chase:
        if (DetectPlayer()) {
            m_lostPlayerTimer = 0.0f;
        }
        else {
            m_lostPlayerTimer += dt;
            if (m_lostPlayerTimer >= 0.1f) {
                m_state = GhostWarriorState::Patrol;
                ChangeAnimation(runTexture, 8, 0.05f);
                m_recentlyReturnedFromChase = true;
                break;
            }
        }

        if (m_playerCollider) {
            float dx = m_playerCollider->x - posX;
            float moveDir = (dx > 0) ? 1.0f : -1.0f;
            m_flip = (moveDir > 0) ?  SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

            float nextPosX = posX + moveDir * m_speed * 2 * dt;
            if (nextPosX < m_pointA || nextPosX > m_pointB) {
                m_state = GhostWarriorState::Patrol;
                ChangeAnimation(runTexture, 8, 0.05f);
                m_recentlyReturnedFromChase = true;
                if (posX < m_pointA + 10) {
                    m_direction = 1; m_flip = SDL_FLIP_HORIZONTAL;
                }
                else if (posX > m_pointB - 10) {
                    m_direction = -1; m_flip = SDL_FLIP_NONE;
                }
                break;
            }
            posX = nextPosX;

            float ecx = posX + m_sprite->GetWidth() / 2;
            float pcx = m_playerCollider->x + m_playerCollider->w / 2;
            float dx2 = pcx - ecx;

            if (!m_isAttacking && std::abs(dx2) < 130) {
                m_state = GhostWarriorState::Attack;
                m_attackFlip = m_flip;
                m_isAttacking = true;
                ghostWarriorAttackIndex = (rand() % 2 == 0) ? 1 : 2;
                int frames = 11;
                float speed = (ghostWarriorAttackIndex == 1) ? 0.1f : 0.08f;
                ChangeAnimation((ghostWarriorAttackIndex == 1) ? attack1Texture : attack2Texture, frames, speed);
                m_attackTimer = 0.75f;
            }
        }
        break;

    case GhostWarriorState::Attack:
        m_attackTimer -= dt;
        if (m_sprite && !m_didDamage &&
            ((ghostWarriorAttackIndex == 1 && m_sprite->GetCurrentFrame() == 5) ||
                (ghostWarriorAttackIndex == 2 && m_sprite->GetCurrentFrame() == 6)) && m_isAttacking) {
            m_didDamage = true;
            int offsetX = (m_attackFlip == SDL_FLIP_NONE) ? -m_sprite->GetWidth() / 4 : 100;
            m_attackBox = { m_collider.x + offsetX, m_collider.y + m_sprite->GetHeight() / 4, 50, m_sprite->GetHeight() / 2 };
            if (m_playerCollider && CheckCollision(m_attackBox, *m_playerCollider)) {
                if (m_onHitPlayer) m_onHitPlayer(m_damage);
            }
        }
        if (m_attackTimer <= 0.0f) {
            m_isAttacking = false;
            m_didDamage = false;

            bool isChasing = DetectPlayer();
            m_state = isChasing ? GhostWarriorState::Chase : GhostWarriorState::Patrol;
            ChangeAnimation(runTexture, 8, 0.05f);

            if (!isChasing) {
                if (posX < m_pointA + 10) {
                    m_direction = 1;
                    m_flip = SDL_FLIP_HORIZONTAL;
                }
                else if (posX > m_pointB - 10) {
                    m_direction = -1;
                    m_flip = SDL_FLIP_NONE;
                }
                else {
                    // Giữ nguyên direction, nhưng update flip theo direction
                    m_flip = (m_direction == 1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                }
            }
            else {
                float enemyCenterX = posX + m_sprite->GetWidth() / 2;
                float playerCenterX = m_playerCollider->x + m_playerCollider->w / 2;
                float dx = playerCenterX - enemyCenterX;
                m_flip = (dx > 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
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
    m_sprite->SetFlip((m_state == GhostWarriorState::Attack) ? m_attackFlip : m_flip);
    m_collider.x = (int)posX;

    //m_velocityY += m_gravityForce * dt;
    //if (m_velocityY > 800) m_velocityY = 800;
    //posY += m_velocityY * dt;

    //m_isGrounded = GroundCheck();
    //if (m_isGrounded) {
    //    m_velocityY = 0;
    //    posY = m_groundCollider->y - m_sprite->GetHeight() + 2;
    //}

    m_sprite->Set2DPosition(posX, posY);
    m_collider = { (int)posX + 50, (int)posY, m_sprite->GetWidth() / 2, m_sprite->GetHeight() };

    m_hpBarBg.x = m_collider.x + m_collider.w / 2 - m_barWidth / 2;
    m_hpBarBg.y = m_collider.y - 10;
    m_hpBarFill = m_hpBarBg;
    m_hpBarFill.w = static_cast<int>(m_barWidth * std::clamp((float)m_currentHP / m_maxHP, 0.0f, 1.0f));

    m_sprite->Update(dt);
}


void GhostWarrior::Draw(SDL_Renderer* renderer)
{
    if (m_isDead && m_finishDead) return;
    if (m_sprite) m_sprite->Draw(renderer);

    Vector2 camPos = Camera::GetInstance()->GetPosition();

    //SDL_Rect drawCollider = m_collider;
    //drawCollider.x -= (int)camPos.x;
    //drawCollider.y -= (int)camPos.y;
    //SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    //SDL_RenderDrawRect(renderer, &drawCollider);

    //SDL_Rect drawBox = m_attackBox;
    //drawBox.x -= (int)camPos.x;
    //drawBox.y -= (int)camPos.y;
    //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    //SDL_RenderDrawRect(renderer, &drawBox);

    SDL_Rect drawBg = m_hpBarBg;
    drawBg.x -= (int)camPos.x;
    drawBg.y -= (int)camPos.y;

    SDL_Rect drawFill = m_hpBarFill;
    drawFill.x -= (int)camPos.x;
    drawFill.y -= (int)camPos.y;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &drawBg);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &drawFill);

    ////Draw Movement
    //int markerSize = 6;
    //SDL_Color lineColor = { 0, 255, 0, 255 }; // Xanh lá

    //// Vị trí Y dưới chân enemy
    //int drawY = static_cast<int>(m_sprite->Get2DPosition().y + m_sprite->GetHeight()) + 5 - static_cast<int>(camPos.y);

    //// ✅ Điều chỉnh thêm offset X = nửa chiều rộng sprite
    //int offsetX = m_sprite->GetWidth() / 2;

    //// Vẽ đường từ A đến B (có offset để canh giữa enemy)
    //SDL_SetRenderDrawColor(renderer, lineColor.r, lineColor.g, lineColor.b, lineColor.a);
    //SDL_RenderDrawLine(renderer,
    //    static_cast<int>(m_pointA + offsetX - camPos.x),
    //    drawY,
    //    static_cast<int>(m_pointB + offsetX - camPos.x),
    //    drawY
    //);

    //// Vẽ chấm tại pointA
    //SDL_Rect pointAmarker = {
    //    static_cast<int>(m_pointA + offsetX - camPos.x - markerSize / 2),
    //    drawY - markerSize / 2,
    //    markerSize,
    //    markerSize
    //};
    //SDL_RenderFillRect(renderer, &pointAmarker);

    //// Vẽ chấm tại pointB
    //SDL_Rect pointBmarker = {
    //    static_cast<int>(m_pointB + offsetX - camPos.x - markerSize / 2),
    //    drawY - markerSize / 2,
    //    markerSize,
    //    markerSize
    //};
    //SDL_RenderFillRect(renderer, &pointBmarker);
}

bool GhostWarrior::DetectPlayer()
{
    if (!m_playerCollider) return false;
    if (m_playerCollider->w == 1) return false;
    float ex = m_collider.x + m_collider.w / 2.0f;
    float px = m_playerCollider->x + m_playerCollider->w / 2.0f;

    float enemyCenterY = m_collider.y + m_collider.h / 2.0f;
    float playerCenterY = m_playerCollider->y + m_playerCollider->h / 2.0f;
    float distanceY = std::abs(enemyCenterY - playerCenterY);
    if (distanceY > 50) return false;
    return std::abs(ex - px) < 250;
}

void GhostWarrior::ChangeAnimation(std::shared_ptr<TextureManager> texture, int frameCount, float frameTime)
{
    if (!m_sprite) return;
    Vector2 pos = m_sprite->Get2DPosition();
    m_sprite = std::make_shared<SpriteAnimation>(texture, 1, frameCount, 1, frameTime);
    m_sprite->SetSize(96 * 2, 64 * 2);
    m_sprite->Set2DPosition(pos.x, pos.y);
    m_sprite->SetFlip(m_flip);
}

bool GhostWarrior::CheckCollision(const SDL_Rect& a, const SDL_Rect& b) { return SDL_HasIntersection(&a, &b); }
bool GhostWarrior::GroundCheck() { return m_groundCollider && CheckCollision(m_collider, *m_groundCollider); }

void GhostWarrior::SetOnDeathCallback(std::function<void()> callback)
{
    m_onDeathCallback = callback;
}

void GhostWarrior::SetSpawnExpCallback(std::function<void(Vector2)> callback)
{
    m_spawnExpCallback = callback;
}
