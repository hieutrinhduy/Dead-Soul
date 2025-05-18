#include "SecretLevel.h"
#include "Level3.h"
#include "ExpOrb.h"
#include "Camera.h"
#include "ResourceManagers.h"
#include "Sprite2D.h"

extern std::vector<std::shared_ptr<ExpOrb>> g_expOrbs;

SecretLevel::SecretLevel(std::shared_ptr<PlayerController> player)
{
    m_playerController = player;
    m_backgroundList.clear();
    auto tex1 = ResourceManagers::GetInstance()->GetTexture("background_layer_1(Map2).png");
    auto tex2 = ResourceManagers::GetInstance()->GetTexture("/Paralax background Pack/Green mountain/Clouds.png");
    auto tex3 = ResourceManagers::GetInstance()->GetTexture("/Paralax background Pack/Green mountain/Green_mountain.png");

    m_backgroundList.push_back(std::make_shared<Sprite2D>(tex1, SDL_FLIP_NONE));
    m_backgroundList.push_back(std::make_shared<Sprite2D>(tex2, SDL_FLIP_NONE));
    m_backgroundList.push_back(std::make_shared<Sprite2D>(tex3, SDL_FLIP_NONE));

    for (auto& bg : m_backgroundList) {
        bg->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
        bg->Set2DPosition(0, 0);
    }

    auto level3Decor = ResourceManagers::GetInstance()->GetTexture("decorations/Level4Decor.png");
    if (level3Decor != nullptr)
    {
        auto decor = std::make_shared<SpriteAnimation>(level3Decor, 1, 1, 1, 10);
        int decorWidth = 6592;
        int decorHeight = 3072;

        decor->SetSize(decorWidth, decorHeight);
        decor->Set2DPosition(0, -625);
        m_decorList.push_back(decor);
    }

    m_gate = std::make_shared<ChangeLevelGate>();
    m_gate->Init(INT_MAX, INT_MAX);

    m_shop = std::make_shared<Shop>();
    m_shop->Init(INT_MAX, INT_MAX);

    m_boss1 = std::make_shared<Boss1>();
    m_boss1->Init(-300, 3700, 385);
    m_boss1->SetPlayerCollider(&player->GetCollider());

    m_boss2 = std::make_shared<ScecretBoss>();
    m_boss2->Init(-300, 3700, 455);
    m_boss2->SetPlayerCollider(&player->GetCollider());

    // Thiết lập các callback
    SetupCallbacks(player);
    SetupSpawnExp(player);

    // Lưu collider
    m_enemyColliders = {
        m_boss1->GetEnemyCollider(),
        m_boss2->GetEnemyCollider()
    };

    m_boss1->SetOnDeathCallback([this](Vector2 deathPos) {
        m_boss1_isDead = true;
        });
    m_boss2->SetOnDeathCallback([this]{
        m_boss2_isDead = true;
        });
}

SecretLevel::~SecretLevel() {}

void SecretLevel::Update(float dt)
{
    m_gate->Update(dt);
    m_shop->Update(dt);
    m_boss1->Update(dt);
    m_boss2->Update(dt);

    // === 1. Khi cả hai boss đã chết lần đầu ===
    if (m_boss1_isDead && m_boss2_isDead && !m_finishGame)
    {
        m_finishGame = true;

        float gateX = 3000, gateY = 400;
        m_gate->SetGatePos({ 3000, 400 });

        Camera::GetInstance()->SetTarget(nullptr);
        Camera::GetInstance()->LerpToPosition(gateX - SCREEN_WIDTH / 2, gateY - SCREEN_HEIDHT / 2, 1.0f);

        m_isCameraMovingToGate = true;
        m_isWaitingAtGate = false;
        m_isReturningToPlayer = false;
        m_cameraWaitTimer = 0.0f;
    }

    // === 2. Khi camera đã đến gate, bắt đầu đếm thời gian chờ 2s ===
    if (m_isCameraMovingToGate && !Camera::GetInstance()->IsLerping() && !m_isReturningToPlayer && !m_isWaitingAtGate)
    {
        m_isWaitingAtGate = true;
        m_cameraWaitTimer = 0.0f;
    }

    // === 3. Đang chờ tại gate trong 2s ===
    if (m_isWaitingAtGate)
    {
        m_cameraWaitTimer += dt;

        if (m_cameraWaitTimer >= m_waitDurationAtGate)
        {
            float px = m_playerController->GetPlayerPosition().x;
            float py = m_playerController->GetPlayerPosition().y;

            Camera::GetInstance()->LerpToPosition(px - SCREEN_WIDTH / 2, py - SCREEN_HEIDHT / 2, 1.0f);

            m_isWaitingAtGate = false;
            m_isReturningToPlayer = true;
        }
    }

    // === 4. Sau khi Lerp xong về player thì SetTarget lại ===
    if (m_isReturningToPlayer && !Camera::GetInstance()->IsLerping())
    {
        Camera::GetInstance()->SetTarget(m_playerController->GetPlayer());

        m_isCameraMovingToGate = false;
        m_isReturningToPlayer = false;
        m_isWaitingAtGate = false;
        Camera::GetInstance()->SetLerpingRoutineState(false);
    }
}


void SecretLevel::Draw(SDL_Renderer* renderer)
{
    m_gate->Draw(renderer);
    m_shop->Draw(renderer);
    for (auto& decor : m_decorList)
    {
        decor->Draw(renderer);
    }
    m_boss1->Draw(renderer);
    m_boss2->Draw(renderer);
}

std::vector<SDL_Rect*> SecretLevel::GetEnemyColliders()
{
    return m_enemyColliders;
}

std::vector<std::function<void(int)>> SecretLevel::GetHitEnemyCallbacks()
{
    return m_hitCallbacks;
}

void SecretLevel::SetupCallbacks(std::shared_ptr<PlayerController> player)
{
    // Callback khi enemy đánh player
    auto createHitPlayerCallback = [player](std::function<SDL_Rect* ()> getCollider) {
        return [player, getCollider](int damage) {
            int dir = (getCollider()->x < player->GetCollider().x) ? 1 : -1;
            player->TakeDamage(damage, dir);
            };
        };
    m_boss1->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_boss1->GetEnemyCollider(); }));
    m_boss2->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_boss2->GetEnemyCollider(); }));

    // Callback khi player đánh enemy
    auto createHitEnemyCallback = [player](std::function<SDL_Rect* ()> getCollider, std::function<void(int, int)> takeDamageFunc) {
        return [player, getCollider, takeDamageFunc](int damage) {
            int dir = (player->GetCollider().x > getCollider()->x) ? -1 : 1;
            takeDamageFunc(damage, dir);
            };
        };

    m_hitCallbacks.push_back([this](int d) { m_boss1->TakeDamage(d); });
    m_hitCallbacks.push_back([this](int d) { m_boss2->TakeDamage(d); });
}


void SecretLevel::SetupSpawnExp(std::shared_ptr<PlayerController> player)
{
    auto spawnExp = [player](Vector2 pos) {
        auto orb = std::make_shared<ExpOrb>(pos);
        orb->SetOnCollectedCallback([player]() {
            player->SetExpGained(10);
            player->SetGoldGained(10);
            });
        g_expOrbs.push_back(orb);
        };

    auto setupSpawn = [spawnExp](auto enemy) {
        enemy->SetSpawnExpCallback([spawnExp](Vector2 pos) {
            spawnExp(pos);
            });
        };

    setupSpawn(m_boss1);
    setupSpawn(m_boss2);
}

std::vector<std::shared_ptr<Sprite2D>> SecretLevel::GetBackgrounds()
{
    return m_backgroundList;
}

