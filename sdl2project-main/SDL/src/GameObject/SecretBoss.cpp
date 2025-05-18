#include "SecretBoss.h"
#include "ResourceManagers.h"
#include "Camera.h"
#include "Renderer.h"
#include <cstdlib>
#include <iostream>
#include <algorithm> 
#include "BossBullet.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define M_PI_2 (M_PI / 2.0f)
#define M_PI_4 (M_PI / 4.0f)

int SecretBossAttackIndex = 0;

int secretBossxAlign = 300;
int secretBossyAlign = 170;

extern std::vector<std::shared_ptr<BossBullet>> g_bossBullets;

ScecretBoss::ScecretBoss()
    : m_gravityForce(150.0f), m_velocityY(0.0f), m_speed(300.0f),
    m_direction(1),
    m_isAttacking(false), m_attackTimer(0.0f), m_attackCooldown(0.0f),
    m_isHit(false), m_hitTimer(0.0f),
    m_isDead(false),
    m_groundCollider(nullptr),
    m_playerCollider(nullptr),
    m_lostPlayerTimer(0.0f),
    m_currentState(SecretBossState::Patrol),
    m_drawRay(false),
    m_maxHP(200), m_currentHP(200), m_damage(20),
    m_attackFlip(SDL_FLIP_HORIZONTAL)
{
}

ScecretBoss::~ScecretBoss()
{
}

void ScecretBoss::Init(float pointA, float pointB, float posY)
{
    idleTexture = ResourceManagers::GetInstance()->GetTexture("boss2_idle.png");
    runTexture = ResourceManagers::GetInstance()->GetTexture("boss2_move.png");
    flyTexture = ResourceManagers::GetInstance()->GetTexture("boss2_fly.png");
    attack1Texture = ResourceManagers::GetInstance()->GetTexture("boss2_attack.png");
    attack2Texture = ResourceManagers::GetInstance()->GetTexture("boss2_attack2.png");
    hitTexture = ResourceManagers::GetInstance()->GetTexture("boss2_hit.png");
    deadTexture = ResourceManagers::GetInstance()->GetTexture("boss2_dead.png");

    m_enemy = std::make_shared<SpriteAnimation>(runTexture, 1, 12, 1, 0.05f);
    m_enemy->SetSize(288 * 3, 160 * 3);
    m_enemy->Set2DPosition(pointB, posY);
    m_enemyCollider = { (int)m_enemy->GetPosition().x + secretBossxAlign, (int)m_enemy->GetPosition().y + secretBossyAlign, m_enemy->GetWidth() / 5, m_enemy->GetHeight() / 2 };

    m_pointA = pointA;
    m_pointB = pointB;

    m_hpBarBg = { 0, 0, m_barWidth, m_barHeight };
    m_hpBarFill = m_hpBarBg;
}

void ScecretBoss::SetGroundCollider(SDL_Rect* ground)
{
    m_groundCollider = ground;
}

void ScecretBoss::SetPlayerCollider(const SDL_Rect* player)
{
    m_playerCollider = player;
}

SDL_Rect* ScecretBoss::GetEnemyCollider() {
    return &m_enemyCollider;
}

void ScecretBoss::TakeDamage(int damage)
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
            m_hitTimer = 0.24f;
            ChangeAnimation(hitTexture, 5, 0.05f);
        }
    }
}


void ScecretBoss::Die()
{
    m_isDead = true;
    m_deadTimer = 0.9f;
    m_finishDead = false;
    ChangeAnimation(deadTexture, 10, 0.1f);

    if (m_onDeathCallback) {
        m_onDeathCallback();
    }

    if (m_spawnExpCallback) {
        Vector2 basePos = m_enemy->Get2DPosition();
        basePos.x += 400;
        basePos.y += 400;

        for (int i = 0; i < m_exp_orb_number; ++i) {
            Vector2 offset = {
                basePos.x + (rand() % 310 - 150),
                basePos.y
            };
            m_spawnExpCallback(offset);
        }
    }
}

void ScecretBoss::DestroyEnemy()
{
    m_enemy = nullptr;
}

// Cập nhật Boss1::Update giống EnemyController để xử lý flip ổn định
// Boss1.cpp - Đã chỉnh sửa phần Update để giống logic EnemyController hoàn toàn, bao gồm flip, moveDir và detect hướng chính xác

