#include "Boss1.h"
#include "ResourceManagers.h"
#include "Camera.h"
#include "Renderer.h"
#include <cstdlib>
#include <iostream>
#include <algorithm> 

int bossAttackIndex = 0;

int xAlign = 300;
int yAlign = 240;

Boss1::Boss1()
    : m_gravityForce(150.0f), m_velocityY(0.0f), m_speed(300.0f),
    m_direction(1),
    m_isAttacking(false), m_attackTimer(0.0f), m_attackCooldown(0.0f),
    m_isHit(false), m_hitTimer(0.0f),
    m_isDead(false),
    m_groundCollider(nullptr),
    m_playerCollider(nullptr),
    m_lostPlayerTimer(0.0f),
    m_currentState(BossState::Patrol),
    m_drawRay(false),
    m_maxHP(200), m_currentHP(200), m_damage(20),
    m_attackFlip(SDL_FLIP_HORIZONTAL)
{
}

Boss1::~Boss1()
{
}

void Boss1::Init(float pointA, float pointB, float posY)
{
    idleTexture = ResourceManagers::GetInstance()->GetTexture("boss_idle.png");
    runTexture = ResourceManagers::GetInstance()->GetTexture("boss_move.png");
    attack1Texture = ResourceManagers::GetInstance()->GetTexture("boss_attack1.png");
    attack2Texture = ResourceManagers::GetInstance()->GetTexture("boss_attack2.png");
    attack3Texture = ResourceManagers::GetInstance()->GetTexture("boss_attack3.png");
    hitTexture = ResourceManagers::GetInstance()->GetTexture("boss_hit.png");
    deadTexture = ResourceManagers::GetInstance()->GetTexture("boss_die.png");

    m_enemy = std::make_shared<SpriteAnimation>(runTexture, 1, 12, 1, 0.05f);
    m_enemy->SetSize(288 * 3, 160 * 3);
    m_enemy->Set2DPosition(pointA, posY);
    m_enemyCollider = { (int)m_enemy->GetPosition().x + xAlign, (int)m_enemy->GetPosition().y + yAlign, m_enemy->GetWidth() / 5, m_enemy->GetHeight()/2 };

    m_pointA = pointA;
    m_pointB = pointB;

    m_hpBarBg = { 0, 0, m_barWidth, m_barHeight };
    m_hpBarFill = m_hpBarBg;
}

void Boss1::SetGroundCollider(SDL_Rect* ground)
{
    m_groundCollider = ground;
}

void Boss1::SetPlayerCollider(const SDL_Rect* player)
{
    m_playerCollider = player;
}

SDL_Rect* Boss1::GetEnemyCollider() {
    return &m_enemyCollider;
}

void Boss1::TakeDamage(int damage)
{
    if (m_isDead) return;

    m_currentHP -= damage;

    float hpPercent = std::clamp((float)m_currentHP / m_maxHP, 0.0f, 1.0f);
    m_hpBarFill.w = static_cast<int>(m_barWidth * hpPercent);

    if (m_currentHP <= 0) {
        m_currentHP = 0;
        Die();
    }
    else {
        int currentFrame = m_enemy->GetCurrentFrame();
        if (m_isAttacking) return;
        if (!m_isAttacking) {
            m_isAttacking = false;
            m_isHit = true;
            m_hitTimer = 0.3f;
            ChangeAnimation(hitTexture, 7, 0.05f);
        }
    }
}


void Boss1::Die()
{
    m_isDead = true;
    m_deadTimer = 1.3f;
    m_finishDead = false;
    ChangeAnimation(deadTexture, 19, 0.1f);
}

void Boss1::DestroyEnemy()
{
    m_enemy = nullptr;
}

