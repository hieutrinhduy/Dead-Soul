#include "PlayerController.h"
#include "ResourceManagers.h"
#include "Camera.h"
#include "Renderer.h"
#include <iostream>
#include <algorithm> 
#include "GhostWarriorRed.h"
#include "RangedEnemy.h"
#include "PlayerBullet.h"


constexpr int PLAYER_SIZE = 150, PLAYER_STARTING_POSITION_X = 200, PLAYER_STARTING_POSITION_Y = 100; // sprite size, measure unit: screen pixel
constexpr float COLLIDER_X = 0.35f, COLLIDER_Y = 0.1f, COLLIDER_W = 0.24f, COLLIDER_H = 0.9f;      // collider : sprite_size ratio
constexpr float COLLIDER_X_FLIP = 1 - COLLIDER_X - COLLIDER_W;                                     // collider-x when flipped


float PlayerController::m_airTime = 0.0f;
float m_deadTimer;
bool m_finishDead;
//TTF_Font* m_font = nullptr;
extern std::vector<std::shared_ptr<PlayerBullet>> g_playerBullets;

/* ----------------------------------------------------------------------------------------------------------------------- */
//      CONSTRUCTOR & DECONSTRUCTOR  ->  INIT  ->  HANDLE KEY_EVENTS  ->  HANDLE MOUSE_EVENTS  ->  UPDATE  ->  DRAW
/* ----------------------------------------------------------------------------------------------------------------------- */

PlayerController::PlayerController()
    : m_gravityForce(150.0f), m_jumpForce(-500.0f), m_velocityY(0.0f), m_speed(700.0f),
    m_jumpCount(0), m_maxJump(2), m_isGrounded(false),
    m_forceJumpAnimation(false), m_forceJumpTimer(0.0f), m_forceJumpDuration(0.15f),
    m_playerFlip(SDL_FLIP_NONE),
    m_KeyPress(0),
    m_isAttacking(false), m_attackTimer(0.0f), m_attackDuration(0.39f), m_lastAttackTime(-999.0f), m_justDidAttack2(false), m_justDidAttack3(false),
    m_isRolling(false), m_rollTimer(0.0f), m_rollDuration(0.35f), m_rollSpeed(900.0f),
    m_groundCollider(nullptr),
    /*m_maxHP(5), m_currentHP(100),*/
    m_isDead(false),
    m_isHit(false), m_hitTimer(0.0f)
{
}

PlayerController::~PlayerController()
{
    /*if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }*/
}



void PlayerController::Init()
{
    idleTexture = ResourceManagers::GetInstance()->GetTexture("NewPlayerIdle.png");
    runTexture = ResourceManagers::GetInstance()->GetTexture("NewPlayerRun.png");
    jumpTexture = ResourceManagers::GetInstance()->GetTexture("NewPlayerJump2.png");
    jumpOnAirTexture = ResourceManagers::GetInstance()->GetTexture("NewPlayerJumpOnAir.png");
    jumpFallTexture = ResourceManagers::GetInstance()->GetTexture("_JumpFallInbetween.png");
    landTexture = ResourceManagers::GetInstance()->GetTexture("NewPlayerLand.png");
    fallTexture = ResourceManagers::GetInstance()->GetTexture("_Fall.png");
    rollTexture = ResourceManagers::GetInstance()->GetTexture("_Roll.png");
    hurtTexture = ResourceManagers::GetInstance()->GetTexture("player_is_hit.png");
    deadTexture = ResourceManagers::GetInstance()->GetTexture("player_is_dead.png");
    //For Default Weapon (m_currentWeaponIndex = 0)
    attackTexture = ResourceManagers::GetInstance()->GetTexture("NewPlayerAttack1.png");
    attack2Texture = ResourceManagers::GetInstance()->GetTexture("NewPlayerAttack2.png");
    attack3Texture = ResourceManagers::GetInstance()->GetTexture("NewPlayerAttack3.png");
    //For Weapon Arrow (m_currentWeaponIndex = 1)
    arrowAttackTexture = ResourceManagers::GetInstance()->GetTexture("bow_attack.png");

    // SPRITE:
    m_player = std::make_shared<SpriteAnimation>(idleTexture, 1, 2, 1, 0.5f);
    m_player->SetSize(PLAYER_SIZE, PLAYER_SIZE);
    m_player->Set2DPosition(PLAYER_STARTING_POSITION_X, PLAYER_STARTING_POSITION_Y);

    // COLLIDER:
    Vector2 pos = m_player->Get2DPosition();
    m_playerCollider = {
        (int)(PLAYER_SIZE * COLLIDER_X + pos.x),
        (int)(PLAYER_SIZE * COLLIDER_Y + pos.y),
        (int)(PLAYER_SIZE * COLLIDER_W),
        (int)(PLAYER_SIZE * COLLIDER_H),
    };

    m_enoughMP = true;
    m_enoughSP = true;

    // set CAMERA'S TARGET:
    Camera::GetInstance()->SetTarget(m_player);
}



