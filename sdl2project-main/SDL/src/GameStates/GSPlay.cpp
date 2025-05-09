/*
    m_timeScale == -2 => LOADING SCENE
    m_timeScale == -1 => CHANGE LEVEL
    m_timeScale == 0 => PAUSE GAME (just is pause, or level up, die, victory<complete all levels>)
    m_timeScale == 1 => RESUME GAME
*/

#include "GSPlay.h"

#include <iostream>

#include "ResourceManagers.h"
#include "GameObject/MouseButton.h"
#include "GameObject/Sprite2D.h"
#include "GameObject/SpriteAnimation.h"
#include "GameObject/Camera.h"
#include "KeyState.h"
#include "Sound.h"
#include "TileMap.h"
#include "PlayerController.h"
#include "EnemyController.h"
#include "RangedEnemy.h"
#include "Bullet.h"
#include "PlayerBullet.h"



SDL_Rect groundCollider;
SDL_Rect enemyCollider;
std::vector<std::shared_ptr<Bullet>> g_bullets;
std::vector<std::shared_ptr<PlayerBullet>> g_playerBullets;



GSPlay::GSPlay()
{
    timeScale = std::make_shared<int>(-2);
    m_uiManager = std::make_shared<UIManager>(timeScale);

    m_playerController = std::make_shared<PlayerController>();

    m_tileMapLoaded = false;
    m_tileMap = std::make_shared<TileMap>("Data/Textures/Map/output.bin", "Data/Textures/Map/tiles", 32);
}


GSPlay::~GSPlay() {}




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

    /* m_tileMap->LoadMap("Data/Map/map6.txt");
     if (!m_tileMapLoaded) {
         m_tileMap->LoadTiles(Renderer::GetInstance()->GetRenderer(), "Data/Map/tiles", 32);
         m_tileMapLoaded = true;
     }*/

    Camera::GetInstance()->SetLevelDimension(
        m_tileMap->GetWidth() * m_tileMap->GetTileSize(),
        m_tileMap->GetHeight() * m_tileMap->GetTileSize()
    );

    m_playerController->Init();
    m_tileMap->SetPlayer(m_playerController);
    m_tileMap->StartCheckingPlayerMovementAbility();

    // Enemy thường
    m_enemyController = std::make_shared<EnemyController>();
    m_enemyController->Init(200, 800, 575);

    m_enemyController2 = std::make_shared<EnemyController>();
    m_enemyController2->Init(1060, 1210, 575);

    m_enemyController3 = std::make_shared<EnemyController>();
    m_enemyController3->Init(600, 1900, 1315);

    // Enemy đặc biệt: EvilCreature
    m_evilCreature = std::make_shared<EvilCreature>();
    m_evilCreature->Init(2200, 2500);

    // Enemy đặc biệt: GhostWarrior
    m_ghostWarrior = std::make_shared<GhostWarrior>();
    m_ghostWarrior->Init(2700, 3000);

    // Enemy đặc biệt: GhostWarriorRed
    m_ghostWarriorRed = std::make_shared<GhostWarriorRed>();
    m_ghostWarriorRed->Init(3200, 3500);

    //Enemy ban dan: RangedEnemy
    m_rangedEnemy = std::make_shared<RangedEnemy>();
    m_rangedEnemy->Init(4500, 4800);

    // Set ground collider cho tất cả
    m_playerController->SetGroundCollider(&groundCollider);
    m_enemyController->SetGroundCollider(&groundCollider);
    m_enemyController2->SetGroundCollider(&groundCollider);
    m_enemyController3->SetGroundCollider(&groundCollider);
    m_evilCreature->SetGroundCollider(&groundCollider);
    m_ghostWarrior->SetGroundCollider(&groundCollider);
    m_ghostWarriorRed->SetGroundCollider(&groundCollider);
    m_rangedEnemy->SetGroundCollider(&groundCollider);
    m_rangedEnemy->SetPlayerController(m_playerController.get());

    // Set player collider cho tất cả
    m_enemyController->SetPlayerCollider(&m_playerController->GetCollider());
    m_enemyController2->SetPlayerCollider(&m_playerController->GetCollider());
    m_enemyController3->SetPlayerCollider(&m_playerController->GetCollider());
    m_evilCreature->SetPlayerCollider(&m_playerController->GetCollider());
    m_ghostWarrior->SetPlayerCollider(&m_playerController->GetCollider());
    m_ghostWarriorRed->SetPlayerCollider(&m_playerController->GetCollider());
    m_rangedEnemy->SetPlayerCollider(&m_playerController->GetCollider());

    // Lấy danh sách collider của enemy
    std::vector<SDL_Rect*> enemyColliders;
    enemyColliders.push_back(m_enemyController->GetEnemyCollider());
    enemyColliders.push_back(m_enemyController2->GetEnemyCollider());
    enemyColliders.push_back(m_enemyController3->GetEnemyCollider());
    enemyColliders.push_back(m_evilCreature->GetCollider());
    enemyColliders.push_back(m_ghostWarrior->GetCollider());
    enemyColliders.push_back(m_ghostWarriorRed->GetCollider());
    enemyColliders.push_back(m_rangedEnemy->GetCollider());
    m_playerController->SetEnemyColliders(enemyColliders);

    // Callback khi enemy đánh player
    m_enemyController->SetOnHitPlayerCallback([this](int damage) {
        m_playerController->TakeDamage(damage);
        });
    m_enemyController2->SetOnHitPlayerCallback([this](int damage) {
        m_playerController->TakeDamage(damage);
        });
    m_enemyController3->SetOnHitPlayerCallback([this](int damage) {
        m_playerController->TakeDamage(damage);
        });
    m_evilCreature->SetOnHitPlayerCallback([this](int damage) {
        m_playerController->TakeDamage(damage);
        });
    m_ghostWarrior->SetOnHitPlayerCallback([this](int damage) {
        m_playerController->TakeDamage(damage);
        });
    m_ghostWarriorRed->SetOnHitPlayerCallback([this](int damage) {
        m_playerController->TakeDamage(damage);
        });
    m_rangedEnemy->SetOnHitPlayerCallback([this](int damage) {
        m_playerController->TakeDamage(damage);
        });

    // Callback khi player đánh trúng enemy
    std::vector<std::function<void(int)>> hitEnemyCallbacks;
    hitEnemyCallbacks.push_back([this](int damage) {
        std::cout << "Enemy 0 nhận damage = " << damage << std::endl;
        m_enemyController->TakeDamage(damage);
        });
    hitEnemyCallbacks.push_back([this](int damage) {
        std::cout << "Enemy 1 nhận damage = " << damage << std::endl;
        m_enemyController2->TakeDamage(damage);
        });
    hitEnemyCallbacks.push_back([this](int damage) {
        std::cout << "Enemy 2 nhận damage = " << damage << std::endl;
        m_enemyController3->TakeDamage(damage);
        });
    hitEnemyCallbacks.push_back([this](int damage) {
        std::cout << "EvilCreature nhận damage = " << damage << std::endl;
        m_evilCreature->TakeDamage(damage);
        });
    hitEnemyCallbacks.push_back([this](int damage) {
        std::cout << "GhostWarrior nhận damage = " << damage << std::endl;
        m_ghostWarrior->TakeDamage(damage);
        });
    hitEnemyCallbacks.push_back([this](int damage) {
        std::cout << "GhostWarriorRed nhận damage = " << damage << std::endl;
        m_ghostWarriorRed->TakeDamage(damage);
        });
    hitEnemyCallbacks.push_back([this](int damage) {
        std::cout << "RangedEnemy nhận damage = " << damage << std::endl;
        m_rangedEnemy->TakeDamage(damage);
        });
    m_playerController->SetOnHitEnemyCallbacks(hitEnemyCallbacks);

    // Rung màn hình khi đánh trúng enemy
    m_playerController->SetOnScreenShakeCallback([]() {
        int randomMagnitude = 5 + rand() % 2;
        Camera::GetInstance()->Shake(0.1f, randomMagnitude);
        });


    // SOUNDS:
    Sound::GetInstance()->LoadSound("gameplay_music.mp3");
    Sound::GetInstance()->PlaySound("gameplay_music.mp3");

    // run game:
    *timeScale = -2;
}