void Boss1::Update(float deltaTime)
{
    if (m_isDead) {
        m_enemyCollider = { 0, 0, 0, 0 };
        if (!m_finishDead) {
            m_deadTimer -= deltaTime;
            if (m_deadTimer <= 0.0f) {
                m_finishDead = true;
                DestroyEnemy();
            }
        }
        if (m_enemy) m_enemy->Update(deltaTime);
        return;
    }

    if (m_isHit) {
        m_hitTimer -= deltaTime;
        if (m_hitTimer <= 0.0f) {
            m_isHit = false;
            ChangeAnimation(runTexture, 12, 0.05f);
        }
        m_enemy->Update(deltaTime);
        return;
    }

    float posX = m_enemy->Get2DPosition().x;
    float posY = m_enemy->Get2DPosition().y;

    switch (m_currentState) {
    case BossState::Patrol:
        if (posX < m_pointA) {
            m_direction = 1;
            m_enemyFlip = SDL_FLIP_HORIZONTAL;
        }
        else if (posX > m_pointB) {
            m_direction = -1;
            m_enemyFlip = SDL_FLIP_NONE;
        }

        posX += m_direction * m_speed * deltaTime;

        if (m_direction == 1 && posX > m_pointB) {
            m_direction = -1;
            m_enemyFlip = SDL_FLIP_NONE;
        }
        else if (m_direction == -1 && posX < m_pointA) {
            m_direction = 1;
            m_enemyFlip = SDL_FLIP_HORIZONTAL;
        }

        if (!m_recentlyReturnedFromChase && DetectPlayer()) {
            m_currentState = BossState::Chase;
            m_lostPlayerTimer = 0.0f;
        }
        break;

    case BossState::Chase:
        if (DetectPlayer()) {
            m_lostPlayerTimer = 0.0f;
        }
        else {
            m_lostPlayerTimer += deltaTime;
            if (m_lostPlayerTimer >= 0.1f) {
                m_currentState = BossState::Patrol;
                ChangeAnimation(runTexture, 12, 0.05f);
                m_recentlyReturnedFromChase = true;
                break;
            }
        }

        if (m_playerCollider) {
            float enemyCenterX = posX + m_enemy->GetWidth() / 2;
            float playerCenterX = m_playerCollider->x + m_playerCollider->w / 2;
            float moveDir = (playerCenterX > enemyCenterX) ? 1.0f : -1.0f;

            if (!m_isAttacking)
                m_enemyFlip = (moveDir > 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

            float nextPosX = posX + moveDir * m_speed * 2 * deltaTime;
            if (nextPosX < m_pointA || nextPosX > m_pointB) {
                m_currentState = BossState::Patrol;
                ChangeAnimation(runTexture, 12, 0.05f);
                m_recentlyReturnedFromChase = true;

                if (posX < m_pointA + 10) {
                    m_direction = 1;
                    m_enemyFlip = SDL_FLIP_HORIZONTAL;
                }
                else if (posX > m_pointB - 10) {
                    m_direction = -1;
                    m_enemyFlip = SDL_FLIP_NONE;
                }
                break;
            }

            posX = nextPosX;

            float dx = playerCenterX - enemyCenterX;
            if (!m_isAttacking && std::abs(dx) < 130) {
                m_currentState = BossState::Attack;
                m_attackFlip = m_enemyFlip;
                m_isAttacking = true;

                int n = rand() % 3 + 1;
                if (n == 1) {
                    ChangeAnimation(attack1Texture, 15, 0.1f);
                    bossAttackIndex = 1;
                    m_attackTimer = 1.49f;
                }
                else if (n == 2) {
                    ChangeAnimation(attack2Texture, 16, 0.08f);
                    bossAttackIndex = 2;
                    m_attackTimer = 1.27f;
                }
                else {
                    ChangeAnimation(attack3Texture, 11, 0.13f);
                    bossAttackIndex = 3;
                    m_attackTimer = 1.42f;
                }
            }
        }
        break;

    case BossState::Attack:
        m_attackTimer -= deltaTime;

        if (m_enemy) {
            int currentFrame = m_enemy->GetCurrentFrame();

            switch (bossAttackIndex)
            {
            case 1:
                if (currentFrame == 10 && !m_didDamage && m_isAttacking) {
                    m_didDamage = true;
                    int offsetX = 0;
                    if (m_attackFlip == SDL_FLIP_NONE) {
                        offsetX = -200;
                    }
                    else
                    {
                        offsetX = 70;
                    }
                    m_attackBox = { m_enemyCollider.x + offsetX, m_enemyCollider.y, 400, m_enemy->GetHeight() / 2 };
                    if (m_playerCollider && CheckCollision(m_attackBox, *m_playerCollider)) {
                        if (m_onHitPlayer) m_onHitPlayer(m_damage);
                    }
                }
                break;
            case 2:
                if (currentFrame == 10 && !m_didDamage && m_isAttacking) {
                    m_didDamage = true;
                    int offsetX = 0;
                    if (m_attackFlip == SDL_FLIP_NONE) {
                        offsetX = -170;
                    }
                    else
                    {
                        offsetX = -150;
                    }
                    m_attackBox = { m_enemyCollider.x + offsetX, m_enemyCollider.y, 550, m_enemy->GetHeight() / 2 };
                    if (m_playerCollider && CheckCollision(m_attackBox, *m_playerCollider)) {
                        if (m_onHitPlayer) m_onHitPlayer(m_damage);
                    }
                }
                break;
            case 3:
                if ((currentFrame >= 3) && !m_didDamage && m_isAttacking) {
                    m_didDamage = true;
                    int offsetX = 0;
                    if (m_attackFlip == SDL_FLIP_NONE) {
                        offsetX = -200;
                    }
                    else
                    {
                        offsetX = 70;
                    }
                    m_attackBox = { m_enemyCollider.x + offsetX, m_enemyCollider.y + 50, 400, m_enemy->GetHeight() / 4 };
                    if (m_playerCollider && CheckCollision(m_attackBox, *m_playerCollider)) {
                        if (m_onHitPlayer) m_onHitPlayer(m_damage);
                    }
                }
                break;
            }
        }

        if (m_attackTimer <= 0.0f) {
            m_isAttacking = false;
            m_didDamage = false;

            bool isChasing = DetectPlayer();
            m_currentState = isChasing ? BossState::Chase : BossState::Patrol;
            ChangeAnimation(runTexture, 12, 0.05f);

            if (!isChasing) {
                if (posX < m_pointA + 10) {
                    m_direction = 1;
                    m_enemyFlip = SDL_FLIP_HORIZONTAL;
                }
                else if (posX > m_pointB - 10) {
                    m_direction = -1;
                    m_enemyFlip = SDL_FLIP_NONE;
                }
                else {
                    // Giữ nguyên direction, nhưng update flip theo direction
                    m_enemyFlip = (m_direction == 1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                }
            }
            else {
                float enemyCenterX = posX + m_enemy->GetWidth() / 2;
                float playerCenterX = m_playerCollider->x + m_playerCollider->w / 2;
                float dx = playerCenterX - enemyCenterX;
                m_enemyFlip = (dx > 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            }
        }


        break;
    }

    m_enemy->Set2DPosition(posX, posY);
    m_enemy->SetFlip((m_currentState == BossState::Attack) ? m_attackFlip : m_enemyFlip);
    m_enemyCollider.x = (int)posX;
    m_enemy->Set2DPosition(posX, posY);
    m_enemyCollider = { (int)m_enemy->GetPosition().x + xAlign, (int)m_enemy->GetPosition().y + yAlign, m_enemy->GetWidth() / 4, m_enemy->GetHeight() / 2 };

    m_hpBarBg.x = m_enemyCollider.x + m_enemyCollider.w / 2 - m_barWidth / 2;
    m_hpBarBg.y = m_enemyCollider.y - 10;
    m_hpBarFill = m_hpBarBg;
    float hpPercent = std::clamp((float)m_currentHP / m_maxHP, 0.0f, 1.0f);
    m_hpBarFill.w = static_cast<int>(m_barWidth * hpPercent);

    if (m_lostPlayerTimer > 0.0f) m_lostPlayerTimer -= deltaTime;
    if (m_recentlyReturnedFromChase) {
        m_recentlyReturnedTimer += deltaTime;
        if (m_recentlyReturnedTimer >= 0.3f) {
            m_recentlyReturnedFromChase = false;
            m_recentlyReturnedTimer = 0.0f;
        }
    }

    m_enemy->Update(deltaTime);
}



void Boss1::Draw(SDL_Renderer* renderer)
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

bool Boss1::DetectPlayer()
{
    if (!m_playerCollider) return false;
    float enemyCenterX = m_enemyCollider.x + m_enemyCollider.w / 2.0f;
    float playerCenterX = m_playerCollider->x + m_playerCollider->w / 2.0f;
    float distance = std::abs(enemyCenterX - playerCenterX);
    return distance < 1000.0f;
}

void Boss1::ChangeAnimation(std::shared_ptr<TextureManager> texture, int frameCount, float frameTime)
{
    if (!m_enemy) return;

    Vector2 pos = m_enemy->Get2DPosition();

    m_enemy = std::make_shared<SpriteAnimation>(texture, 1, frameCount, 1, frameTime);
    m_enemy->SetSize(288 * 3, 160 * 3);
    m_enemy->Set2DPosition(pos.x, pos.y);

    m_enemy->SetFlip(m_enemyFlip);
}



bool Boss1::CheckCollision(const SDL_Rect& a, const SDL_Rect& b)
{
    return SDL_HasIntersection(&a, &b);
}

bool Boss1::GroundCheck()
{
    if (!m_groundCollider) return false;
    return CheckCollision(m_enemyCollider, *m_groundCollider);
}