void PlayerController::HandleKeyEvents(SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_a: m_KeyPress |= 1; break;
        case SDLK_d: m_KeyPress |= (1 << 2); break;
        case SDLK_SPACE:
            if (!m_isAttacking && m_jumpCount < m_maxJump && !m_isRolling && !m_isHit) {
                m_velocityY = m_jumpForce;
                ChangeAnimation(jumpTexture, 2, 0.1f);
                if (m_jumpCount == 1) {
                    m_airTime = 0.0f;
                    m_forceJumpAnimation = true;
                    m_forceJumpTimer = m_forceJumpDuration;
                }
                m_jumpCount++;
                m_isGrounded = false;
                std::cout << topLimit << std::endl;
                std::cout << m_velocityY << std::endl;
            }
            break;
        case SDLK_LSHIFT:
            if (!m_isAttacking && !m_isRolling && m_isGrounded && !m_isHit && m_enoughSP) {
                //Giả sử lộn mất 30Sp
                SetSpLose(30);
                ChangeAnimation(rollTexture, 12, 0.03f);
                m_isRolling = true;
                m_rollTimer = m_rollDuration;
            }
            break;
        case SDLK_1:
            SetWeaponIndex(0);
            break;
        case SDLK_2:
            SetWeaponIndex(1);
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
            switch (m_currentWeaponIndex)
            {
            case 0:
                if (!m_isAttacking && m_isGrounded && !m_isRolling && !m_isHit && m_enoughMP) {
                    float now = SDL_GetTicks() / 1000.0f;
                    //Cac don danh cach nhau 1s
                    if (!m_justDidAttack2 && !m_justDidAttack3 && (now - m_lastAttackTime <= 1.0f)) {
                        // Giá sử mất 30 MP khi tấn công
                        SetMpLose(30);
                        ChangeAnimation(attack2Texture, 4, 0.1f);
                        m_justDidAttack2 = true;
                        m_justDidAttack3 = false;
                    }
                    else if (!m_justDidAttack3 && (now - m_lastAttackTime <= 1.0f)) {
                        // Giá sử mất 30 MP khi tấn công
                        SetMpLose(30);
                        ChangeAnimation(attack3Texture, 4, 0.1f);
                        m_justDidAttack3 = true;
                        m_justDidAttack2 = false;
                    }
                    else {
                        // Giá sử mất 30 MP khi tấn công
                        SetMpLose(30);
                        ChangeAnimation(attackTexture, 4, 0.1f);
                        m_justDidAttack2 = false;
                        m_justDidAttack3 = false;
                    }
                    m_isAttacking = true;
                    m_attackTimer = m_attackDuration;
                    m_lastAttackTime = now;
                }
                break;
            case 1:
                if (!m_isAttacking && m_isGrounded && !m_isRolling && !m_isHit && m_enoughMP) {
                    float now = SDL_GetTicks() / 1000.0f;
                    //Cac don danh cach nhau 1s
                    //if (now - m_lastAttackTime <= 1.0f) {
                    //}
                    // Giá sử mất 30 SP khi tấn công
                    SetMpLose(30);
                    ChangeAnimation(arrowAttackTexture, 6, 0.1f);
                    m_isAttacking = true;
                    m_attackTimer = m_attackDuration;
                    m_lastAttackTime = now;
                }
            }
        }
    }
}



