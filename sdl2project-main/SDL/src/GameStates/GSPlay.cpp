// === GSPlay.cpp (Camera follow fix applied) ===
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

GSPlay::GSPlay()
{
    m_tileMap = std::make_shared<TileMap>();
    m_tileMapLoaded = false;
}

GSPlay::~GSPlay() {}


//player collider section
SDL_Rect playerCollider;
bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return SDL_HasIntersection(&a, &b);
}
bool isGrounded = false;

//Player physics
float gravityForce = 150;
float jumpForce = -200.0f;
float velocityY = 0.0f;
float speed = 500.0f;

SDL_Rect enemyCollider;
SDL_Rect groundCollider;

bool GroundCheck() {
    if (CheckCollision(playerCollider, groundCollider)) {
        std::cout << "Is Grounded" << std::endl;
        return true;
    } 
    std::cout << "Is not Grounded" << std::endl;
    return false;
}

void GSPlay::Init()
{
    auto tex1 = ResourceManagers::GetInstance()->GetTexture("background_layer_1.png");
    auto bg1 = std::make_shared<Sprite2D>(tex1, SDL_FLIP_NONE);
    bg1->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
    bg1->Set2DPosition(0, 0);
    m_backgroundList.push_back(bg1);

    auto tex2 = ResourceManagers::GetInstance()->GetTexture("background_layer_2.png");
    auto bg2 = std::make_shared<Sprite2D>(tex2, SDL_FLIP_NONE);
    bg2->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
    bg2->Set2DPosition(0, 0);
    m_backgroundList.push_back(bg2);

    auto tex3 = ResourceManagers::GetInstance()->GetTexture("background_layer_3.png");
    auto bg3 = std::make_shared<Sprite2D>(tex3, SDL_FLIP_NONE);
    bg3->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
    bg3->Set2DPosition(0, 0);
    m_backgroundList.push_back(bg3);

    auto texture = ResourceManagers::GetInstance()->GetTexture("btn_close.tga");
    button = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
    button->SetSize(50, 50);
    button->Set2DPosition(SCREEN_WIDTH - 50, 10);
    button->SetOnClick([this]() {
        GameStateMachine::GetInstance()->PopState();
        });
    m_listButton.push_back(button);

    //Spawn Player
    texture = ResourceManagers::GetInstance()->GetTexture("Actor1_2.tga");
    obj = std::make_shared<SpriteAnimation>(texture, 2, 9, 6, 0.2f);
    obj->SetFlip(SDL_FLIP_HORIZONTAL);
    obj->SetSize(80, 100);
    obj->Set2DPosition(240, 0);
    m_listAnimation.push_back(obj);
    playerCollider = { (int)obj->GetPosition().x, (int)obj->GetPosition().y, obj->GetWidth(), obj->GetHeight() };
    Camera::GetInstance()->SetTarget(obj);

    //Spawn Enemy
    texture = ResourceManagers::GetInstance()->GetTexture("Actor1_2.tga");
    enemy = std::make_shared<SpriteAnimation>(texture, 2, 9, 6, 0.2f);
    enemy->SetFlip(SDL_FLIP_HORIZONTAL);
    enemy->SetSize(100, 100);
    enemy->Set2DPosition(400, 400);
    enemyCollider = { (int)enemy->GetPosition().x, (int)enemy->GetPosition().y, enemy->GetWidth(), enemy->GetHeight() };

    //Spawn GroundCollider
    groundCollider = { 0, 500,10000, 200 };

    m_KeyPress = 0;
    m_tileMap->LoadMap("Data/Map/map6.txt");
    if (!m_tileMapLoaded)
    {
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

void GSPlay::HandleKeyEvents(SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_a: m_KeyPress |= 1; break;
        case SDLK_s: m_KeyPress |= 1 << 1; break;
        case SDLK_d: m_KeyPress |= 1 << 2; break;
        case SDLK_w: m_KeyPress |= 1 << 3; break;
        case SDLK_SPACE:
            if (isGrounded) {
                velocityY = jumpForce;
                isGrounded = false;
            }
            break;
        }
    }
    else if (e.type == SDL_KEYUP)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_a: m_KeyPress &= ~1; break;
        case SDLK_s: m_KeyPress &= ~(1 << 1); break;
        case SDLK_d: m_KeyPress &= ~(1 << 2); break;
        case SDLK_w: m_KeyPress &= ~(1 << 3); break;
        }
    }

}

void GSPlay::HandleTouchEvents(SDL_Event& e)
{
    for (auto& button : m_listButton)
    {
        if (button->HandleTouchEvent(&e)) break;
    }
}

void GSPlay::HandleMouseMoveEvents(int x, int y) {}

void PlayerMovement() {

}

void GSPlay::Update(float deltaTime)
{
    //PlayerMovement
    speed = 500.0f;

    Vector2 oldPos = obj->Get2DPosition();
    Vector2 newPos = oldPos;

    if (m_KeyPress & 1) {
        newPos.x -= speed * deltaTime;
        obj->SetFlip(SDL_FLIP_NONE);
    }
    if (m_KeyPress & (1 << 2)) {
        newPos.x += speed * deltaTime;
        obj->SetFlip(SDL_FLIP_HORIZONTAL);
    }

    //Check Collision
    SDL_Rect newCollider = { (int)newPos.x, (int)newPos.y, obj->GetWidth(), obj->GetHeight() };

    if (!CheckCollision(newCollider, enemyCollider)) {
        obj->Set2DPosition(newPos.x, newPos.y);
        playerCollider = newCollider;
    }

    //PlayerPhysic
    velocityY += gravityForce * deltaTime; // Trọng lực

    float posX = obj->Get2DPosition().x;
    float posY = obj->Get2DPosition().y + velocityY * deltaTime;

    playerCollider = { (int)posX, (int)posY, obj->GetWidth(), obj->GetHeight() };

    isGrounded = GroundCheck();

    if (isGrounded) {
        velocityY = 0;
        posY = groundCollider.y - obj->GetHeight() +2;
    }

    obj->Set2DPosition(posX, posY);
    playerCollider.y = (int)posY;


    for (auto& it : m_listButton) it->Update(deltaTime);
    for (auto& it : m_listAnimation) it->Update(deltaTime);

    Camera::GetInstance()->Update(deltaTime);
}


void GSPlay::Draw(SDL_Renderer* renderer)
{
    for (auto& bg : m_backgroundList) bg->Draw(renderer);

    if (m_tileMap)
    {
        Vector2 camPos = Camera::GetInstance()->GetPosition();
        m_tileMap->DrawMap(Renderer::GetInstance()->GetRenderer(), camPos.x, camPos.y, SCREEN_WIDTH, SCREEN_HEIDHT);
    }

    for (auto& it : m_listButton) it->Draw(renderer);
    for (auto& it : m_listAnimation) it->Draw(renderer);

    enemy->Draw(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect renderPlayerCollider = playerCollider;
    Vector2 camPos = Camera::GetInstance()->GetPosition();
    renderPlayerCollider.x -= camPos.x;
    renderPlayerCollider.y -= camPos.y;
    SDL_RenderDrawRect(renderer, &renderPlayerCollider);

    SDL_Rect renderEnemyCollider = enemyCollider;
    renderEnemyCollider.x -= camPos.x;
    renderEnemyCollider.y -= camPos.y;
    SDL_RenderDrawRect(renderer, &renderEnemyCollider);

    SDL_Rect renderGroundCollider = groundCollider;
    SDL_RenderDrawRect(renderer, &renderGroundCollider);
}

