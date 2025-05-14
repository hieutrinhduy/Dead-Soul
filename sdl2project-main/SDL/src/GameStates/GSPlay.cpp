/*
    m_timeScale == -2 => LOADING SCENE
    m_timeScale == -1 => CHANGE LEVEL
	m_timeScale == 0 => PAUSE GAME
	m_timeScale == 1 => RESUME GAME
	m_timeScale == 2 => LEVEL UP
*/

#include "GSPlay.h"

#include <iostream>
#include <thread>

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
#include "Boss1.h"
#include "SecretBoss.h"
#include "BossBullet.h"
#include "ExpOrb.h"


SDL_Rect enemyCollider;
std::vector<std::shared_ptr<Bullet>> g_bullets;
std::vector<std::shared_ptr<BossBullet>> g_bossBullets;
std::vector<std::shared_ptr<PlayerBullet>> g_playerBullets;

// ExpOrbs
std::vector<std::shared_ptr<ExpOrb>> g_expOrbs;

GSPlay::GSPlay()
{
    timeScale = std::make_shared<int>(-2);

    m_uiManager = std::make_shared<UIManager>(timeScale);

    m_tileMap = std::make_shared<TileMap>("Data/Textures/Map/output1.bin", "Map/map1_tileset/", 32, timeScale);

    m_playerController = std::make_shared<PlayerController>();
    m_playerController->Init();
}


GSPlay::~GSPlay() {}



