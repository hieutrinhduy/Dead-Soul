#include "GSPlay.h"
#include "ResourceManagers.h"
#include "GameObject/MouseButton.h"
#include "GameObject/Sprite2D.h"
#include "GameObject/SpriteAnimation.h"
#include "GameObject/Camera.h"
#include "KeyState.h"
#include "Sound.h"
#include "TileMap.h"
#include <iostream>

SDL_Rect playerCollider;
SDL_Rect enemyCollider;
SDL_Rect groundCollider;

float gravityForce = 150.0f;
float jumpForce = -500.0f;
float velocityY = 0.0f;
float speed = 500.0f;
static float airTime = 0.0f;

int jumpCount = 0;
const int maxJump = 2;
bool isGrounded = false;

bool forceJumpAnimation = false;
float forceJumpTimer = 0.0f;
const float forceJumpDuration = 0.15f;

SDL_RendererFlip playerFlip = SDL_FLIP_NONE;

float lastMoveX = 0.0;

bool isAttacking = false;
float attackTimer = 0.0f;
const float attackDuration = 0.3f;
float lastAttackTime = -999.0f;
bool justDidAttack2 = false;

bool isRolling = false;
float rollTimer = 0.0f;
const float rollDuration = 0.35f;
const float rollSpeed = 900.0f;

bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return SDL_HasIntersection(&a, &b);
}

bool GroundCheck() {
    return CheckCollision(playerCollider, groundCollider);
}

GSPlay::GSPlay() {
    m_tileMap = std::make_shared<TileMap>();
    m_tileMapLoaded = false;
}

GSPlay::~GSPlay() {}

void GSPlay::Init() {
    auto tex1 = ResourceManagers::GetInstance()->GetTexture("background_layer_1.png");
    auto tex2 = ResourceManagers::GetInstance()->GetTexture("background_layer_2.png");
    auto tex3 = ResourceManagers::GetInstance()->GetTexture("background_layer_3.png");

    m_backgroundList.push_back(std::make_shared<Sprite2D>(tex1, SDL_FLIP_NONE));
    m_backgroundList.push_back(std::make_shared<Sprite2D>(tex2, SDL_FLIP_NONE));
    m_backgroundList.push_back(std::make_shared<Sprite2D>(tex3, SDL_FLIP_NONE));

    for (auto& bg : m_backgroundList) {
        bg->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
        bg->Set2DPosition(0, 0);
    }

    auto texture = ResourceManagers::GetInstance()->GetTexture("btn_close.tga");
    button = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
    button->SetSize(50, 50);
    button->Set2DPosition(SCREEN_WIDTH - 50, 10);
    button->SetOnClick([this]() { GameStateMachine::GetInstance()->PopState(); });
    m_listButton.push_back(button);

    texture = ResourceManagers::GetInstance()->GetTexture("_Idle.png");
    //texture = ResourceManagers::GetInstance()->GetTexture("_Run.png");
    obj = std::make_shared<SpriteAnimation>(texture, 1, 10, 1, 0.05f);
    obj->SetFlip(SDL_FLIP_NONE);
    obj->SetSize(360, 240);
    obj->Set2DPosition(240, 0);
    m_listAnimation.push_back(obj);
    playerCollider = { (int)obj->GetPosition().x, (int)obj->GetPosition().y, obj->GetWidth(), obj->GetHeight() };
    Camera::GetInstance()->SetTarget(obj);

    enemy = std::make_shared<SpriteAnimation>(texture, 2, 9, 6, 0.2f);
    enemy->SetFlip(SDL_FLIP_NONE);
    enemy->SetSize(100, 100);
    enemy->Set2DPosition(400, 400);
    enemyCollider = { (int)enemy->GetPosition().x, (int)enemy->GetPosition().y, enemy->GetWidth(), enemy->GetHeight() };

    groundCollider = { 0, 1000, 10000, 200 };

    m_KeyPress = 0;
    m_tileMap->LoadMap("Data/Map/map6.txt");
    if (!m_tileMapLoaded) {
        m_tileMap->LoadTiles(Renderer::GetInstance()->GetRenderer(), "Data/Map/tiles", 32);
        m_tileMapLoaded = true;
    }

    Camera::GetInstance()->SetLevelDimension(
        m_tileMap->GetWidth() * m_tileMap->GetTileSize(),
        m_tileMap->GetHeight() * m_tileMap->GetTileSize()
    );
}

