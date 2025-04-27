#include "PlayerController.h"
#include "ResourceManagers.h"
#include "Camera.h"
#include "Renderer.h"

float PlayerController::m_airTime = 0.0f;

PlayerController::PlayerController()
    : m_gravityForce(150.0f), m_jumpForce(-500.0f), m_velocityY(0.0f), m_speed(500.0f),
    m_jumpCount(0), m_maxJump(2), m_isGrounded(false),
    m_forceJumpAnimation(false), m_forceJumpTimer(0.0f), m_forceJumpDuration(0.15f),
    m_playerFlip(SDL_FLIP_NONE),
    m_KeyPress(0),
    m_isAttacking(false), m_attackTimer(0.0f), m_attackDuration(0.3f), m_lastAttackTime(-999.0f), m_justDidAttack2(false),
    m_isRolling(false), m_rollTimer(0.0f), m_rollDuration(0.35f), m_rollSpeed(900.0f),
    m_groundCollider(nullptr), m_enemyCollider(nullptr)
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::Init()
{
    idleTexture = ResourceManagers::GetInstance()->GetTexture("_Idle.png");
    runTexture = ResourceManagers::GetInstance()->GetTexture("_Run.png");
    jumpTexture = ResourceManagers::GetInstance()->GetTexture("_Jump.png");
    jumpFallTexture = ResourceManagers::GetInstance()->GetTexture("_JumpFallInbetween.png");
    fallTexture = ResourceManagers::GetInstance()->GetTexture("_Fall.png");
    rollTexture = ResourceManagers::GetInstance()->GetTexture("_Roll.png");
    attackTexture = ResourceManagers::GetInstance()->GetTexture("_Attack.png");
    attack2Texture = ResourceManagers::GetInstance()->GetTexture("_Attack2.png");

    m_player = std::make_shared<SpriteAnimation>(idleTexture, 1, 10, 1, 0.05f);
    m_player->SetSize(360, 240);
    m_player->Set2DPosition(240, 0);
    m_playerCollider = { (int)m_player->GetPosition().x, (int)m_player->GetPosition().y, m_player->GetWidth(), m_player->GetHeight() };

    Camera::GetInstance()->SetTarget(m_player);
}

void PlayerController::SetGroundCollider(SDL_Rect* ground)
{
    m_groundCollider = ground;
}

void PlayerController::SetEnemyCollider(SDL_Rect* enemy)
{
    m_enemyCollider = enemy;
}

void PlayerController::HandleKeyEvents(SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_a: m_KeyPress |= 1; break;
        case SDLK_d: m_KeyPress |= (1 << 2); break;
        case SDLK_SPACE:
            if (!m_isAttacking && m_jumpCount < m_maxJump && !m_isRolling) {
                m_velocityY = m_jumpForce;
                if (m_jumpCount == 1) { // Lần nhảy thứ 2
                    m_airTime = 0.0f;

                    // ➡️ Force chạy anim Jump ngay lập tức
                    ChangeAnimation(jumpTexture, 3, 0.05f);

                    m_forceJumpAnimation = true;
                    m_forceJumpTimer = m_forceJumpDuration;
                }
                m_jumpCount++;
                m_isGrounded = false;
            }
            break;
        case SDLK_LSHIFT:
            if (!m_isAttacking && !m_isRolling && m_isGrounded) {
                ChangeAnimation(rollTexture, 12, 0.03f);
                m_isRolling = true;
                m_rollTimer = m_rollDuration;
            }
            break;
        }
    }
    else if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
        case SDLK_a: m_KeyPress &= ~1; break;
        case SDLK_d: m_KeyPress &= ~(1 << 2); break;
        }
    }
}

void PlayerController::HandleMouseEvents(SDL_Event& e)
{
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            if (!m_isAttacking && m_isGrounded && !m_isRolling) {
                float now = SDL_GetTicks() / 1000.0f;
                if (!m_justDidAttack2 && (now - m_lastAttackTime <= 1.0f)) {
                    ChangeAnimation(attack2Texture, 6, 0.08f);
                    m_justDidAttack2 = true;
                }
                else {
                    ChangeAnimation(attackTexture, 4, 0.08f);
                    m_justDidAttack2 = false;
                }
                m_isAttacking = true;
                m_attackTimer = m_attackDuration;
                m_lastAttackTime = now;
            }
        }
    }
}