void GSPlay::Init()
{

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

    Camera::GetInstance()->SetLevelDimension(
        m_tileMap->GetWidth() * m_tileMap->GetTileSize(),
        m_tileMap->GetHeight() * m_tileMap->GetTileSize()
    );

    // Enemy thường
    m_enemyController = std::make_shared<EnemyController>();
    m_enemyController->Init(0, 800, 450);

    m_enemyController2 = std::make_shared<EnemyController>();
    m_enemyController2->Init(1220, 1790, 385);

    //Boss1
    m_boss1 = std::make_shared<Boss1>();
    m_boss1->Init(600, 1900, 960);

    //Boss2
    m_boss2 = std::make_shared<ScecretBoss>();
    m_boss2->Init(600, 1900, 1030);

    // Enemy đặc biệt: EvilCreature
    m_evilCreature = std::make_shared<EvilCreature>();
    m_evilCreature->Init(1850, 3000, 325);

    // Enemy đặc biệt: GhostWarrior
    m_ghostWarrior = std::make_shared<GhostWarrior>();
    m_ghostWarrior->Init(1850, 3000, 325);

    // Enemy đặc biệt: GhostWarriorRed
    m_ghostWarriorRed = std::make_shared<GhostWarriorRed>();
    m_ghostWarriorRed->Init(3550, 5700, 300);

    //Enemy ban dan: RangedEnemy
    m_rangedEnemy = std::make_shared<RangedEnemy>();
    m_rangedEnemy->Init(3550, 5700, 300);

    // Set player collider cho tất cả
    m_enemyController->SetPlayerCollider(&m_playerController->GetCollider());
    m_enemyController2->SetPlayerCollider(&m_playerController->GetCollider());
    m_evilCreature->SetPlayerCollider(&m_playerController->GetCollider());
    m_ghostWarrior->SetPlayerCollider(&m_playerController->GetCollider());
    m_ghostWarriorRed->SetPlayerCollider(&m_playerController->GetCollider());
    m_rangedEnemy->SetPlayerCollider(&m_playerController->GetCollider());
    m_rangedEnemy->SetPlayerController(m_playerController.get());

    m_boss1->SetPlayerCollider(&m_playerController->GetCollider());
    m_boss2->SetPlayerCollider(&m_playerController->GetCollider());

    // Lấy danh sách collider của enemy
    enemyColliders.push_back(m_enemyController->GetEnemyCollider());
    enemyColliders.push_back(m_enemyController2->GetEnemyCollider());
    enemyColliders.push_back(m_evilCreature->GetCollider());
    enemyColliders.push_back(m_ghostWarrior->GetCollider());
    enemyColliders.push_back(m_ghostWarriorRed->GetCollider());
    enemyColliders.push_back(m_rangedEnemy->GetCollider());
    enemyColliders.push_back(m_boss1->GetEnemyCollider());
    enemyColliders.push_back(m_boss2->GetEnemyCollider());
    m_playerController->SetEnemyColliders(enemyColliders);

    // Callback khi enemy đánh player
    m_enemyController->SetOnHitPlayerCallback([this](int damage) {
        int dir = (m_enemyController->GetEnemyCollider()->x < m_playerController->GetCollider().x) ? 1 : -1;
        m_playerController->TakeDamage(damage, dir);
        });
    m_enemyController2->SetOnHitPlayerCallback([this](int damage) {
        int dir = (m_enemyController2->GetEnemyCollider()->x < m_playerController->GetCollider().x) ? 1 : -1;
        m_playerController->TakeDamage(damage, dir);
        });
    m_evilCreature->SetOnHitPlayerCallback([this](int damage) {
        int dir = (m_evilCreature->GetCollider()->x < m_playerController->GetCollider().x) ? 1 : -1;
        m_playerController->TakeDamage(damage, dir);
        });
    m_ghostWarrior->SetOnHitPlayerCallback([this](int damage) {
        int dir = (m_ghostWarrior->GetCollider()->x < m_playerController->GetCollider().x) ? 1 : -1;
        m_playerController->TakeDamage(damage, dir);
        });
    m_ghostWarriorRed->SetOnHitPlayerCallback([this](int damage) {
        int dir = (m_ghostWarriorRed->GetCollider()->x < m_playerController->GetCollider().x) ? 1 : -1;
        m_playerController->TakeDamage(damage, dir);
        });
    m_rangedEnemy->SetOnHitPlayerCallback([this](int damage) {
        int dir = (m_rangedEnemy->GetCollider()->x < m_playerController->GetCollider().x) ? 1 : -1;
        m_playerController->TakeDamage(damage, dir);
        });
    m_boss1->SetOnHitPlayerCallback([this](int damage) {
        int dir = (m_boss1->GetEnemyCollider()->x < m_playerController->GetCollider().x) ? 1 : -1;
        m_playerController->TakeDamage(damage, dir);
        });
    m_boss2->SetOnHitPlayerCallback([this](int damage) {
        int dir = (m_boss2->GetEnemyCollider()->x < m_playerController->GetCollider().x) ? 1 : -1;
        m_playerController->TakeDamage(damage, dir);
        });

    // Callback khi player đánh trúng enemy
    m_hitEnemyCallbacks.push_back([this](int damage) {
        int dir = (m_playerController->GetCollider().x > m_enemyController->GetEnemyCollider()->x) ? -1 : 1;
        m_enemyController->TakeDamage(damage, dir);
        });

    m_hitEnemyCallbacks.push_back([this](int damage) {
        int dir = (m_playerController->GetCollider().x > m_enemyController2->GetEnemyCollider()->x) ? -1 : 1;
        m_enemyController2->TakeDamage(damage, dir);
        });
    m_hitEnemyCallbacks.push_back([this](int damage) {
        int dir = (m_playerController->GetCollider().x > m_evilCreature->GetCollider()->x) ? -1 : 1;
        m_evilCreature->TakeDamage(damage, dir);
        });
    m_hitEnemyCallbacks.push_back([this](int damage) {
        int dir = (m_playerController->GetCollider().x > m_ghostWarrior->GetCollider()->x) ? -1 : 1;
        m_ghostWarrior->TakeDamage(damage, dir);
        });
    m_hitEnemyCallbacks.push_back([this](int damage) {
        int dir = (m_playerController->GetCollider().x > m_ghostWarriorRed->GetCollider()->x) ? -1 : 1;
        m_ghostWarriorRed->TakeDamage(damage, dir);
        });
    m_hitEnemyCallbacks.push_back([this](int damage) {
        int dir = (m_playerController->GetCollider().x > m_rangedEnemy->GetCollider()->x) ? -1 : 1;
        m_rangedEnemy->TakeDamage(damage, dir);
        });
    m_hitEnemyCallbacks.push_back([this](int damage) {
        m_boss1->TakeDamage(damage);
        });
    m_hitEnemyCallbacks.push_back([this](int damage) {
        m_boss2->TakeDamage(damage);
        });
    m_playerController->SetOnHitEnemyCallbacks(m_hitEnemyCallbacks);

    // Rung màn hình khi đánh trúng enemy
    m_playerController->SetOnScreenShakeCallback([]() {
        int randomMagnitude = 5 + rand() % 2;
        Camera::GetInstance()->Shake(0.1f, randomMagnitude);
    });

    //Spawn Exp khi enemy Chet
    m_enemyController->SetSpawnExpCallback([this](Vector2 pos) {
        auto orb = std::make_shared<ExpOrb>(pos);
        orb->SetOnCollectedCallback([this]() {
            m_playerController->SetExpGained(10);
            m_playerController->SetGoldGained(10);
            });
        g_expOrbs.push_back(orb);
        });
    m_enemyController2->SetSpawnExpCallback([this](Vector2 pos) {
        auto orb = std::make_shared<ExpOrb>(pos);
        orb->SetOnCollectedCallback([this]() {
            m_playerController->SetExpGained(10);
            m_playerController->SetGoldGained(10);
            });
        g_expOrbs.push_back(orb);
        });
    m_evilCreature->SetSpawnExpCallback([this](Vector2 pos) {
        auto orb = std::make_shared<ExpOrb>(pos);
        orb->SetOnCollectedCallback([this]() {
            m_playerController->SetExpGained(10);
            m_playerController->SetGoldGained(10);
            });
        g_expOrbs.push_back(orb);
        });
    m_ghostWarrior->SetSpawnExpCallback([this](Vector2 pos) {
        auto orb = std::make_shared<ExpOrb>(pos);
        orb->SetOnCollectedCallback([this]() {
            m_playerController->SetExpGained(10);
            m_playerController->SetGoldGained(10);
            });
        g_expOrbs.push_back(orb);
        });
    m_ghostWarriorRed->SetSpawnExpCallback([this](Vector2 pos) {
        auto orb = std::make_shared<ExpOrb>(pos);
        orb->SetOnCollectedCallback([this]() {
            m_playerController->SetExpGained(10);
            m_playerController->SetGoldGained(10);
            });
        g_expOrbs.push_back(orb);
        });
    m_rangedEnemy->SetSpawnExpCallback([this](Vector2 pos) {
        auto orb = std::make_shared<ExpOrb>(pos);
        orb->SetOnCollectedCallback([this]() {
            m_playerController->SetExpGained(10);
            m_playerController->SetGoldGained(10);
            });
        g_expOrbs.push_back(orb);
        });
    m_boss1->SetSpawnExpCallback([this](Vector2 pos) {
        g_expOrbs.push_back(std::make_shared<ExpOrb>(pos));
        });
    m_boss2->SetSpawnExpCallback([this](Vector2 pos) {
        auto orb = std::make_shared<ExpOrb>(pos);
        orb->SetOnCollectedCallback([this]() {
            m_playerController->SetExpGained(10);
            m_playerController->SetGoldGained(10);
            });
        g_expOrbs.push_back(orb);
        });
    DisplayLoadingView();
}

