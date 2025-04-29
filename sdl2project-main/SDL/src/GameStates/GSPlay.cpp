#include "GSPlay.h"
#include "ResourceManagers.h"
#include "GameObject/MouseButton.h"
#include "GameObject/Sprite2D.h"
#include "GameObject/SpriteAnimation.h"
#include "GameObject/Camera.h"
#include "KeyState.h"
#include "Sound.h"
#include "TileMap.h"
#include "PlayerController.h" // Thêm dòng này
#include <iostream>
#include "EnemyController.h"

SDL_Rect groundCollider;
SDL_Rect enemyCollider;

GSPlay::GSPlay()
{
    m_tileMap = std::make_shared<TileMap>();
    m_tileMapLoaded = false;
    m_playerController = std::make_shared<PlayerController>(); // Khởi tạo player controller
}

GSPlay::~GSPlay()
{
}

void GSPlay::Init()
{
    groundCollider = { 0, 1440, 10000, 200 };
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

    m_tileMap->LoadMap("Data/Map/map6.txt");
    if (!m_tileMapLoaded) {
        m_tileMap->LoadTiles(Renderer::GetInstance()->GetRenderer(), "Data/Map/tiles", 32);
        m_tileMapLoaded = true;
    }

    Camera::GetInstance()->SetLevelDimension(
        m_tileMap->GetWidth() * m_tileMap->GetTileSize(),
        m_tileMap->GetHeight() * m_tileMap->GetTileSize()
    );

    // Khởi tạo PlayerController
    m_playerController->Init();

    m_enemyController = std::make_shared<EnemyController>();
    m_enemyController->Init(200, 800); // Enemy patrol từ X=200 tới X=800

    m_playerController->SetGroundCollider(&groundCollider);
    m_enemyController->SetGroundCollider(&groundCollider);
    m_enemyController->SetPlayerCollider(&m_playerController->GetCollider());

    m_playerController->SetEnemyCollider(m_enemyController-> GetEnemyCollider());

    m_enemyController->SetOnHitPlayerCallback([this](int damage) {
        m_playerController->TakeDamage(damage);
    });

    m_playerController->SetOnHitEnemyCallback([&](int damage) {
        m_enemyController->TakeDamage(damage);
        });

    m_playerController->SetOnScreenShakeCallback([]() {
        int randomMagnitude = 5 + rand() % 2;
        Camera::GetInstance()->Shake(0.1f, randomMagnitude);
        });
} 

void GSPlay::Exit()
{
}

void GSPlay::Pause()
{
}

void GSPlay::Resume()
{
}

void GSPlay::HandleEvents()
{
}

void GSPlay::HandleKeyEvents(SDL_Event& e)
{
    m_playerController->HandleKeyEvents(e);
    m_playerController->HandleMouseEvents(e);
}

void GSPlay::HandleTouchEvents(SDL_Event& e)
{
    for (auto& button : m_listButton)
        if (button->HandleTouchEvent(&e)) break;
}

void GSPlay::HandleMouseMoveEvents(SDL_Event& e)
{
}

void GSPlay::Update(float deltaTime)
{
    for (auto& it : m_listButton) it->Update(deltaTime);

    m_playerController->Update(deltaTime);
    m_enemyController->Update(deltaTime);

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

    // Vẽ player
    m_playerController->Draw(renderer);

    m_enemyController->Draw(renderer);

    // Vẽ enemy
    if (enemy)
        enemy->Draw(renderer);

    // Debug collider
    auto ConvertToCameraView = [](SDL_Rect objCollider, Vector2 camPos) -> SDL_Rect {
        SDL_Rect out = objCollider;
        out.x -= camPos.x;
        out.y -= camPos.y;
        return out;
        };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &ConvertToCameraView(enemyCollider, camPos));

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderDrawRect(renderer, &ConvertToCameraView(groundCollider, camPos));

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}