void PlayerController::Update(float deltaTime)
{
    if (!m_player) return;

    //Nếu chết thì đếm ngược thời gian trước khi dừng draw
    if (m_isDead) {
        if (!m_finishDead) {
            m_deadTimer -= deltaTime;
            if (m_deadTimer <= 0.0f) {
                m_finishDead = true;
            }
        }
        m_player->Update(deltaTime);
        return;
    }

    //Nếu bị hit, sau khi bị hit chuyển về trạng thái chạy hoặc idle
    if (m_isHit) {
        m_hitTimer -= deltaTime;
        if (m_hitTimer <= 0.0f) {
            m_isHit = false;
            if ((m_KeyPress & 1) || (m_KeyPress & (1 << 2))) {
                ChangeAnimation(runTexture, 6, 0.07f);
            }
            else {
                ChangeAnimation(idleTexture, 2, 0.5f);
            }
        }
        m_player->Update(deltaTime);
        return;
    }

    //Sau khi tấn công 1 khoảng thời gian trở về trạng thái idle
    if (m_isAttacking) {
        m_attackTimer -= deltaTime;
        if (m_attackTimer <= 0.0f) {
            m_didDamage = false;
            m_isAttacking = false;
            ChangeAnimation(idleTexture, 2, 0.5f);
        }
    }

    //Khi đang lộn, nếu flip trái thì lộn trái, nếu đang flip phải thì lộn phải
    if (m_isRolling) {
        m_rollTimer -= deltaTime;
        float moveDir = (m_playerFlip == SDL_FLIP_NONE) ? 1.0f : -1.0f;
        float moveAmount = moveDir * m_rollSpeed * deltaTime;
        m_player->MoveX(moveAmount);

        //Lộn xong về idle
        if (m_rollTimer <= 0.0f) {
            m_isRolling = false;
            ChangeAnimation(idleTexture, 2, 0.5f);
        }
    } 

    //Tốc độ nhân vật
    m_speed = 500.0f;
    auto temp = m_player->Get2DPosition(); //Biến tạm lưu vị trí nhân vật
    SDL_FPoint oldPos; //Biến tạm lưu vị trí hiện tại nhân vật
    oldPos.x = temp.x;
    oldPos.y = temp.y;
    SDL_FPoint newPos = oldPos;
    SDL_FPoint velocity = { 0, 0 };

    //Tính toán vị trí tiếp theo khi nhấn nút di chuyển (Không phải hiện tại nhé)
    if (!m_isAttacking && !m_isRolling &&!m_isHit) {
        if (m_KeyPress & 1) velocity.x -= 1.0f;
        if (m_KeyPress & (1 << 2)) velocity.x += 1.0f;
    }
    if (velocity.x < 0) { newPos.x += std::max(velocity.x * m_speed * deltaTime, -leftLimit); }
    else newPos.x += std::min(velocity.x * m_speed * deltaTime, rightLimit);

    //Flip nhân vật
    if (velocity.x != 0 && !m_isAttacking) {
        m_playerFlip = (velocity.x > 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    }
    m_player->SetFlip(m_playerFlip);

    //Tính toán collider tiếp theo khi nhấn nút di chuyển (Không phải hiện tại nhé)
    SDL_Rect newCollider;
    if (m_playerFlip == SDL_FLIP_NONE) {
        newCollider = { (int)newPos.x + 40, (int)newPos.y , m_player->GetWidth() - 100, m_player->GetHeight()};
    }
    else
    {
        newCollider = { (int)newPos.x + 50, (int)newPos.y , m_player->GetWidth() - 100, m_player->GetHeight()};
    }

    //Set vị trí và collider x mới
    m_player->Set2DPosition(newPos.x, oldPos.y);
    m_playerCollider.x = (int)newPos.x;

    //Nếu ko ở trên mặt đất thì tăng dần biến m_airTime, nếu trên mặt đất thì reset = 0
    if (!m_isGrounded) 
    {
        //Check trạng thái nhảy
        if (m_velocityY <= 0) {
            m_isJumping = true;
            m_isFalling = false;
        }
        else
        {
            m_isJumping = false;
            m_isFalling = true;
        }
        m_airTime += deltaTime;
    } 
    else m_airTime = 0.0f;

    //Chỉnh tốc độ rơi dựa trên m_airTime (thời gian trên không), càng lâu thì rơi càng nhanh, tối đa vận tốc rơi là 800
    m_velocityY += m_gravityForce * (1.0f + m_airTime * 15.0f) * deltaTime;
    if (m_velocityY > 800) m_velocityY = 800;


    // Tính vị trí mới theo trục Y dựa vào trọng lực (trên kia là tính x)
    float posX = m_player->Get2DPosition().x;
    float posY;
    if (m_velocityY < 0) {
        if (-topLimit > m_velocityY * deltaTime) {
            posY = m_player->Get2DPosition().y - topLimit;
            m_velocityY = -m_velocityY;
        }
        else posY = m_player->Get2DPosition().y + m_velocityY * deltaTime;
    }
    else posY = m_player->Get2DPosition().y + std::min(m_velocityY * deltaTime, bottomLimit);

    // Cập nhật tạm collider theo vị trí dự đoán
    if (m_playerFlip == SDL_FLIP_NONE) {
        m_playerCollider = { (int)posX + 40, (int)posY , m_player->GetWidth() - 100, m_player->GetHeight() };
    }
    else
    {
        m_playerCollider = { (int)posX + 50, (int)posY, m_player->GetWidth() - 100, m_player->GetHeight() };
    }

    // just landing on the ground
    if (m_isGrounded) {
        m_velocityY = 0;
        m_jumpCount = 0;
        m_isJumping = false;
        m_isFalling = false;
    }

    // Cập nhật vị trí và collider mới sau khi xử lý rơi và mặt đất
    m_player->Set2DPosition(posX, posY);
    //Cap nhat velocity X
    m_velocityX = (newPos.x - oldPos.x) / deltaTime;
    if (m_playerFlip == SDL_FLIP_NONE) {
        m_playerCollider = { (int)posX + 40, (int)posY, m_player->GetWidth() - 100, m_player->GetHeight() };
    }
    else
    {
        m_playerCollider = { (int)posX + 50, (int)posY, m_player->GetWidth() - 100, m_player->GetHeight() };
    }

    // ---- Xử lý trạng thái chuyển động để đổi animation ----

    static bool wasRunning = false;
    static bool wasGrounded = true;
    static bool wasJumpingUp = false;

    bool isRunning = (velocity.x != 0);
    bool isJumpingUp = (!m_isGrounded && m_velocityY < 0);
    bool isFallingDown = (!m_isGrounded && m_velocityY >= 0);

    // Đếm ngược thời gian chạy anim nhảy
    if (m_forceJumpAnimation) {
        m_forceJumpTimer -= deltaTime;
        if (m_forceJumpTimer <= 0.0f) {
            m_forceJumpAnimation = false;
        }
    }
    // không đang tấn công hay lộn, thì xét đổi animation
    if (!m_forceJumpAnimation && !m_isAttacking && !m_isRolling) {
        // Vừa tiếp đất
        if (!wasGrounded && m_isGrounded) {
            if (isRunning)
                ChangeAnimation(runTexture, 6, 0.07f);
            else
                ChangeAnimation(idleTexture, 2, 0.5f);
        }
        // Đang trên không
        else if (!m_isGrounded) {
            if (m_airTime > 0.19f) {
                ChangeAnimation(jumpOnAirTexture, 1, 0.05f);
            }
        }
        // Trạng thái chạy thay đổi (từ chạy quá idle, ngược lại)
        else if (m_isGrounded && (wasRunning != isRunning)) {
            if (isRunning)
                ChangeAnimation(runTexture, 6, 0.07f);
            else
                ChangeAnimation(idleTexture, 2, 0.5f);
        }
    }

    wasRunning = isRunning;
    wasGrounded = m_isGrounded;
    wasJumpingUp = isJumpingUp;

    // Khi đang Attack
    int currentFrame = m_player->GetCurrentFrame();
    switch (m_currentWeaponIndex)
    {
    case 0:
        if (m_isAttacking && !m_enemyColliders.empty())
        {
            float elapsedAttackTime = m_attackDuration - m_attackTimer;
            m_canDealDamage = false;


            //Attack1
            if (!m_justDidAttack2 && !m_justDidAttack3)
            {
                if (currentFrame == 2 && !m_didDamage) {
                    m_canDealDamage = true;
                    m_didDamage = true;
                }
            }
            //Attack2
            else if(!m_justDidAttack3)
            {
                if (currentFrame == 2 && !m_didDamage) {
                    m_canDealDamage = true;
                    m_didDamage = true;
                }
            }
            //Attack3
            else
            {
                if (currentFrame == 2 && !m_didDamage) {
                    m_canDealDamage = true;
                    m_didDamage = true;
                }
            }
        }
        else
        {
            m_canDealDamage = false;
        }
        break;
    case 1:
        if (currentFrame == 5 && !m_didDamage && m_isAttacking) {
            ShootProjectile();
            m_didDamage = true;
        }
    }

    if (m_player)
        m_player->Update(deltaTime);
}



void PlayerController::Draw(SDL_Renderer* renderer)
{
    if (m_isDead && m_finishDead)
    {
        DestroyPlayer();
        return;
    }

    if (m_player)
        m_player->Draw(renderer);

    Vector2 camPos = Camera::GetInstance()->GetPosition();

    //SDL_Rect drawCollider = m_playerCollider;
    //drawCollider.x -= (int)camPos.x;
    //drawCollider.y -= (int)camPos.y;

    //SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    //SDL_RenderDrawRect(renderer, &drawCollider);

    if (m_canDealDamage)
    {
        SDL_Rect attackBox;
        if (m_playerFlip == SDL_FLIP_NONE)
            attackBox.x = m_playerCollider.x + m_playerCollider.w * 0.6f;
        else
            attackBox.x = m_playerCollider.x - 100;

        attackBox.y = m_playerCollider.y + m_playerCollider.h * 0.3f;
        attackBox.w = 150;
        attackBox.h = 60;

        attackBox.x -= (int)camPos.x;
        attackBox.y -= (int)camPos.y;

        //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        //SDL_RenderDrawRect(renderer, &attackBox);

        int damageAmount = 10;
        for (size_t i = 0; i < m_enemyColliders.size(); ++i)
        {
            SDL_Rect enemyScreen = *m_enemyColliders[i];
            enemyScreen.x -= (int)camPos.x;
            enemyScreen.y -= (int)camPos.y;

            if (CheckCollision(attackBox, enemyScreen))
            {
                if (i < m_onHitEnemyList.size()) {
                    if (m_onHitEnemyList[i]) {
                        m_onHitEnemyList[i](damageAmount);
                    }
                }

                if (m_onScreenShake)
                    m_onScreenShake();
            }
        }
    }
}


/* ------------------------- */
//      other FUNCTIONS
/* ------------------------- */

void PlayerController::TakeDamage(int damage)
{
    if (m_isDead || m_isHit || m_isRolling) return;

    SetHpLose(damage);
    //m_currentHP -= damage;
    //if (m_currentHP <= 0)
    //{
    //    m_currentHP = 0;
    //    Die();
    //    return;
    //}

    // Bị hit
    m_isHit = true;
    m_hitTimer = 0.20f;
    ChangeAnimation(hurtTexture, 3, 0.07f);
}

void PlayerController::Die()
{
    m_isDead = true;
    m_deadTimer = 0.29f;
    m_finishDead = false;
    ChangeAnimation(deadTexture, 6, 0.05f);
}


void PlayerController::ChangeAnimation(std::shared_ptr<TextureManager> texture, int frameCount, float frameTime)
{
    if (!m_player) return;

    auto temp = m_player->Get2DPosition();
    SDL_FPoint pos;
    pos.x = temp.x;
    pos.y = temp.y;
    m_player = std::make_shared<SpriteAnimation>(texture, 1, frameCount, 1, frameTime);
    m_player->SetSize(180, 180);
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


void PlayerController::SetGroundCollider(SDL_Rect* ground)
{
    m_groundCollider = ground;
}

void PlayerController::SetEnemyColliders(const std::vector<SDL_Rect*>& enemies)
{
    m_enemyColliders = enemies;
}


void PlayerController::DestroyPlayer()
{
    m_playerCollider = { 0, 0, 0, 0 };
}

void PlayerController::SetOnHitEnemyCallbacks(const std::vector<std::function<void(int)>>& callbacks)
{
    m_onHitEnemyList = callbacks;
}

void PlayerController::ShootProjectile()
{
    Vector2 pos = { static_cast<float>(m_playerCollider.x), static_cast<float>(m_playerCollider.y) };
    int direction = (m_playerFlip == SDL_FLIP_NONE) ? 1 : -1;
    pos.x += (direction == 1) ? m_playerCollider.w : -32;

    auto bullet = std::make_shared<PlayerBullet>(pos, 1500, direction, 1);

    //bullet->SetOnHitEnemyCallback([this](int damage) {

    //    });

    g_playerBullets.push_back(bullet);
}



void PlayerController::LoseTheirFooting() {
    m_isRolling = false;
    m_jumpCount = 1;
    m_velocityY = 600;
    ChangeAnimation(jumpOnAirTexture, 1, 0.05f);
}