void PlayerController::Update(float deltaTime)
{
    if (!m_player) return;

    if (m_isAttacking) {
        m_attackTimer -= deltaTime;
        if (m_attackTimer <= 0.0f) {
            m_isAttacking = false;
            ChangeAnimation(idleTexture, 10, 0.05f);
        }
    }

    if (m_isRolling) {
        m_rollTimer -= deltaTime;
        float moveDir = (m_playerFlip == SDL_FLIP_NONE) ? 1.0f : -1.0f;
        float moveAmount = moveDir * m_rollSpeed * deltaTime;
        auto temp = m_player->Get2DPosition();
        SDL_FPoint pos;
        pos.x = temp.x + moveAmount;
        pos.y = temp.y;
        m_player->Set2DPosition(pos.x, pos.y);
        m_playerCollider.x = (int)pos.x;

        if (m_rollTimer <= 0.0f) {
            m_isRolling = false;
            ChangeAnimation(idleTexture, 10, 0.05f);
        }
    }

    m_speed = 500.0f;
    auto temp = m_player->Get2DPosition();
    SDL_FPoint oldPos;
    oldPos.x = temp.x;
    oldPos.y = temp.y;
    SDL_FPoint newPos = oldPos;
    SDL_FPoint velocity = { 0, 0 };

    if (!m_isAttacking && !m_isRolling) {
        if (m_KeyPress & 1) velocity.x -= 1.0f;
        if (m_KeyPress & (1 << 2)) velocity.x += 1.0f;
    }

    newPos.x += velocity.x * m_speed * deltaTime;

    if (velocity.x != 0 && !m_isAttacking) {
        m_playerFlip = (velocity.x > 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    }

    m_player->SetFlip(m_playerFlip);

    SDL_Rect newCollider = { (int)newPos.x, (int)newPos.y, m_player->GetWidth(), m_player->GetHeight() };
    if (!m_enemyCollider || !CheckCollision(newCollider, *m_enemyCollider)) {
        m_player->Set2DPosition(newPos.x, oldPos.y);
        m_playerCollider.x = (int)newPos.x;
    }

    if (!m_isGrounded) m_airTime += deltaTime;
    else m_airTime = 0.0f;

    m_velocityY += m_gravityForce * (1.0f + m_airTime * 15.0f) * deltaTime;
    if (m_velocityY > 800) m_velocityY = 800;

    float posX = m_player->Get2DPosition().x;
    float posY = m_player->Get2DPosition().y + m_velocityY * deltaTime;

    m_playerCollider = { (int)posX, (int)posY, m_player->GetWidth(), m_player->GetHeight() };
    m_isGrounded = GroundCheck();

    if (m_isGrounded) {
        m_velocityY = 0;
        posY = m_groundCollider->y - m_player->GetHeight() + 2;
        m_jumpCount = 0;
    }

    m_player->Set2DPosition(posX, posY);
    m_playerCollider.y = (int)posY;

    static bool wasRunning = false;
    static bool wasGrounded = true;
    static bool wasJumpingUp = false;

    bool isRunning = (velocity.x != 0);
    bool isJumpingUp = (!m_isGrounded && m_velocityY < 0);
    bool isFallingDown = (!m_isGrounded && m_velocityY >= 0);

    // Giảm forceJumpTimer
    if (m_forceJumpAnimation) {
        m_forceJumpTimer -= deltaTime;
        if (m_forceJumpTimer <= 0.0f) {
            m_forceJumpAnimation = false;
        }
    }

    if (!m_forceJumpAnimation && !m_isAttacking && !m_isRolling) {
        if (!wasGrounded && m_isGrounded) {
            if (isRunning)
                ChangeAnimation(runTexture, 10, 0.05f);
            else
                ChangeAnimation(idleTexture, 10, 0.05f);
        }
        else if (wasGrounded && !m_isGrounded) {
            ChangeAnimation(jumpTexture, 3, 0.05f);
        }
        else if (!m_isGrounded) {
            if (isJumpingUp && !wasJumpingUp)
                ChangeAnimation(jumpFallTexture, 2, 0.05f);
            else if (isFallingDown && wasJumpingUp)
                ChangeAnimation(fallTexture, 3, 0.05f);
        }
        else if (m_isGrounded && (wasRunning != isRunning)) {
            if (isRunning)
                ChangeAnimation(runTexture, 10, 0.05f);
            else
                ChangeAnimation(idleTexture, 10, 0.05f);
        }
    }

    wasRunning = isRunning;
    wasGrounded = m_isGrounded;
    wasJumpingUp = isJumpingUp;

    if (m_player)
        m_player->Update(deltaTime);
}

void PlayerController::Draw(SDL_Renderer* renderer)
{
    if (m_player)
        m_player->Draw(renderer);
}

void PlayerController::ChangeAnimation(std::shared_ptr<TextureManager> texture, int frameCount, float frameTime)
{
    if (!m_player) return;

    auto temp = m_player->Get2DPosition();
    SDL_FPoint pos;
    pos.x = temp.x;
    pos.y = temp.y;
    m_player = std::make_shared<SpriteAnimation>(texture, 1, frameCount, 1, frameTime);
    m_player->SetSize(360, 240);
    m_player->Set2DPosition(pos.x, pos.y);
    m_player->SetFlip(m_playerFlip);

    Camera::GetInstance()->SetTarget(m_player);
}

bool PlayerController::CheckCollision(const SDL_Rect& a, const SDL_Rect& b)
{
    return SDL_HasIntersection(&a, &b);
}

bool PlayerController::GroundCheck()
{
    if (!m_groundCollider) return false;
    return CheckCollision(m_playerCollider, *m_groundCollider);
}
