#include "EvilCreature.h"
#include "ResourceManagers.h"
#include "Camera.h"
#include "Renderer.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>

int devilCreatureAttackIndex = 0;


EvilCreature::EvilCreature()
    : m_gravityForce(150.0f), m_velocityY(0.0f), m_speed(200.0f),
    m_direction(1), m_isAttacking(false), m_attackTimer(0.0f), m_attackCooldown(0.0f),
    m_isHit(false), m_hitTimer(0.0f), m_isDead(false), m_groundCollider(nullptr),
    m_playerCollider(nullptr), m_lostPlayerTimer(0.0f), m_state(EvilCreatureState::Patrol),
    m_maxHP(100), m_currentHP(100), m_damage(1), m_flip(SDL_FLIP_NONE) {
}

EvilCreature::~EvilCreature() {}

void EvilCreature::Init(float pointA, float pointB, float posY)
{
    idleTexture = ResourceManagers::GetInstance()->GetTexture("EvilCreature-Idle.png");
    runTexture = ResourceManagers::GetInstance()->GetTexture("EvilCreature-Walk.png");
    attack1Texture = ResourceManagers::GetInstance()->GetTexture("EvilCreature-Attack1.png");
    attack2Texture = ResourceManagers::GetInstance()->GetTexture("EvilCreature-Attack2.png");
    hitTexture = ResourceManagers::GetInstance()->GetTexture("EvilCreature-Hurt.png");
    deadTexture = ResourceManagers::GetInstance()->GetTexture("EvilCreature-Die.png");

    m_sprite = std::make_shared<SpriteAnimation>(runTexture, 1, 10, 1, 0.05f);
    m_sprite->SetSize(96 * 2, 64 * 2);
    m_sprite->Set2DPosition(pointA, posY);
    m_collider = { (int)m_sprite->GetPosition().x + 50, (int)m_sprite->GetPosition().y, m_sprite->GetWidth() / 2, m_sprite->GetHeight() };

    m_pointA = pointA;
    m_pointB = pointB;
    m_flip = SDL_FLIP_HORIZONTAL;

    m_hpBarBg = { 0, 0, m_barWidth, m_barHeight };
    m_hpBarFill = m_hpBarBg;
}

void EvilCreature::SetGroundCollider(SDL_Rect* ground) { m_groundCollider = ground; }
void EvilCreature::SetPlayerCollider(const SDL_Rect* player) { m_playerCollider = player; }
SDL_Rect* EvilCreature::GetCollider() { return &m_collider; }

void EvilCreature::TakeDamage(int damage)
{
    if (m_isDead) return;
    std::cout << "[EvilCreature] Bị trúng đòn: -" << damage << " HP\n";

    m_currentHP -= damage;
    float hpPercent = std::clamp((float)m_currentHP / m_maxHP, 0.0f, 1.0f);
    m_hpBarFill.w = static_cast<int>(m_barWidth * hpPercent);

    if (m_currentHP <= 0) {
        m_currentHP = 0;
        Die();
    }
    else if (!m_isAttacking) {
        m_isHit = true;
        m_hitTimer = 0.2f;
        ChangeAnimation(hitTexture, 5, 0.05f);
    }
}

void EvilCreature::Die()
{
    m_isDead = true;
    m_deadTimer = 1.3f;
    m_finishDead = false;
    ChangeAnimation(deadTexture, 14, 0.1f);
}

void EvilCreature::Destroy()
{
    m_collider = { 0, 0, 0, 0 };
    m_sprite = nullptr;
    std::cout << "EvilCreature destroyed!\n";
}

