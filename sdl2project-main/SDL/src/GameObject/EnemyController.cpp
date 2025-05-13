#include "EnemyController.h"
#include "ResourceManagers.h"
#include "Camera.h"
#include "Renderer.h"
#include <cstdlib>
#include <iostream>
#include <algorithm> 

int attackIndex = 0;

const char* EnemyStateToString(EnemyState state) {
    switch (state) {
    case EnemyState::Idle: return "Idle";
    case EnemyState::Patrol: return "Patrol";
    case EnemyState::Chase: return "Chase";
    case EnemyState::Attack: return "Attack";
    case EnemyState::Hit: return "Hit";
    case EnemyState::Dead: return "Dead";
    default: return "Unknown";
    }
}

EnemyController::EnemyController()
    : m_gravityForce(150.0f), m_velocityY(0.0f), m_speed(200.0f),
    m_direction(1),
    m_isAttacking(false), m_attackTimer(0.0f), m_attackCooldown(0.0f),
    m_isHit(false), m_hitTimer(0.0f),
    m_isDead(false),
    m_groundCollider(nullptr),
    m_playerCollider(nullptr),
    m_lostPlayerTimer(0.0f),
    m_currentState(EnemyState::Patrol),
    m_drawRay(false),
    m_maxHP(100), m_currentHP(100), m_damage(30),
    m_attackFlip(SDL_FLIP_NONE)
{
}

EnemyController::~EnemyController()
{
}

void EnemyController::Init(float pointA, float pointB, float posY)
{
    idleTexture = ResourceManagers::GetInstance()->GetTexture("Skeleton_01_White_Idle.png");
    runTexture = ResourceManagers::GetInstance()->GetTexture("Skeleton_01_White_Walk.png");
    attack1Texture = ResourceManagers::GetInstance()->GetTexture("Skeleton_01_White_Attack1.png");
    attack2Texture = ResourceManagers::GetInstance()->GetTexture("Skeleton_01_White_Attack2.png");
    hitTexture = ResourceManagers::GetInstance()->GetTexture("Skeleton_01_White_Hurt.png");
    deadTexture = ResourceManagers::GetInstance()->GetTexture("Skeleton_01_White_Die.png");

    m_enemy = std::make_shared<SpriteAnimation>(runTexture, 1, 10, 1, 0.05f);
    m_enemy->SetSize(96 * 2, 64 * 2);
    m_enemy->Set2DPosition(pointA, posY);
    m_enemyCollider = { (int)m_enemy->GetPosition().x + 50, (int)m_enemy->GetPosition().y, m_enemy->GetWidth() / 2, m_enemy->GetHeight() };

    m_pointA = pointA;
    m_pointB = pointB;
    m_enemyFlip = SDL_FLIP_NONE;

    m_hpBarBg = { 0, 0, m_barWidth, m_barHeight };
    m_hpBarFill = m_hpBarBg;
}

void EnemyController::SetGroundCollider(SDL_Rect* ground)
{
    m_groundCollider = ground;
}

void EnemyController::SetPlayerCollider(const SDL_Rect* player)
{
    m_playerCollider = player;
}

SDL_Rect* EnemyController::GetEnemyCollider() {
    return &m_enemyCollider;
}

void EnemyController::TakeDamage(int damage, int knockbackDir)
{
    if (m_isDead) return;

    std::cout << "[EnemyController] Bị trúng đòn: -" << damage << " HP\n";

    m_currentHP -= damage;
    float hpPercent = std::clamp((float)m_currentHP / m_maxHP, 0.0f, 1.0f);
    m_hpBarFill.w = static_cast<int>(m_barWidth * hpPercent);
    m_isAttacking = false;
    m_currentState = EnemyState::Patrol;
    if (m_currentHP <= 0) {
        m_currentHP = 0;
        std::cout << "[EnemyController] ĐÃ CHẾT\n";
        Die();
    }
    else {
        m_isHit = true;
        m_hitTimer = 0.2f;
        ChangeAnimation(hitTexture, 5, 0.05f);

        // Knockback setup
        m_isKnockback = true;
        m_knockbackTimer = m_knockbackDuration;
        m_knockbackDir = knockbackDir;
    }
}