void GSPlay::Exit() {}
void GSPlay::Pause() {}
void GSPlay::Resume() {}
void GSPlay::HandleEvents() {}

void GSPlay::HandleKeyEvents(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_a: m_KeyPress |= 1; break;
        case SDLK_s: m_KeyPress |= 1 << 1; break;
        case SDLK_d: m_KeyPress |= 1 << 2; break;
        case SDLK_w: m_KeyPress |= 1 << 3; break;
        case SDLK_SPACE:
            if (!isAttacking && jumpCount < maxJump && !isRolling) {
                velocityY = jumpForce;
                if (jumpCount == 1) {
                    airTime = 0.0f;

                    auto currentPos = obj->Get2DPosition();
                    obj = std::make_shared<SpriteAnimation>(
                        ResourceManagers::GetInstance()->GetTexture("_Jump.png"),
                        1, 3, 1, 0.05f
                    );
                    obj->SetSize(360, 240);
                    obj->Set2DPosition(currentPos.x, currentPos.y);
                    obj->SetFlip(playerFlip);

                    m_listAnimation.clear();
                    m_listAnimation.push_back(obj);
                    Camera::GetInstance()->SetTarget(obj);

                    forceJumpAnimation = true;
                    forceJumpTimer = forceJumpDuration;
                }
                jumpCount++;
                isGrounded = false;
            }
            break;
        case SDLK_LSHIFT:
            if (!isAttacking && !isRolling && isGrounded) {
                auto currentPos = obj->Get2DPosition();
                obj = std::make_shared<SpriteAnimation>(
                    ResourceManagers::GetInstance()->GetTexture("_Roll.png"),
                    1, 12, 1, 0.03f
                );
                obj->SetSize(360, 240);
                obj->Set2DPosition(currentPos.x, currentPos.y);
                obj->SetFlip(playerFlip);

                m_listAnimation.clear();
                m_listAnimation.push_back(obj);
                Camera::GetInstance()->SetTarget(obj);

                isRolling = true;
                rollTimer = rollDuration;
            }
            break;
        }
    }
    else if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
        case SDLK_a: m_KeyPress &= ~1; break;
        case SDLK_s: m_KeyPress &= ~(1 << 1); break;
        case SDLK_d: m_KeyPress &= ~(1 << 2); break;
        case SDLK_w: m_KeyPress &= ~(1 << 3); break;
        }
    }
    else if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            if (!isAttacking && isGrounded && !isRolling) {
                float now = SDL_GetTicks() / 1000.0f;
                auto currentPos = obj->Get2DPosition();

                if (!justDidAttack2 && (now - lastAttackTime <= 1.0f)) {
                    obj = std::make_shared<SpriteAnimation>(
                        ResourceManagers::GetInstance()->GetTexture("_Attack2.png"),
                        1, 6, 1, 0.08f
                    );
                    justDidAttack2 = true; // ghi nhớ đã Attack2
                }
                else {
                    obj = std::make_shared<SpriteAnimation>(
                        ResourceManagers::GetInstance()->GetTexture("_Attack.png"),
                        1, 4, 1, 0.08f
                    );
                    justDidAttack2 = false; // reset sau Attack1
                }

                obj->SetSize(360, 240);
                obj->Set2DPosition(currentPos.x, currentPos.y);
                obj->SetFlip(playerFlip);

                m_listAnimation.clear();
                m_listAnimation.push_back(obj);
                Camera::GetInstance()->SetTarget(obj);

                isAttacking = true;
                attackTimer = attackDuration;
                lastAttackTime = now;
            }
        }
    }

}

void GSPlay::HandleTouchEvents(SDL_Event& e) {
    for (auto& button : m_listButton)
        if (button->HandleTouchEvent(&e)) break;
}

void GSPlay::HandleMouseMoveEvents(int x, int y) {}