void EvilCreature::Update(float deltaTime)
{
    if (m_isDead) {
        if (!m_finishDead) {
            m_deadTimer -= deltaTime;
            if (m_deadTimer <= 0.0f) {
                m_finishDead = true;
                Destroy();
            }
        }
        if (m_sprite) m_sprite->Update(deltaTime);
        return;
    }

    if (m_isHit) {
        m_hitTimer -= deltaTime;
        if (m_hitTimer <= 0.0f) {
            m_isHit = false;
            ChangeAnimation(runTexture, 10, 0.05f);
        }
        m_sprite->Update(deltaTime);
        return;
    }

    float posX = m_sprite->Get2DPosition().x;
    float posY = m_sprite->Get2DPosition().y;

    switch (m_state) {
    case EvilCreatureState::Patrol:
        if (posX < m_pointA) {
            m_direction = 1;
            m_flip = SDL_FLIP_HORIZONTAL;
        }
        else if (posX > m_pointB) {
            m_direction = -1;
            m_flip = SDL_FLIP_NONE;
        }
        posX += m_direction * m_speed * deltaTime;

        if (m_direction == 1 && posX > m_pointB) {
            m_direction = -1;
            m_flip = SDL_FLIP_NONE;
        }
        else if (m_direction == -1 && posX < m_pointA) {
            m_direction = 1;
            m_flip = SDL_FLIP_HORIZONTAL;
        }

        if (!m_recentlyReturnedFromChase && DetectPlayer()) {
            m_state = EvilCreatureState::Chase;
            m_lostPlayerTimer = 0.0f;
        }
        break;

    case EvilCreatureState::Chase:
        if (DetectPlayer()) {
            m_lostPlayerTimer = 0.0f;
        }
        else {
            m_lostPlayerTimer += deltaTime;
            if (m_lostPlayerTimer >= 0.1f) {
                m_state = EvilCreatureState::Patrol;
                ChangeAnimation(runTexture, 10, 0.05f);
                m_recentlyReturnedFromChase = true;
                break;
            }
        }

        if (m_playerCollider) {
            float distanceX = m_playerCollider->x - posX;
            float moveDir = (distanceX > 0) ? 1.0f : -1.0f;
            m_flip = (moveDir > 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

            float nextPosX = posX + moveDir * m_speed * 2 * deltaTime;
            if (nextPosX < m_pointA || nextPosX > m_pointB) {
                m_state = EvilCreatureState::Patrol;
                ChangeAnimation(runTexture, 10, 0.05f);
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
            float dx = playerCenterX - enemyCenterX;

            int attackDistance = 130;
            if (!m_isAttacking && std::abs(dx) < attackDistance) {
                m_state = EvilCreatureState::Attack;
                m_attackFlip = m_flip;
                m_isAttacking = true;

                if (rand() % 2 == 0) {
                    ChangeAnimation(attack1Texture, 14, 0.1f);
                    devilCreatureAttackIndex = 1;
                    m_attackTimer = 1.2f;
                }
                else {
                    ChangeAnimation(attack2Texture, 19, 0.08f);
                    devilCreatureAttackIndex = 2;
                    m_attackTimer = 1.5f;
                }
            }
        }
        break;

    case EvilCreatureState::Attack:
        m_attackTimer -= deltaTime;
        if (m_sprite) {
            int currentFrame = m_sprite->GetCurrentFrame();
            if (((devilCreatureAttackIndex == 1 && currentFrame == 9) || (devilCreatureAttackIndex == 2 && currentFrame == 10)) && !m_didDamage && m_isAttacking) {
                m_didDamage = true;
                int offsetX = (m_attackFlip == SDL_FLIP_NONE) ? -m_sprite->GetWidth() / 4 : 100;
                m_attackBox = {
                    m_collider.x + offsetX,
                    m_collider.y + m_sprite->GetHeight() / 4,
                    50,
                    m_sprite->GetHeight() / 2
                };
                if (m_playerCollider && CheckCollision(m_attackBox, *m_playerCollider)) {
                    if (m_onHitPlayer) m_onHitPlayer(m_damage);
                }
            }
        }
        if (m_attackTimer <= 0.0f) {
            m_isAttacking = false;
            m_didDamage = false;
            if (DetectPlayer()) {
                m_state = EvilCreatureState::Chase;
                ChangeAnimation(runTexture, 10, 0.05f);
                m_flip = (m_playerCollider->x > m_sprite->Get2DPosition().x) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            }
            else {
                m_state = EvilCreatureState::Patrol;
                ChangeAnimation(runTexture, 10, 0.05f);
                m_flip = (m_direction == 1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            }
        }
        break;
    }

    if (m_lostPlayerTimer > 0.0f)
        m_lostPlayerTimer -= deltaTime;

    if (m_recentlyReturnedFromChase) {
        m_recentlyReturnedTimer += deltaTime;
        if (m_recentlyReturnedTimer >= 0.3f) {
            m_recentlyReturnedFromChase = false;
            m_recentlyReturnedTimer = 0.0f;
        }
    }

    m_sprite->Set2DPosition(posX, posY);
    m_sprite->SetFlip((m_state == EvilCreatureState::Attack) ? m_attackFlip : m_flip);
    m_collider.x = (int)posX;

    //m_velocityY += m_gravityForce * deltaTime;
    //if (m_velocityY > 800) m_velocityY = 800;
    //posY += m_velocityY * deltaTime;

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
    float hpPercent = std::clamp((float)m_currentHP / m_maxHP, 0.0f, 1.0f);
    m_hpBarFill.w = static_cast<int>(m_barWidth * hpPercent);

    m_sprite->Update(deltaTime);
}



void EvilCreature::Draw(SDL_Renderer* renderer)
{
    if (m_isDead && m_finishDead) return;

    if (m_sprite) m_sprite->Draw(renderer);

    Vector2 camPos = Camera::GetInstance()->GetPosition();

    SDL_Rect drawCollider = m_collider;
    drawCollider.x -= (int)camPos.x;
    drawCollider.y -= (int)camPos.y;
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderDrawRect(renderer, &drawCollider);

    SDL_Rect drawAttackBox = m_attackBox;
    drawAttackBox.x -= (int)camPos.x;
    drawAttackBox.y -= (int)camPos.y;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &drawAttackBox);

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
}

bool EvilCreature::DetectPlayer()
{
    if (!m_playerCollider) return false;
    float enemyCenterX = m_collider.x + m_collider.w / 2.0f;
    float playerCenterX = m_playerCollider->x + m_playerCollider->w / 2.0f;
    return std::abs(enemyCenterX - playerCenterX) < 250;
}

void EvilCreature::ChangeAnimation(std::shared_ptr<TextureManager> texture, int frameCount, float frameTime)
{
    if (!m_sprite) return;
    Vector2 pos = m_sprite->Get2DPosition();
    m_sprite = std::make_shared<SpriteAnimation>(texture, 1, frameCount, 1, frameTime);
    m_sprite->SetSize(96 * 2, 64 * 2);
    m_sprite->Set2DPosition(pos.x, pos.y);
    m_sprite->SetFlip(m_flip);
}

bool EvilCreature::CheckCollision(const SDL_Rect& a, const SDL_Rect& b)
{
    return SDL_HasIntersection(&a, &b);
}

bool EvilCreature::GroundCheck()
{
    if (!m_groundCollider) return false;
    return CheckCollision(m_collider, *m_groundCollider);
}
