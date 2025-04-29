#include "EnemyController.h"
#include "ResourceManagers.h"
#include "Camera.h"
#include "Renderer.h"
#include <cstdlib>
#include <iostream>

int attackIndex = 0;
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
    m_maxHP(100), m_currentHP(100), m_damage(1)
{
}

EnemyController::~EnemyController()
{
}

void EnemyController::Init(float pointA, float pointB)
{
    idleTexture = ResourceManagers::GetInstance()->GetTexture("Skeleton_01_White_Idle.png");
    runTexture = ResourceManagers::GetInstance()->GetTexture("Skeleton_01_White_Walk.png");
    attack1Texture = ResourceManagers::GetInstance()->GetTexture("Skeleton_01_White_Attack1.png");
    attack2Texture = ResourceManagers::GetInstance()->GetTexture("Skeleton_01_White_Attack2.png");
    hitTexture = ResourceManagers::GetInstance()->GetTexture("Skeleton_01_White_Hurt.png");
    deadTexture = ResourceManagers::GetInstance()->GetTexture("Skeleton_01_White_Die.png");

    m_enemy = std::make_shared<SpriteAnimation>(runTexture, 1, 10, 1, 0.05f);
    m_enemy->SetSize(96 * 2, 64 * 2);
    m_enemy->Set2DPosition(pointA, -500);
    m_enemyCollider = { (int)m_enemy->GetPosition().x + 50, (int)m_enemy->GetPosition().y, m_enemy->GetWidth()/2, m_enemy->GetHeight() };

    m_pointA = pointA;
    m_pointB = pointB;
    m_enemyFlip = SDL_FLIP_NONE;
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

void EnemyController::TakeDamage(int damage)
{
    if (m_isDead) return;

    m_currentHP -= damage;

    if (m_currentHP <= 0)
    {
        m_currentHP = 0;
        Die();
    }
    else
    {
        m_isHit = true;
        m_hitTimer = 0.25f;
        ChangeAnimation(hitTexture, 5, 0.05);
    }
}

void EnemyController::Die()
{
    m_isDead = true;
    m_deadTimer = 1.3f;
    m_finishDead = false;
    ChangeAnimation(deadTexture, 13, 0.1f);
}

void EnemyController::Update(float deltaTime)
{
    if (m_isDead) {
        if (!m_finishDead) {
            m_deadTimer -= deltaTime;
            if (m_deadTimer <= 0.0f) {
                m_finishDead = true;
            }
        }
        m_enemy->Update(deltaTime);
        return;
    }

    if (m_isHit) {
        m_hitTimer -= deltaTime;
        if (m_hitTimer <= 0.0f) {
            m_isHit = false;
            std::cout << "Stop hit" << std::endl;
            ChangeAnimation(runTexture, 10, 0.05f);
        }
        m_enemy->Update(deltaTime);
        return;
    }

    float posX = m_enemy->Get2DPosition().x;
    float posY = m_enemy->Get2DPosition().y;

    switch (m_currentState)
    {
    case EnemyState::Patrol:
    {
        // Nếu đang ngoài vùng patrol thì move về trong vùng
        if (posX < m_pointA)
        {
            m_direction = 1;
            m_enemyFlip = SDL_FLIP_NONE;
        }
        else if (posX > m_pointB)
        {
            m_direction = -1;
            m_enemyFlip = SDL_FLIP_HORIZONTAL;
        }

        posX += m_direction * m_speed * deltaTime;

        // Nếu enemy đi quá A-B, chỉ đổi hướng, không gán posX luôn
        if (m_direction == 1 && posX > m_pointB)
        {
            m_direction = -1;
            m_enemyFlip = SDL_FLIP_HORIZONTAL;
        }
        else if (m_direction == -1 && posX < m_pointA)
        {
            m_direction = 1;
            m_enemyFlip = SDL_FLIP_NONE;
        }

        if (DetectPlayer())
        {
            m_currentState = EnemyState::Chase;
            m_lostPlayerTimer = 0.0f;
        }
    }
    break;

    case EnemyState::Chase:
    {
        if (DetectPlayer()) {
            m_lostPlayerTimer = 0.0f;
        }
        else {
            m_lostPlayerTimer += deltaTime;
            if (m_lostPlayerTimer >= 0.1f) {
                m_currentState = EnemyState::Patrol;
                ChangeAnimation(runTexture, 10, 0.05f);
                // Flip lại theo patrol khi mất Player
                m_enemyFlip = (m_direction == 1) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
                break;
            }
        }

        if (m_playerCollider) {
            float distanceX = m_playerCollider->x - posX;
            std::cout << distanceX << std::endl;

            int attackDistance = INT_MAX;
            if (m_enemyFlip == SDL_FLIP_NONE) {
                attackDistance = 150;
            }
            else
            {
                attackDistance = 50;
            }
            if (abs(distanceX) < attackDistance) {
                m_currentState = EnemyState::Attack;

                if (rand() % 2 == 0) {
                    ChangeAnimation(attack1Texture, 10, 0.1f);
                    attackIndex = 1;
                    m_attackTimer = 1.5f;
                }
                else {
                    ChangeAnimation(attack2Texture, 9, 0.08f);
                    attackIndex = 2; 
                    m_attackTimer = 1.5f;
                }
            }
            else {
                if (distanceX > 0) {
                    m_enemyFlip = SDL_FLIP_NONE;
                    posX += m_speed * 2 * deltaTime;
                }
                else {
                    m_enemyFlip = SDL_FLIP_HORIZONTAL;
                    posX -= m_speed * 2 * deltaTime;
                }
            }
        }
    }
    break;

    case EnemyState::Attack:
    {
        m_attackTimer -= deltaTime;


        if (attackIndex == 1) {
            if (m_attackTimer <= 0.9f && m_attackTimer >= 0.8f) // (Tùy chỉnh lại nếu cần)
            {
                int offsetX = 60;
                if (m_enemyFlip == SDL_FLIP_HORIZONTAL) offsetX = -m_enemy->GetWidth() / 2;

                m_attackBox = {
                    m_enemyCollider.x + offsetX,
                    m_enemyCollider.y + m_enemy->GetHeight() / 4,
                    50,
                    m_enemy->GetHeight() / 2
                };

                bool isHit = CheckCollision(m_attackBox, *m_playerCollider);

                if (m_playerCollider && CheckCollision(m_attackBox, *m_playerCollider)) {
                    if (m_onHitPlayer) {
                        m_onHitPlayer(m_damage);
                    }
                }
            }
        }
        else if (attackIndex == 2) {
            if (m_attackTimer <= 1.1f && m_attackTimer >= 1.0f) // (Tùy chỉnh lại nếu cần)
            {
                int offsetX = 60;
                if (m_enemyFlip == SDL_FLIP_HORIZONTAL) offsetX = -m_enemy->GetWidth() / 2;

                m_attackBox = {
                    m_enemyCollider.x + offsetX,
                    m_enemyCollider.y + m_enemy->GetHeight() / 4,
                    50,
                    m_enemy->GetHeight() / 2
                };

                bool isHit = CheckCollision(m_attackBox, *m_playerCollider);
                std::cout << "[DEBUG] CheckCollision result = " << (isHit ? "TRUE" : "FALSE") << std::endl;


                if (m_playerCollider && CheckCollision(m_attackBox, *m_playerCollider)) {
                    if (m_onHitPlayer) {
                        m_onHitPlayer(m_damage);
                    }
                }

            }
        }

        if (m_attackTimer <= 0.0f) {
            if (DetectPlayer()) {
                m_currentState = EnemyState::Chase;
                ChangeAnimation(runTexture, 10, 0.05f);
            }
            else {
                m_currentState = EnemyState::Patrol;
                ChangeAnimation(runTexture, 10, 0.05f);
            }
        }
    }
    break;

    }


    m_enemy->Set2DPosition(posX, posY);
    m_enemy->SetFlip(m_enemyFlip);
    m_enemyCollider.x = (int)posX;

    // Gravity
    if (!m_isGrounded) m_airTime += deltaTime;
    else m_airTime = 0.0f;

    m_velocityY += m_gravityForce * (1.0f + m_airTime * 10.0f) * deltaTime;
    if (m_velocityY > 800) m_velocityY = 800;

    posY += m_velocityY * deltaTime;
    m_enemyCollider = { (int)posX + 50, (int)posY, m_enemy->GetWidth()/2, m_enemy->GetHeight() };
    m_isGrounded = GroundCheck();

    if (m_isGrounded) {
        m_velocityY = 0;
        posY = m_groundCollider->y - m_enemy->GetHeight() + 2;
    }

    m_enemy->Set2DPosition(posX, posY);
    m_enemyCollider = { (int)posX + 50, (int)posY, m_enemy->GetWidth() / 2, m_enemy->GetHeight() };

    m_enemy->Update(deltaTime);
}

void EnemyController::Draw(SDL_Renderer* renderer)
{
    if (m_isDead && m_finishDead) return;

    if (m_enemy)
        m_enemy->Draw(renderer);

    Vector2 camPos = Camera::GetInstance()->GetPosition();

    SDL_Rect drawCollider = m_enemyCollider;
    drawCollider.x -= (int)camPos.x;
    drawCollider.y -= (int)camPos.y;

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderDrawRect(renderer, &drawCollider);

    SDL_Rect drawAttackBox = m_attackBox;
    drawAttackBox.x -= (int)camPos.x;
    drawAttackBox.y -= (int)camPos.y;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &drawAttackBox);


    // Debug Raycast
    if (m_drawRay) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Màu vàng
        SDL_RenderDrawLine(
            renderer,
            (int)(m_lastRayStart.x - Camera::GetInstance()->GetPosition().x),
            (int)(m_lastRayStart.y - Camera::GetInstance()->GetPosition().y),
            (int)(m_lastRayEnd.x - Camera::GetInstance()->GetPosition().x),
            (int)(m_lastRayEnd.y - Camera::GetInstance()->GetPosition().y)
        );
    }


}

bool EnemyController::DetectPlayer()
{
    if (!m_playerCollider) return false;

    float enemyCenterX = m_enemyCollider.x + m_enemyCollider.w / 2.0f;
    float playerCenterX = m_playerCollider->x + m_playerCollider->w / 2.0f;

    float distance = std::abs(enemyCenterX - playerCenterX);

    return distance < 500.0f;
}





void EnemyController::ChangeAnimation(std::shared_ptr<TextureManager> texture, int frameCount, float frameTime)
{
    if (!m_enemy) return;

    auto temp = m_enemy->Get2DPosition();
    SDL_FPoint pos;
    pos.x = temp.x;
    pos.y = temp.y;

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
