#include "Level1.h"
#include "ExpOrb.h"
#include "Camera.h"
#include "ResourceManagers.h"
#include "Sprite2D.h"

extern std::vector<std::shared_ptr<ExpOrb>> g_expOrbs;

Level1::Level1(std::shared_ptr<PlayerController> player)
{
    m_backgroundList.clear();
    auto tex1 = ResourceManagers::GetInstance()->GetTexture("background_layer_1(Map1).png");
    auto tex2 = ResourceManagers::GetInstance()->GetTexture("background_layer_2(Map1).png");
    auto tex3 = ResourceManagers::GetInstance()->GetTexture("background_layer_3(Map1).png");

    m_backgroundList.push_back(std::make_shared<Sprite2D>(tex1, SDL_FLIP_NONE));
    m_backgroundList.push_back(std::make_shared<Sprite2D>(tex2, SDL_FLIP_NONE));
    m_backgroundList.push_back(std::make_shared<Sprite2D>(tex3, SDL_FLIP_NONE));

    for (auto& bg : m_backgroundList) {
        bg->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
        bg->Set2DPosition(0, 0);
    }

    // Khởi tạo enemy
    m_enemyController = std::make_shared<EnemyController>();
    m_enemyController->Init(0, 800, 450);

    m_enemyController2 = std::make_shared<EnemyController>();
    m_enemyController2->Init(1220, 1790, 385);

    m_evilCreature = std::make_shared<EvilCreature>();
    m_evilCreature->Init(1850, 3000, 325);

    m_ghostWarrior = std::make_shared<GhostWarrior>();
    m_ghostWarrior->Init(1850, 3000, 325);

    m_ghostWarriorRed = std::make_shared<GhostWarriorRed>();
    m_ghostWarriorRed->Init(3550, 5700, 300);

    m_rangedEnemy = std::make_shared<RangedEnemy>();
    m_rangedEnemy->Init(3550, 5700, 300);
    m_rangedEnemy->SetPlayerController(player.get());

    //m_boss1 = std::make_shared<Boss1>();
    //m_boss1->Init(600, 1900, 960);

    //m_boss2 = std::make_shared<ScecretBoss>();
    //m_boss2->Init(600, 1900, 1030);

    // Cài đặt player collider
    m_enemyController->SetPlayerCollider(&player->GetCollider());
    m_enemyController2->SetPlayerCollider(&player->GetCollider());
    m_evilCreature->SetPlayerCollider(&player->GetCollider());
    m_ghostWarrior->SetPlayerCollider(&player->GetCollider());
    m_ghostWarriorRed->SetPlayerCollider(&player->GetCollider());
    m_rangedEnemy->SetPlayerCollider(&player->GetCollider());
    //m_boss1->SetPlayerCollider(&player->GetCollider());
    //m_boss2->SetPlayerCollider(&player->GetCollider());

    // Thiết lập các callback
    SetupCallbacks(player);
    SetupSpawnExp(player);

    // Lưu collider
    m_enemyColliders = {
        m_enemyController->GetEnemyCollider(),
        m_enemyController2->GetEnemyCollider(),
        m_evilCreature->GetCollider(),
        m_ghostWarrior->GetCollider(),
        m_ghostWarriorRed->GetCollider(),
        m_rangedEnemy->GetCollider()
        //m_boss1->GetEnemyCollider(),
        //m_boss2->GetEnemyCollider()
    };
}

Level1::~Level1() {}

void Level1::Update(float dt)
{
    m_enemyController->Update(dt);
    m_enemyController2->Update(dt);
    m_evilCreature->Update(dt);
    m_ghostWarrior->Update(dt);
    m_ghostWarriorRed->Update(dt);
    m_rangedEnemy->Update(dt);
    //m_boss1->Update(dt);
    //m_boss2->Update(dt);
}

void Level1::Draw(SDL_Renderer* renderer)
{
    m_enemyController->Draw(renderer);
    m_enemyController2->Draw(renderer);
    m_evilCreature->Draw(renderer);
    m_ghostWarrior->Draw(renderer);
    m_ghostWarriorRed->Draw(renderer);
    m_rangedEnemy->Draw(renderer);
    //m_boss1->Draw(renderer);
    //m_boss2->Draw(renderer);
}

std::vector<SDL_Rect*> Level1::GetEnemyColliders()
{
    return m_enemyColliders;
}

std::vector<std::function<void(int)>> Level1::GetHitEnemyCallbacks()
{
    return m_hitCallbacks;
}

void Level1::SetupCallbacks(std::shared_ptr<PlayerController> player)
{
    // Callback khi enemy đánh player
    auto createHitPlayerCallback = [player](std::function<SDL_Rect* ()> getCollider) {
        return [player, getCollider](int damage) {
            int dir = (getCollider()->x < player->GetCollider().x) ? 1 : -1;
            player->TakeDamage(damage, dir);
            };
        };

    m_enemyController->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_enemyController->GetEnemyCollider(); }));
    m_enemyController2->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_enemyController2->GetEnemyCollider(); }));
    m_evilCreature->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_evilCreature->GetCollider(); }));
    m_ghostWarrior->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_ghostWarrior->GetCollider(); }));
    m_ghostWarriorRed->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_ghostWarriorRed->GetCollider(); }));
    m_rangedEnemy->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_rangedEnemy->GetCollider(); }));
    //m_boss1->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_boss1->GetEnemyCollider(); }));
    //m_boss2->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_boss2->GetEnemyCollider(); }));

    // Callback khi player đánh enemy
    auto createHitEnemyCallback = [player](std::function<SDL_Rect* ()> getCollider, std::function<void(int, int)> takeDamageFunc) {
        return [player, getCollider, takeDamageFunc](int damage) {
            int dir = (player->GetCollider().x > getCollider()->x) ? -1 : 1;
            takeDamageFunc(damage, dir);
            };
        };

    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_enemyController->GetEnemyCollider(); }, [this](int d, int dir) { m_enemyController->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_enemyController2->GetEnemyCollider(); }, [this](int d, int dir) { m_enemyController2->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_evilCreature->GetCollider(); }, [this](int d, int dir) { m_evilCreature->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_ghostWarrior->GetCollider(); }, [this](int d, int dir) { m_ghostWarrior->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_ghostWarriorRed->GetCollider(); }, [this](int d, int dir) { m_ghostWarriorRed->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_rangedEnemy->GetCollider(); }, [this](int d, int dir) { m_rangedEnemy->TakeDamage(d, dir); }));

    // Boss không cần hướng
    //m_hitCallbacks.push_back([this](int d) { m_boss1->TakeDamage(d); });
    //m_hitCallbacks.push_back([this](int d) { m_boss2->TakeDamage(d); });
}


void Level1::SetupSpawnExp(std::shared_ptr<PlayerController> player)
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

    setupSpawn(m_enemyController);
    setupSpawn(m_enemyController2);
    setupSpawn(m_evilCreature);
    setupSpawn(m_ghostWarrior);
    setupSpawn(m_ghostWarriorRed);
    setupSpawn(m_rangedEnemy);
    //setupSpawn(m_boss1);
    //setupSpawn(m_boss2);
}

std::vector<std::shared_ptr<Sprite2D>> Level1::GetBackgrounds()
{
    return m_backgroundList;
}