void EnemyController::Die()
{
    m_isDead = true;
    m_deadTimer = 1.3f;
    m_finishDead = false;
    ChangeAnimation(deadTexture, 13, 0.1f);

    if (m_onDeathCallback) {
        m_onDeathCallback();
    }

    if (m_spawnExpCallback) {
        Vector2 pos = m_enemy->Get2DPosition();
        m_spawnExpCallback(pos);
    }
}

// EnemyController.cpp
void EnemyController::SetSpawnExpCallback(std::function<void(Vector2)> callback)
{
    m_spawnExpCallback = callback;
}


void EnemyController::DestroyEnemy()
{
    m_enemy = nullptr;
    std::cout << "Enemy destroyed!\n";
}

void EnemyController::Update(float deltaTime)
{
    if (m_isDead) {
        m_enemyCollider = { INT_MAX, INT_MAX, 0, 0 };
        if (!m_finishDead) {
            m_deadTimer -= deltaTime;
            if (m_deadTimer <= 0.0f) {
                m_finishDead = true;
                DestroyEnemy();
            } 
        }
        if (m_enemy)
            m_enemy->Update(deltaTime);
        return;
    }

    if (m_isKnockback) {
        m_knockbackTimer -= deltaTime;

        float knockbackAmount = m_knockbackDir * m_knockbackSpeed * deltaTime;
        float nextX = m_enemy->Get2DPosition().x + knockbackAmount;

        // Giới hạn trong khoảng pointA - pointB
        if (nextX < m_pointA) nextX = m_pointA;
        if (nextX > m_pointB) nextX = m_pointB;

        m_enemy->Set2DPosition(nextX, m_enemy->Get2DPosition().y);
        m_enemyCollider.x = (int)nextX + 50;

        m_hpBarBg.x = m_enemyCollider.x + m_enemyCollider.w / 2 - m_barWidth / 2;
        m_hpBarBg.y = m_enemyCollider.y - 10;
        m_hpBarFill = m_hpBarBg;
        float hpPercent = std::clamp((float)m_currentHP / m_maxHP, 0.0f, 1.0f);
        m_hpBarFill.w = static_cast<int>(m_barWidth * hpPercent);

        if (m_knockbackTimer <= 0.0f) {
            m_isKnockback = false;
        }

        m_enemy->Update(deltaTime);
        if (!m_isHit) {
            return;
        }
    }


    if (m_isHit) {
        m_hitTimer -= deltaTime;
        if (m_hitTimer <= 0.0f) {
            m_isHit = false;
            ChangeAnimation(runTexture, 10, 0.05f);
        }
        m_enemy->Update(deltaTime);
        return;
    }

    float posX = m_enemy->Get2DPosition().x;
    float posY = m_enemy->Get2DPosition().y;

    switch (m_currentState) {
    case EnemyState::Patrol:
        if (posX < m_pointA) {
            m_direction = 1;
            m_enemyFlip = SDL_FLIP_NONE;
        }
        else if (posX > m_pointB) {
            m_direction = -1;
            m_enemyFlip = SDL_FLIP_HORIZONTAL;
        }

        posX += m_direction * m_speed * deltaTime;

        if (m_direction == 1 && posX > m_pointB) {
            m_direction = -1;
            m_enemyFlip = SDL_FLIP_HORIZONTAL;
        }
        else if (m_direction == -1 && posX < m_pointA) {
            m_direction = 1;
            m_enemyFlip = SDL_FLIP_NONE;
        }

        // CHỈ phát hiện player nếu KHÔNG vừa quay về từ chase
        if (!m_recentlyReturnedFromChase && DetectPlayer()) {
            m_currentState = EnemyState::Chase;
            m_lostPlayerTimer = 0.0f;
        }
        break;


    case EnemyState::Chase:
        if (DetectPlayer()) {
            m_lostPlayerTimer = 0.0f;
        }
        else {
            m_lostPlayerTimer += deltaTime;
            if (m_lostPlayerTimer >= 0.1f) {
                m_currentState = EnemyState::Patrol;
                ChangeAnimation(runTexture, 10, 0.05f);
                m_recentlyReturnedFromChase = true; // GẮN CỜ
                break;
            }
        }

        if (m_playerCollider) {
            float distanceX = m_playerCollider->x - posX;
            float moveDir = (distanceX > 0) ? 1.0f : -1.0f;
            m_enemyFlip = (moveDir > 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

            float nextPosX = posX + moveDir * m_speed * 2 * deltaTime;
            if (nextPosX < m_pointA || nextPosX > m_pointB) {
                m_currentState = EnemyState::Patrol;
                ChangeAnimation(runTexture, 10, 0.05f);
                m_recentlyReturnedFromChase = true;

                // ✅ THÊM ĐOẠN NÀY
                if (posX < m_pointA + 10) {
                    m_direction = 1;
                    m_enemyFlip = SDL_FLIP_NONE;
                }
                else if (posX > m_pointB - 10) {
                    m_direction = -1;
                    m_enemyFlip = SDL_FLIP_HORIZONTAL;
                }

                break;
            }


            posX = nextPosX;

            float enemyCenterX = posX + m_enemy->GetWidth() / 2;
            float playerCenterX = m_playerCollider->x + m_playerCollider->w / 2;
            float dx = playerCenterX - enemyCenterX;

            int attackDistance = 130;
            if (!m_isAttacking && std::abs(dx) < attackDistance) {
                m_currentState = EnemyState::Attack;
                m_attackFlip = m_enemyFlip;
                m_isAttacking = true;

                if (rand() % 2 == 0) {
                    ChangeAnimation(attack1Texture, 10, 0.1f);
                    attackIndex = 1;
                    m_attackTimer = .75f;
                }
                else {
                    ChangeAnimation(attack2Texture, 9, 0.08f);
                    attackIndex = 2;
                    m_attackTimer = .75f;
                }
            }
        }
        break;



    case EnemyState::Attack:
    {
        m_attackTimer -= deltaTime;

        if (m_enemy)
        {
            int currentFrame = m_enemy->GetCurrentFrame();

            if (attackIndex == 1 && currentFrame == 5 && !m_didDamage && m_isAttacking)
            {
                m_didDamage = true;
                int offsetX = 100;
                if (m_attackFlip == SDL_FLIP_HORIZONTAL) offsetX = -m_enemy->GetWidth() / 4;

                m_attackBox = {
                    m_enemyCollider.x + offsetX,
                    m_enemyCollider.y + m_enemy->GetHeight() / 4,
                    50,
                    m_enemy->GetHeight() / 2
                };

                if (m_playerCollider && CheckCollision(m_attackBox, *m_playerCollider)) {
                    if (m_onHitPlayer) m_onHitPlayer(m_damage);
                }
            }
            else if (attackIndex == 2 && currentFrame == 6 && !m_didDamage && m_isAttacking)
            {
                m_didDamage = true;
                int offsetX = 100;
                if (m_attackFlip == SDL_FLIP_HORIZONTAL) offsetX = -m_enemy->GetWidth() / 4;

                m_attackBox = {
                    m_enemyCollider.x + offsetX,
                    m_enemyCollider.y + m_enemy->GetHeight() / 4,
                    50,
                    m_enemy->GetHeight() / 2
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
                m_currentState = EnemyState::Chase;
                ChangeAnimation(runTexture, 10, 0.05f);
                m_enemyFlip = (m_playerCollider->x > m_enemy->Get2DPosition().x) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            }
            else {
                m_currentState = EnemyState::Patrol;
                ChangeAnimation(runTexture, 10, 0.05f);
                m_enemyFlip = (m_direction == 1) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            }
        }
    }
    break;
    }

    m_enemy->Set2DPosition(posX, posY);
    m_enemy->SetFlip((m_currentState == EnemyState::Attack) ? m_attackFlip : m_enemyFlip);
    m_enemyCollider.x = (int)posX;

    //if (!m_isGrounded) m_airTime += deltaTime;
    //else m_airTime = 0.0f;

    //m_velocityY += m_gravityForce * (1.0f + m_airTime * 10.0f) * deltaTime;
    //if (m_velocityY > 800) m_velocityY = 800;

    //posY += m_velocityY * deltaTime;
    //m_enemyCollider = { (int)posX + 50, (int)posY, m_enemy->GetWidth() / 2, m_enemy->GetHeight() };
    //m_isGrounded = GroundCheck();

    //if (m_isGrounded) {
    //    m_velocityY = 0;
    //    posY = m_groundCollider->y - m_enemy->GetHeight() + 2;
    //}

    m_enemy->Set2DPosition(posX, posY);
    m_enemyCollider = { (int)posX + 50, (int)posY, m_enemy->GetWidth() / 2, m_enemy->GetHeight() };

    // Cập nhật vị trí thanh máu
    m_hpBarBg.x = m_enemyCollider.x + m_enemyCollider.w / 2 - m_barWidth / 2;
    m_hpBarBg.y = m_enemyCollider.y - 10;  // Khoảng cách phía trên đầu

    m_hpBarFill = m_hpBarBg;
    float hpPercent = std::clamp((float)m_currentHP / m_maxHP, 0.0f, 1.0f);
    m_hpBarFill.w = static_cast<int>(m_barWidth * hpPercent);

    if (m_lostPlayerTimer > 0.0f)
        m_lostPlayerTimer -= deltaTime;

    if (m_recentlyReturnedFromChase) {
        m_recentlyReturnedTimer += deltaTime;
        if (m_recentlyReturnedTimer >= 0.3f) {
            m_recentlyReturnedFromChase = false;
            m_recentlyReturnedTimer = 0.0f;
        }
    }


    m_enemy->Update(deltaTime);
}

void EnemyController::Draw(SDL_Renderer* renderer)
{
    if (m_isDead && m_finishDead) return;

    if (m_enemy)
        m_enemy->Draw(renderer);

    Vector2 camPos = Camera::GetInstance()->GetPosition();

    //SDL_Rect drawCollider = m_enemyCollider;
    //drawCollider.x -= (int)camPos.x;
    //drawCollider.y -= (int)camPos.y;
    //SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    //SDL_RenderDrawRect(renderer, &drawCollider);

    //SDL_Rect drawAttackBox = m_attackBox;
    //drawAttackBox.x -= (int)camPos.x;
    //drawAttackBox.y -= (int)camPos.y;
    //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    //SDL_RenderDrawRect(renderer, &drawAttackBox);

    SDL_Rect drawBg = m_hpBarBg;
    drawBg.x -= static_cast<int>(camPos.x);
    drawBg.y -= static_cast<int>(camPos.y);

    SDL_Rect drawFill = m_hpBarFill;
    drawFill.x -= static_cast<int>(camPos.x);
    drawFill.y -= static_cast<int>(camPos.y);

    // Vẽ nền (đen)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &drawBg);

    // Vẽ phần máu còn lại (đỏ)
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &drawFill);
}

bool EnemyController::DetectPlayer()
{
    if (!m_playerCollider) return false;
    if (m_playerCollider->w == 1) 
    { 
        return false;
    }
    float enemyCenterX = m_enemyCollider.x + m_enemyCollider.w / 2.0f;
    float playerCenterX = m_playerCollider->x + m_playerCollider->w / 2.0f;

    float enemyCenterY = m_enemyCollider.y + m_enemyCollider.h / 2.0f;
    float playerCenterY = m_playerCollider->y + m_playerCollider->h / 2.0f;
    float distanceY = std::abs(enemyCenterY - playerCenterY);
    if (distanceY > 50) return false;

    float distance = std::abs(enemyCenterX - playerCenterX);
    return distance < 250.0f;
}

void EnemyController::ChangeAnimation(std::shared_ptr<TextureManager> texture, int frameCount, float frameTime)
{
    if (!m_enemy) return;

    Vector2 pos = m_enemy->Get2DPosition();

    m_enemy = std::make_shared<SpriteAnimation>(texture, 1, frameCount, 1, frameTime);
    m_enemy->SetSize(96 * 2, 64 * 2);
    m_enemy->Set2DPosition(pos.x, pos.y);

    m_enemy->SetFlip(m_enemyFlip);
}



bool EnemyController::CheckCollision(const SDL_Rect& a, const SDL_Rect& b)
{
    return SDL_HasIntersection(&a, &b);
}

bool EnemyController::GroundCheck()
{
    if (!m_groundCollider) return false;
    return CheckCollision(m_enemyCollider, *m_groundCollider);
}

void EnemyController::SetOnDeathCallback(std::function<void()> callback)
{
    m_onDeathCallback = callback;
}