void ScecretBoss::Update(float deltaTime)
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
    case SecretBossState::Patrol:
        if (posX < m_pointA) {
            m_direction = 1;
        }
        else if (posX > m_pointB) {
            m_direction = -1;
        }

        posX += m_direction * m_speed * deltaTime;

        if (m_direction == 1 && posX > m_pointB) {
            m_direction = -1;
        }
        else if (m_direction == -1 && posX < m_pointA) {
            m_direction = 1;
        }

        if (!m_recentlyReturnedFromChase && DetectPlayer()) {
            m_currentState = SecretBossState::Chase;
            m_lostPlayerTimer = 0.0f;
        }
        break;

    case SecretBossState::Chase:
        if (DetectPlayer()) {
            m_lostPlayerTimer = 0.0f;
        }
        else {
            m_lostPlayerTimer += deltaTime;
            if (m_lostPlayerTimer >= 0.1f) {
                m_currentState = SecretBossState::Patrol;
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
                m_enemyFlip = (moveDir > 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

            float nextPosX = posX + moveDir * m_speed * 2 * deltaTime;
            if (nextPosX < m_pointA || nextPosX > m_pointB) {
                m_currentState = SecretBossState::Patrol;
                ChangeAnimation(runTexture, 12, 0.05f);
                m_recentlyReturnedFromChase = true;

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

            float dx = playerCenterX - enemyCenterX;
            if (!m_isAttacking && std::abs(dx) < 130) {
                m_currentState = SecretBossState::Attack;
                m_attackFlip = m_enemyFlip;
                m_isAttacking = true;

                int r = rand() % 5; // r ∈ [0, 4]
                int n;
                if (r < 2)       n = 1;  // 40% xác suất
                else if (r < 4)  n = 2;  // 40% xác suất
                else             n = 3;  // 20% xác suất

                if (n == 1) {
                    ChangeAnimation(attack1Texture, 7, 0.1f);
                    SecretBossAttackIndex = 1;
                    m_attackTimer = 0.69f;
                }
                else if (n == 2) {
                    ChangeAnimation(attack2Texture, 9, 0.08f);
                    SecretBossAttackIndex = 2;
                    m_attackTimer = 0.71f;
                }
                else
                {
                    ChangeAnimation(flyTexture, 6, 0.08f);
                    SecretBossAttackIndex = 3;
                    m_attackTimer = 10;
                    m_originalY = posY;
                }
            }
        }
        break;

    case SecretBossState::Attack:
        m_attackTimer -= deltaTime;

        if (m_enemy) {
            int currentFrame = m_enemy->GetCurrentFrame();

            switch (SecretBossAttackIndex)
            {
            case 1:
                if (currentFrame == 3 && !m_didDamage && m_isAttacking) {
                    m_didDamage = true;
                    int offsetX = 0;
                    if (m_attackFlip == SDL_FLIP_HORIZONTAL) {
                        offsetX = -200;
                    }
                    else
                    {
                        offsetX = 70;
                    }
                    m_attackBox = { m_enemyCollider.x + offsetX, m_enemyCollider.y, 400, m_enemy->GetHeight() / 4 };
                    if (m_playerCollider && CheckCollision(m_attackBox, *m_playerCollider)) {
                        if (m_onHitPlayer) m_onHitPlayer(m_damage);
                    }
                }
                break;
            case 2:
                if (currentFrame == 4 && !m_didDamage && m_isAttacking) {
                    m_didDamage = true;
                    int offsetX = 0;
                    if (m_attackFlip == SDL_FLIP_HORIZONTAL) {
                        offsetX = -350;
                    }
                    else
                    {
                        offsetX = 70;
                    }
                    m_attackBox = { m_enemyCollider.x + offsetX, m_enemyCollider.y, 550, m_enemy->GetHeight() / 4 };
                    if (m_playerCollider && CheckCollision(m_attackBox, *m_playerCollider)) {
                        if (m_onHitPlayer) m_onHitPlayer(m_damage);
                    }
                }
                break;
            case 3:
                if (!m_isFlying) {
                    posY += m_flySpeed * deltaTime;

                    if (posY <= 0) {
                        posY = 0;
                        m_isFlying = true;
                        m_shootTimer = 0.5f;
                    }
                }
                else if (!m_shootingStarted && !m_returning && !m_shootPhase2 && !m_bulletHellStarted) {
                    m_shootTimer -= deltaTime;
                    if (m_shootTimer <= 0.0f) {
                        m_shootingStarted = true;
                        m_shotsFired = 0;
                    }
                }
                else if (m_shootingStarted && !m_shootPhase2) {
                    m_shootTimer -= deltaTime;
                    if (m_shootTimer <= 0.0f && m_shotsFired < m_totalShots) {
                        float angleRad = 3.665f + m_shotsFired * ((5.76f - 3.665f) / 9.0f); // từ trái sang phải
                        ShootInAngle(angleRad);
                        m_shootTimer = 0.15f;
                        m_shotsFired++;
                    }

                    if (m_shotsFired >= m_totalShots) {
                        m_shootingStarted = false;
                        m_shootTimer = m_betweenShotsDelay; // delay trước đợt 2
                        m_shotsFired = 0;
                        m_shootPhase2 = true;
                    }
                }
                else if (m_shootPhase2) {
                    m_shootTimer -= deltaTime;

                    if (m_shootTimer <= 0.0f && m_shotsFired < m_totalShots) {
                        float angleRad = 5.76f - m_shotsFired * ((5.76f - 3.665f) / 9.0f); // từ phải sang trái
                        ShootInAngle(angleRad);
                        m_shootTimer = 0.15f;
                        m_shotsFired++;
                    }

                    if (m_shotsFired >= m_totalShots) {
                        m_shootPhase2 = false;

                        // BẮT ĐẦU BULLET HELL SAU ĐỢT 2
                        m_bulletHellStarted = true;
                        m_bulletHellTimer = 0.5f;
                        m_bulletHellWaves = 0;
                    }
                }
                else if (m_bulletHellStarted) {
                    m_bulletHellTimer -= deltaTime;
                    if (m_bulletHellTimer <= 0.0f && m_bulletHellWaves < m_maxBulletHellWaves) {
                        switch (m_bulletHellWaves) {
                        case 0: m_currentPattern = BulletPatternType::Circle; break;
                        case 1: m_currentPattern = BulletPatternType::Spiral; break;
                        case 2: m_currentPattern = BulletPatternType::Wave; break;
                        case 3: m_currentPattern = BulletPatternType::Random; break;
                        case 4: m_currentPattern = BulletPatternType::CrossX; break;
                        }

                        FireBulletPattern(m_bulletHellWaves);

                        m_bulletHellTimer = 0.7f;
                        m_bulletHellWaves++;
                    }


                    if (m_bulletHellWaves >= m_maxBulletHellWaves) {
                        m_bulletHellStarted = false;
                        m_returning = true;
                    }
                }
                else if (m_returning) {
                    posY += 400.0f * deltaTime;
                    if (posY >= m_originalY) {
                        posY = m_originalY;
                        m_returning = false;
                        m_isFlying = false;

                        m_isAttacking = false;
                        m_didDamage = false;
                        m_currentState = DetectPlayer() ? SecretBossState::Chase : SecretBossState::Patrol;
                        ChangeAnimation(runTexture, 12, 0.05f);
                    }
                }
                break;

            }
        }
        if (m_attackTimer <= 0.0f) {
            m_isAttacking = false;
            m_didDamage = false;

            bool isChasing = DetectPlayer();
            m_currentState = isChasing ? SecretBossState::Chase : SecretBossState::Patrol;
            ChangeAnimation(runTexture, 12, 0.05f);

            // Update flip NGAY LẬP TỨC, không chờ đến cuối frame
            if (isChasing) {
                float enemyCenterX = posX + m_enemy->GetWidth() / 2;
                float playerCenterX = m_playerCollider->x + m_playerCollider->w / 2;
                float dx = playerCenterX - enemyCenterX;
                m_enemyFlip = (dx > 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            }
            else {
                m_enemyFlip = (m_direction == 1) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            }

            // Gán lại flip ngay để tránh dùng m_attackFlip thêm 1 frame nữa
            m_enemy->SetFlip(m_enemyFlip);


            if (!isChasing) {
                if (posX < m_pointA + 10) {
                    m_direction = 1;
                }
                else if (posX > m_pointB - 10) {
                    m_direction = -1;
                }

                // cập nhật hướng flip ngay lập tức
                m_enemyFlip = (m_direction == 1) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            }
            else {
                float enemyCenterX = posX + m_enemy->GetWidth() / 2;
                float playerCenterX = m_playerCollider->x + m_playerCollider->w / 2;
                float dx = playerCenterX - enemyCenterX;
                m_enemyFlip = (dx > 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            }
        }
        break;
    }

    m_enemy->Set2DPosition(posX, posY);
    m_enemy->SetFlip((m_currentState == SecretBossState::Attack) ? m_attackFlip : m_enemyFlip);
    m_enemyCollider.x = (int)posX;
    m_enemy->Set2DPosition(posX, posY);
    m_enemyCollider = { (int)m_enemy->GetPosition().x + secretBossxAlign, (int)m_enemy->GetPosition().y + secretBossyAlign, m_enemy->GetWidth() / 4, m_enemy->GetHeight() / 2 };

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
    if (!m_isAttacking) {
        m_enemyFlip = (m_direction == 1) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    }
    m_enemy->Update(deltaTime);
}



void ScecretBoss::Draw(SDL_Renderer* renderer)
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

bool ScecretBoss::DetectPlayer()
{
    if (!m_playerCollider) return false;
    if (m_playerCollider->w == 1) return false;
    float enemyCenterX = m_enemyCollider.x + m_enemyCollider.w / 2.0f;
    float playerCenterX = m_playerCollider->x + m_playerCollider->w / 2.0f;
    float distance = std::abs(enemyCenterX - playerCenterX);
    float enemyCenterY = m_enemyCollider.y + m_enemyCollider.h / 2.0f;
    float playerCenterY = m_playerCollider->y + m_playerCollider->h / 2.0f;
    float distanceY = std::abs(enemyCenterY - playerCenterY);
    if (distanceY > 100) return false;
    return distance < 1000.0f;
}

void ScecretBoss::ChangeAnimation(std::shared_ptr<TextureManager> texture, int frameCount, float frameTime)
{
    if (!m_enemy) return;

    Vector2 pos = m_enemy->Get2DPosition();

    m_enemy = std::make_shared<SpriteAnimation>(texture, 1, frameCount, 1, frameTime);
    m_enemy->SetSize(288 * 3, 160 * 3);
    m_enemy->Set2DPosition(pos.x, pos.y);

    m_enemy->SetFlip(m_enemyFlip);
}



bool ScecretBoss::CheckCollision(const SDL_Rect& a, const SDL_Rect& b)
{
    return SDL_HasIntersection(&a, &b);
}

bool ScecretBoss::GroundCheck()
{
    if (!m_groundCollider) return false;
    return CheckCollision(m_enemyCollider, *m_groundCollider);
}

void ScecretBoss::ShootInAngle(float angleRad)
{
    Vector2 dir = { cosf(angleRad), -sinf(angleRad) };

    float speed = 600.0f;
    Vector2 spawnPos = {
        m_enemy->GetPosition().x + m_enemy->GetWidth() / 2,
        m_enemy->GetPosition().y + m_enemy->GetHeight() / 2
    };

    auto bullet = std::make_shared<BossBullet>(spawnPos, dir, speed, m_damage/2);
    bullet->SetOnHitCallback(m_onHitPlayer); // nếu có
    g_bossBullets.push_back(bullet);         // vector bạn quản lý boss bullets
}

void ScecretBoss::FireBulletPattern(int waveIndex)
{
    switch (m_currentPattern)
    {
    case BulletPatternType::Circle:
    {
        const int bullets = 20;
        for (int i = 0; i < bullets; ++i) {
            float angle = (2 * M_PI * i) / bullets;
            ShootInAngle(angle);
        }
        break;
    }

    case BulletPatternType::Spiral:
    {
        const int bullets = 20;
        float offset = waveIndex * 0.3f; // càng về sau xoắn càng nhiều
        for (int i = 0; i < bullets; ++i) {
            float angle = offset + (2 * M_PI * i) / bullets;
            ShootInAngle(angle);
        }
        break;
    }

    case BulletPatternType::Wave:
    {
        float center = M_PI / 2.0f; // bắn xuống
        float spread = M_PI / 10.0f;
        for (int i = -3; i <= 3; ++i) {
            float angle = center + i * spread;
            ShootInAngle(-angle);
        }
        break;
    }

    case BulletPatternType::Random:
    {
        for (int i = 0; i < 10; ++i) {
            float angle = static_cast<float>(rand()) / RAND_MAX * 2 * M_PI;
            ShootInAngle(angle);
        }
        break;
    }

    case BulletPatternType::CrossX:
    {
        float angles[] = {
            0, M_PI_2, M_PI, 3 * M_PI_2,          // + shape
            M_PI_4, 3 * M_PI_4, 5 * M_PI_4, 7 * M_PI_4 // X shape
        };
        for (float angle : angles) {
            ShootInAngle(angle);
        }
        break;
    }
    }
}

void ScecretBoss::SetOnDeathCallback(std::function<void()> callback)
{
    m_onDeathCallback = callback;
}

void ScecretBoss::SetSpawnExpCallback(std::function<void(Vector2)> callback)
{
    m_spawnExpCallback = callback;
}