void GSPlay::Exit() {
    // stop thread at TileMap:
    m_tileMap->ClearThread();
    while(!m_tileMap->m_finishThread) {}

    // stop thread at UIManager:
    m_uiManager->ClearThread();
    while(!m_uiManager->m_finishThread) {}
}


void GSPlay::Pause() {}


void GSPlay::Resume() {}




void GSPlay::HandleEvents() {}



void GSPlay::HandleKeyEvents(SDL_Event& e)
{
    if (*timeScale == 1) {
        m_playerController->HandleKeyEvents(e);
        m_playerController->HandleMouseEvents(e);
    }
}



void GSPlay::HandleTouchEvents(SDL_Event& e)
{
    m_uiManager->HandleTouchEvents(e);
    m_uiManager->HandleMouseEvents(e);
    for (auto& button : m_listButton)
        if (button->HandleTouchEvent(&e)) break;
}



void GSPlay::HandleMouseMoveEvents(SDL_Event& e) {
}




void GSPlay::Update(float deltaTime)
{
    m_uiManager->Update(deltaTime);

    // if the game is not running or hasn't resumed, not update:
    if (*timeScale != 1) return;

    for (auto& it : m_listButton) it->Update(deltaTime);

    m_playerController->Update(deltaTime);
    m_enemyController->Update(deltaTime);
    m_enemyController2->Update(deltaTime);
    m_evilCreature->Update(deltaTime);
    m_ghostWarrior->Update(deltaTime);
    m_ghostWarriorRed->Update(deltaTime);
    m_rangedEnemy->Update(deltaTime);
    m_boss1->Update(deltaTime);
    m_boss2->Update(deltaTime);

    Vector2 playerPos;
    playerPos.x = m_playerController->GetCollider().x; 
    playerPos.y = m_playerController->GetCollider().y + 50;
    for (auto& orb : g_expOrbs)
        orb->Update(deltaTime, playerPos);

    g_expOrbs.erase(
        std::remove_if(g_expOrbs.begin(), g_expOrbs.end(),
            [](const std::shared_ptr<ExpOrb>& orb) {
                return orb->IsCollected();
            }),
        g_expOrbs.end());


    for (auto it = g_bullets.begin(); it != g_bullets.end(); )
    {
        (*it)->Update(deltaTime);
        if ((*it)->HasHitPlayer(&m_playerController->GetCollider()) || (*it)->IsOutOfScreen())
            it = g_bullets.erase(it);
        else
            ++it;
    }

    for (auto it = g_bossBullets.begin(); it != g_bossBullets.end(); )
    {
        (*it)->Update(deltaTime);
        if ((*it)->HasHitPlayer(&m_playerController->GetCollider()))
            it = g_bossBullets.erase(it);
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

    m_playerController->Draw(renderer);

    m_enemyController->Draw(renderer);
    m_enemyController2->Draw(renderer);
    m_evilCreature->Draw(renderer);
    m_ghostWarrior->Draw(renderer);
    m_ghostWarriorRed->Draw(renderer);
    m_rangedEnemy->Draw(renderer);
    m_boss1->Draw(renderer);
    m_boss2->Draw(renderer);

    for (auto& orb : g_expOrbs) orb->Draw(renderer);

    for (auto& bullet : g_bullets)
    {
        bullet->Draw(Renderer::GetInstance()->GetRenderer());
    }

    for (auto& bullet : g_bossBullets)
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

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // draw UI:
    m_uiManager->Draw(renderer);
}



void GSPlay::DisplayLoadingView() {
    std::thread t([this]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        // SOUNDS:
        Sound::GetInstance()->LoadSound("gameplay_music.mp3");
        Sound::GetInstance()->PlaySound("gameplay_music.mp3");

        std::this_thread::sleep_for(std::chrono::milliseconds(1600));

        // tilemap's SUB-THREAD:
        m_tileMap->SetPlayer(m_playerController);
        m_tileMap->StartCheckingPlayerMovementAbility();
        // uiManager's SUB-THREAD:
        m_uiManager->SetReferenceToPlayerController(m_playerController);
        m_uiManager->SetReferenceToTileMap(m_tileMap);
        m_uiManager->UpdateStats();

        *timeScale = 1;
        m_uiManager->ResetTimer();
    });

    t.detach();
}