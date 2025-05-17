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
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "SecretLevel.h"


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
    m_tileMap->SetReferenceToGSPlay(this);
    Camera::GetInstance()->SetLevelDimension(
        m_tileMap->GetWidth() * m_tileMap->GetTileSize(),
        m_tileMap->GetHeight() * m_tileMap->GetTileSize()
    );
    m_playerController = std::make_shared<PlayerController>();
    m_playerController->Init();

    m_currentLevel = LevelID::Level1;
    m_level1 = std::make_shared<Level1>(m_playerController);
    m_playerController->SetEnemyColliders(m_level1->GetEnemyColliders());
    m_playerController->SetOnHitEnemyCallbacks(m_level1->GetHitEnemyCallbacks());
    enemyColliders = m_level1->GetEnemyColliders();
    m_hitEnemyCallbacks = m_level1->GetHitEnemyCallbacks();
    m_ChangeLevelGate = m_level1->GetChangeLevelGate();
    m_shop = m_level1->GetShop();

    // Rung màn hình khi đánh trúng enemy
    m_playerController->SetOnScreenShakeCallback([]() {
        int randomMagnitude = 5 + rand() % 2;
        Camera::GetInstance()->Shake(0.1f, randomMagnitude);
        });

    DisplayLoadingView();

    m_tileMap->ExportFullMapImage(Renderer::GetInstance()->GetRenderer());
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

        if (e.type == SDL_KEYDOWN)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_n:
                ChangeLevel(LevelID::Level2);
                break;
            case SDLK_m:
                ChangeLevel(LevelID::Level1);
                break;
            default:
                break;
            }
        }
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

    // if the game is not running or hasn't resumed, not update:
    if (*timeScale != 1) {
        m_uiManager->Update(deltaTime);
        return;
    }

    for (auto& it : m_listButton) it->Update(deltaTime);

    m_playerController->Update(deltaTime);
    //m_enemyController->Update(deltaTime);
    //m_enemyController2->Update(deltaTime);
    //m_evilCreature->Update(deltaTime);
    //m_ghostWarrior->Update(deltaTime);
    //m_ghostWarriorRed->Update(deltaTime);
    //m_rangedEnemy->Update(deltaTime);
    //m_boss1->Update(deltaTime);
    //m_boss2->Update(deltaTime);
    if (m_currentLevel == LevelID::Level1 && m_level1) m_level1->Update(deltaTime);
    if (m_currentLevel == LevelID::Level2 && m_level2) m_level2->Update(deltaTime);
    if (m_currentLevel == LevelID::Level3 && m_level3) m_level3->Update(deltaTime);
    if (m_currentLevel == LevelID::SecretLevel && m_secretLevel) m_secretLevel->Update(deltaTime);

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
        std::cout << enemyColliders.size() << std::endl;
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

    m_uiManager->Update(deltaTime);
}




void GSPlay::Draw(SDL_Renderer* renderer)
{
    Vector2 camPos = Camera::GetInstance()->GetPosition();
    std::vector<float> parallaxFactors = { 0.2f, 0.5f, 0.8f };

    std::vector<std::shared_ptr<Sprite2D>> bgList;

    if (m_currentLevel == LevelID::Level1 && m_level1)
        bgList = m_level1->GetBackgrounds();
    else if (m_currentLevel == LevelID::Level2 && m_level2)
        bgList = m_level2->GetBackgrounds();
    else if (m_currentLevel == LevelID::Level3 && m_level3)
        bgList = m_level3->GetBackgrounds();
    else if (m_currentLevel == LevelID::SecretLevel && m_secretLevel)
        bgList = m_secretLevel->GetBackgrounds();

    for (size_t i = 0; i < bgList.size(); ++i)
    {
        auto& bg = bgList[i];
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


    //m_enemyController->Draw(renderer);
    //m_enemyController2->Draw(renderer);
    //m_evilCreature->Draw(renderer);
    //m_ghostWarrior->Draw(renderer);
    //m_ghostWarriorRed->Draw(renderer);
    //m_rangedEnemy->Draw(renderer);
    //m_boss1->Draw(renderer);
    //m_boss2->Draw(renderer);

    if (m_currentLevel == LevelID::Level1 && m_level1) m_level1->Draw(renderer);
    if (m_currentLevel == LevelID::Level2 && m_level2) m_level2->Draw(renderer);
    if (m_currentLevel == LevelID::Level3 && m_level3) m_level3->Draw(renderer);
    if (m_currentLevel == LevelID::SecretLevel && m_secretLevel) m_secretLevel->Draw(renderer);

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

    m_playerController->Draw(renderer);
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

void GSPlay::ChangeLevel(LevelID newLevel)
{
    if (newLevel == m_currentLevel) return; // Không đổi level

    // Xóa level cũ
    m_level1.reset();
    m_level2.reset();
    m_level3.reset();
    m_secretLevel.reset();

    m_hitEnemyCallbacks.clear();
    enemyColliders.clear();

    // Tạo level mới
    m_currentLevel = newLevel;

    if (newLevel == LevelID::Level1) {
        m_level1 = std::make_shared<Level1>(m_playerController);
    }
    else if (newLevel == LevelID::Level2) {
        m_level2 = std::make_shared<Level2>(m_playerController);
    }
    else if (newLevel == LevelID::Level3) {
        m_level3 = std::make_shared<Level3>(m_playerController);
    }
    else if (newLevel == LevelID::SecretLevel) {
        m_secretLevel = std::make_shared<SecretLevel>(m_playerController);
    }

    // Cập nhật lại thông tin cho player
    if (newLevel == LevelID::Level1)
    {
        m_playerController->SetEnemyColliders(m_level1->GetEnemyColliders());
        m_playerController->SetOnHitEnemyCallbacks(m_level1->GetHitEnemyCallbacks());

        enemyColliders = m_level1->GetEnemyColliders();
        m_hitEnemyCallbacks = m_level1->GetHitEnemyCallbacks();
        m_ChangeLevelGate = m_level1->GetChangeLevelGate();
        m_shop = m_level1->GetShop();
    }
    else if (newLevel == LevelID::Level2)
    {
        m_playerController->SetEnemyColliders(m_level2->GetEnemyColliders());
        m_playerController->SetOnHitEnemyCallbacks(m_level2->GetHitEnemyCallbacks());

        enemyColliders = m_level2->GetEnemyColliders();
        m_hitEnemyCallbacks = m_level2->GetHitEnemyCallbacks();
        m_ChangeLevelGate = m_level2->GetChangeLevelGate();
        m_shop = m_level2->GetShop();
    }
    else if (newLevel == LevelID::Level3)
    {
        m_playerController->SetEnemyColliders(m_level3->GetEnemyColliders());
        m_playerController->SetOnHitEnemyCallbacks(m_level3->GetHitEnemyCallbacks());

        enemyColliders = m_level3->GetEnemyColliders();
        m_hitEnemyCallbacks = m_level3->GetHitEnemyCallbacks();
        m_ChangeLevelGate = m_level3->GetChangeLevelGate();
        m_shop = m_level3->GetShop();
    }
    else if (newLevel == LevelID::SecretLevel)
    {
        m_playerController->SetEnemyColliders(m_secretLevel->GetEnemyColliders());
        m_playerController->SetOnHitEnemyCallbacks(m_secretLevel->GetHitEnemyCallbacks());

        enemyColliders = m_secretLevel->GetEnemyColliders();
        m_hitEnemyCallbacks = m_secretLevel->GetHitEnemyCallbacks();
        m_ChangeLevelGate = m_secretLevel->GetChangeLevelGate();
        m_shop = m_secretLevel->GetShop();
    }
}