void GSPlay::Exit() {
    // stop thread at TileMap:
    m_tileMap->ClearThread();
    while(!m_tileMap->m_finishThread) {}
}


void GSPlay::Pause() {}


void GSPlay::Resume() {}




void GSPlay::HandleEvents() {}



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



void GSPlay::HandleMouseMoveEvents(SDL_Event& e) {}




void GSPlay::Update(float deltaTime)
{
    // if the game is not running or hasn't resumed, not update:
    if (*timeScale != 1) return;

    for (auto& it : m_listButton) it->Update(deltaTime);

    m_playerController->Update(deltaTime);
    m_enemyController->Update(deltaTime);
    m_enemyController2->Update(deltaTime);
    m_enemyController3->Update(deltaTime);
    m_evilCreature->Update(deltaTime);
    m_ghostWarrior->Update(deltaTime);
    m_ghostWarriorRed->Update(deltaTime);
    m_rangedEnemy->Update(deltaTime);


    for (auto it = g_bullets.begin(); it != g_bullets.end(); )
    {
        (*it)->Update(deltaTime);
        if ((*it)->HasHitPlayer(&m_playerController->GetCollider()) || (*it)->IsOutOfScreen())
            it = g_bullets.erase(it);
        else
            ++it;
    }

    for (int i = 0; i < g_playerBullets.size(); )
    {
        auto& bullet = g_playerBullets[i];
        bullet->Update(deltaTime);

        bool hit = false;
        for (size_t j = 0; j < enemyColliders.size(); ++j)
        {
            if (bullet->HasHitEnemy(enemyColliders[j]))
            {
                std::cout << bullet->GetDamage() << std::endl;
                if (j < m_hitEnemyCallbacks.size())
                    m_hitEnemyCallbacks[j](bullet->GetDamage());
                hit = true;
                break;
            }
        }

        if (bullet->IsOutOfScreen() || hit)
        {
            g_playerBullets.erase(g_playerBullets.begin() + i);
        }
        else
        {
            ++i;
        }
    }




    Camera::GetInstance()->Update(deltaTime);
}




void GSPlay::Draw(SDL_Renderer* renderer)
{
    // if the game isn't running or hasn't resumed, stop draw:
    if (*timeScale != 1) {
        // draw UI:
        m_uiManager->Draw(renderer);

        return;
    }

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


    if (m_tileMap) m_tileMap->DrawMap(renderer);


    for (auto& it : m_listButton) it->Draw(renderer);

    m_playerController->Draw(renderer);

    m_enemyController->Draw(renderer);
    m_enemyController2->Draw(renderer);
    m_enemyController3->Draw(renderer);
    m_evilCreature->Draw(renderer);
    m_ghostWarrior->Draw(renderer);
    m_ghostWarriorRed->Draw(renderer);
    m_rangedEnemy->Draw(renderer);

    for (auto& bullet : g_bullets)
    {
        bullet->Draw(Renderer::GetInstance()->GetRenderer());
    }

    for (auto& bullet : g_playerBullets)
    {
        bullet->Draw(Renderer::GetInstance()->GetRenderer());
    }

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

    // draw UI:
    m_uiManager->Draw(renderer);
}
