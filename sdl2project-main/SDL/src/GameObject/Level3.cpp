#include "Level3.h"
#include "ExpOrb.h"
#include "Camera.h"
#include "ResourceManagers.h"
#include "Sprite2D.h"

extern std::vector<std::shared_ptr<ExpOrb>> g_expOrbs;

Level3::Level3(std::shared_ptr<PlayerController> player)
{
    m_playerController = player;
    m_backgroundList.clear();
    auto tex1 = ResourceManagers::GetInstance()->GetTexture("/Level3BG/7.png");
    auto tex2 = ResourceManagers::GetInstance()->GetTexture("/Level3BG/6.png");
    auto tex3 = ResourceManagers::GetInstance()->GetTexture("/Level3BG/3.png");


    m_backgroundList.push_back(std::make_shared<Sprite2D>(tex1, SDL_FLIP_NONE));
    m_backgroundList.push_back(std::make_shared<Sprite2D>(tex2, SDL_FLIP_NONE));
    m_backgroundList.push_back(std::make_shared<Sprite2D>(tex3, SDL_FLIP_NONE));

    for (auto& bg : m_backgroundList) {
        bg->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
        bg->Set2DPosition(0, 0);

    }

    auto level3Decor = ResourceManagers::GetInstance()->GetTexture("decorations/Level3Decor.png");
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

    m_secretGate = std::make_shared<SecretGate>();
    m_secretGate->Init(INT_MAX, INT_MAX);

    m_shop = std::make_shared<Shop>();
    m_shop->Init(INT_MAX, INT_MAX);

    m_boss1 = std::make_shared<Boss1>();
    m_boss1->Init(-300, 3700, 385);
    m_boss1->SetPlayerCollider(&player->GetCollider());

    // Thiết lập các callback
    SetupCallbacks(player);
    SetupSpawnExp(player);

    // Lưu collider
    m_enemyColliders = {
        m_boss1->GetEnemyCollider()
    };

    m_boss1->SetOnDeathCallback([this](Vector2 deathPos) {
        if (m_gate) {
            m_gate->SetGatePos(m_gate->GetFinalGatePos());
        }
        if (m_secretGate) {
            m_secretGate->SetGatePos(m_secretGate->GetFinalGatePos());
        }
        Camera::GetInstance()->SetTarget(nullptr);
        Camera::GetInstance()->LerpToPosition(m_gate->GetFinalGatePos().x - SCREEN_WIDTH / 2, m_gate->GetFinalGatePos().y - SCREEN_HEIDHT / 2, 1.0f);

        m_isCameraMovingToGate = true;
        m_isWaitingAtGate = false;
        m_isReturningToPlayer = false;
        m_cameraWaitTimer = 0.0f;
        });
}

Level3::~Level3() {}

void Level3::Update(float dt)
{
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
    m_gate->Update(dt);
    m_secretGate->Update(dt);
    m_shop->Update(dt);
    m_boss1->Update(dt);
}

void Level3::Draw(SDL_Renderer* renderer)
{
    m_gate->Draw(renderer);
    m_secretGate->Draw(renderer);
    m_shop->Draw(renderer);
    for (auto& decor : m_decorList)
    {
        decor->Draw(renderer);
    }
    m_boss1->Draw(renderer);
}

std::vector<SDL_Rect*> Level3::GetEnemyColliders()
{
    return m_enemyColliders;
}

std::vector<std::function<void(int)>> Level3::GetHitEnemyCallbacks()
{
    return m_hitCallbacks;
}

void Level3::SetupCallbacks(std::shared_ptr<PlayerController> player)
{
    // Callback khi enemy đánh player
    auto createHitPlayerCallback = [player](std::function<SDL_Rect* ()> getCollider) {
        return [player, getCollider](int damage) {
            int dir = (getCollider()->x < player->GetCollider().x) ? 1 : -1;
            player->TakeDamage(damage, dir);
            };
        };
    m_boss1->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_boss1->GetEnemyCollider(); }));

    // Callback khi player đánh enemy
    auto createHitEnemyCallback = [player](std::function<SDL_Rect* ()> getCollider, std::function<void(int, int)> takeDamageFunc) {
        return [player, getCollider, takeDamageFunc](int damage) {
            int dir = (player->GetCollider().x > getCollider()->x) ? -1 : 1;
            takeDamageFunc(damage, dir);
            };
        };

    m_hitCallbacks.push_back([this](int d) { m_boss1->TakeDamage(d); });
}


void Level3::SetupSpawnExp(std::shared_ptr<PlayerController> player)
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
}

std::vector<std::shared_ptr<Sprite2D>> Level3::GetBackgrounds()
{
    return m_backgroundList;
}