void GSPlay::Update(float deltaTime) {
    static auto idleTexture = ResourceManagers::GetInstance()->GetTexture("_Idle.png");
    static auto runTexture = ResourceManagers::GetInstance()->GetTexture("_Run.png");
    static auto jumpTexture = ResourceManagers::GetInstance()->GetTexture("_Jump.png");
    static auto jumpFallTexture = ResourceManagers::GetInstance()->GetTexture("_JumpFallInbetween.png");
    static auto fallTexture = ResourceManagers::GetInstance()->GetTexture("_Fall.png");

    static bool wasRunning = false;
    static bool wasGrounded = true;
    static bool wasJumpingUp = false;

    if (isAttacking) {
        attackTimer -= deltaTime;
        if (attackTimer <= 0.0f) {
            isAttacking = false;

            // === Force đổi về Idle ngay lập tức sau khi attack xong ===
            auto currentPos = obj->Get2DPosition();
            obj = std::make_shared<SpriteAnimation>(
                ResourceManagers::GetInstance()->GetTexture("_Idle.png"),
                1, 10, 1, 0.05f
            );
            obj->SetSize(360, 240);
            obj->Set2DPosition(currentPos.x, currentPos.y);
            obj->SetFlip(playerFlip);

            m_listAnimation.clear();
            m_listAnimation.push_back(obj);
            Camera::GetInstance()->SetTarget(obj);
        }
    }

    if (isRolling) {
        rollTimer -= deltaTime;
        float moveDir = (playerFlip == SDL_FLIP_NONE) ? 1.0f : -1.0f;
        float moveAmount = moveDir * rollSpeed * deltaTime;

        Vector2 pos = obj->Get2DPosition();
        pos.x += moveAmount;
        obj->Set2DPosition(pos.x, pos.y);
        playerCollider.x = (int)pos.x;

        if (rollTimer <= 0.0f) {
            isRolling = false;

            auto currentPos = obj->Get2DPosition();
            obj = std::make_shared<SpriteAnimation>(
                ResourceManagers::GetInstance()->GetTexture("_Idle.png"),
                1, 10, 1, 0.05f
            );
            obj->SetSize(360, 240);
            obj->Set2DPosition(currentPos.x, currentPos.y);
            obj->SetFlip(playerFlip);

            m_listAnimation.clear();
            m_listAnimation.push_back(obj);
            Camera::GetInstance()->SetTarget(obj);
        }
    }



    speed = 500.0f;

    Vector2 oldPos = obj->Get2DPosition();
    Vector2 newPos = oldPos;
    Vector2 velocity = { 0, 0 };

    if (!isAttacking && !isRolling) {  // đang attack thì không di chuyển
        if (m_KeyPress & 1) velocity.x -= 1.0f;
        if (m_KeyPress & (1 << 2)) velocity.x += 1.0f;
    }

    newPos.x += velocity.x * speed * deltaTime;

    if (velocity.x != 0 && !isAttacking) { // chỉ flip khi không attack
        playerFlip = (velocity.x > 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    }

    SDL_Rect newCollider = { (int)newPos.x, (int)newPos.y, obj->GetWidth(), obj->GetHeight() };
    if (!CheckCollision(newCollider, enemyCollider)) {
        obj->Set2DPosition(newPos.x, newPos.y);
        playerCollider = newCollider;
    }

    if (!isGrounded) airTime += deltaTime;
    else airTime = 0.0f;

    velocityY += gravityForce * (1.0f + airTime * 15.0f) * deltaTime;
    if (velocityY > 800) velocityY = 800;

    float posX = obj->Get2DPosition().x;
    float posY = obj->Get2DPosition().y + velocityY * deltaTime;

    playerCollider = { (int)posX, (int)posY, obj->GetWidth(), obj->GetHeight() };
    isGrounded = GroundCheck();

    if (isGrounded) {
        velocityY = 0;
        posY = groundCollider.y - obj->GetHeight() + 2;
        jumpCount = 0;
    }

    obj->Set2DPosition(posX, posY);
    playerCollider.y = (int)posY;

    bool isRunning = (velocity.x != 0);
    bool isJumpingUp = (!isGrounded && velocityY < 0);
    bool isFallingDown = (!isGrounded && velocityY >= 0);

    if (forceJumpAnimation) {
        forceJumpTimer -= deltaTime;
        if (forceJumpTimer <= 0.0f) {
            forceJumpAnimation = false;
        }
    }

    // === Animation Control ===
    if (!forceJumpAnimation && !isAttacking && !isRolling) { // Đang attack thì giữ nguyên anim attack
        auto currentPos = obj->Get2DPosition();

        if (!wasGrounded && isGrounded) {
            if (isRunning) {
                obj = std::make_shared<SpriteAnimation>(runTexture, 1, 10, 1, 0.05f);
            }
            else {
                obj = std::make_shared<SpriteAnimation>(idleTexture, 1, 10, 1, 0.05f);
            }
        }
        else if (wasGrounded && !isGrounded) {
            obj = std::make_shared<SpriteAnimation>(jumpTexture, 1, 3, 1, 0.05f);
        }
        else if (!isGrounded) {
            if (isJumpingUp && !wasJumpingUp) {
                obj = std::make_shared<SpriteAnimation>(jumpFallTexture, 1, 2, 1, 0.05f);
            }
            else if (isFallingDown && wasJumpingUp) {
                obj = std::make_shared<SpriteAnimation>(fallTexture, 1, 3, 1, 0.05f);
            }
        }
        else if (isGrounded) {
            if (isRunning != wasRunning) {
                if (isRunning) {
                    obj = std::make_shared<SpriteAnimation>(runTexture, 1, 10, 1, 0.05f);
                }
                else {
                    obj = std::make_shared<SpriteAnimation>(idleTexture, 1, 10, 1, 0.05f);
                }
            }
        }

        obj->SetSize(360, 240);
        obj->Set2DPosition(currentPos.x, currentPos.y);
        obj->SetFlip(playerFlip);

        m_listAnimation.clear();
        m_listAnimation.push_back(obj);

        Camera::GetInstance()->SetTarget(obj);
    }

    wasRunning = isRunning;
    wasGrounded = isGrounded;
    wasJumpingUp = isJumpingUp;

    for (auto& it : m_listButton) it->Update(deltaTime);
    for (auto& it : m_listAnimation) it->Update(deltaTime);
    Camera::GetInstance()->Update(deltaTime);
}



void GSPlay::Draw(SDL_Renderer* renderer)
{
    Vector2 camPos = Camera::GetInstance()->GetPosition();
    std::vector<float> parallaxFactors = { 0.2f, 0.5f, 0.8f };

    for (size_t i = 0; i < m_backgroundList.size(); ++i)
    {
        auto& bg = m_backgroundList[i];
        float factor = parallaxFactors[i];

        float bgWidth = SCREEN_WIDTH;
        float bgHeight = SCREEN_HEIDHT;

        float bgX = fmod(-camPos.x * factor, bgWidth);
        if (bgX > 0) bgX -= bgWidth;

        float bgY = 0.0f;

        for (int x = 0; x <= SCREEN_WIDTH / bgWidth + 1; ++x)
        {
            bg->Set2DPosition(bgX + x * bgWidth, bgY);
            bg->Draw(renderer);
        }
    }


    if (m_tileMap)
        m_tileMap->DrawMap(Renderer::GetInstance()->GetRenderer(), camPos.x, camPos.y, SCREEN_WIDTH, SCREEN_HEIDHT);

    for (auto& it : m_listButton) it->Draw(renderer);
    for (auto& it : m_listAnimation) it->Draw(renderer);

    enemy->Draw(renderer);

    auto ConvertToCameraView = [](SDL_Rect objCollider, Vector2 camPos) -> SDL_Rect {
        SDL_Rect out = objCollider;
        out.x -= camPos.x;
        out.y -= camPos.y;
        return out;
        };

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderDrawRect(renderer, &ConvertToCameraView(playerCollider, camPos));

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &ConvertToCameraView(enemyCollider, camPos));

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderDrawRect(renderer, &ConvertToCameraView(groundCollider, camPos));

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}



