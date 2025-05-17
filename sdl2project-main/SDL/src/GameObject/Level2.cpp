#include "Level2.h"
#include "ExpOrb.h"
#include "Camera.h"
#include "ResourceManagers.h"
#include "Sprite2D.h"

extern std::vector<std::shared_ptr<ExpOrb>> g_expOrbs;

Level2::Level2(std::shared_ptr<PlayerController> player)
{
    m_backgroundList.clear();
    auto tex1 = ResourceManagers::GetInstance()->GetTexture("background_layer_1(Map2).png");

    m_backgroundList.push_back(std::make_shared<Sprite2D>(tex1, SDL_FLIP_NONE));

    for (auto& bg : m_backgroundList) {
        bg->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
        bg->Set2DPosition(0, 0);
    }

    auto level2Decor = ResourceManagers::GetInstance()->GetTexture("decorations/Level2Decor.png");
    if (level2Decor != nullptr)
    {
        auto decor = std::make_shared<SpriteAnimation>(level2Decor, 1, 1, 1, 10);
        int decorWidth = 6592;
        int decorHeight = 3072;

        decor->SetSize(decorWidth, decorHeight);
        decor->Set2DPosition(0, 0);
        m_decorList.push_back(decor);
    }
    else
    {
        std::cout << "Lỗi: Không tải được texture Level1Decor.png\n";
    }

    m_gate = std::make_shared<ChangeLevelGate>();
    m_gate->Init(5585, 2180);

    m_shop = std::make_shared<Shop>();
    m_shop->Init(500, 35);

    //Tang 1
    m_evilCreature = std::make_shared<EvilCreature>();
    m_evilCreature->Init(1550, 2430, 490);

    m_ghostWarrior = std::make_shared<GhostWarrior>();
    m_ghostWarrior->Init(1550, 2430, 490);

    m_evilCreature2 = std::make_shared<EvilCreature>();
    m_evilCreature2->Init(2730, 4020, 490);
    m_ghostWarrior2 = std::make_shared<GhostWarrior>();
    m_ghostWarrior2->Init(2730, 4020, 490);

    m_ghostWarrior3 = std::make_shared<GhostWarrior>();
    m_ghostWarrior3->Init(5270, 5990, 490);

    m_ghostWarriorRed = std::make_shared<GhostWarriorRed>();
    m_ghostWarriorRed->Init(4320, 4820, 490);

    m_rangedEnemy = std::make_shared<RangedEnemy>();
    m_rangedEnemy->Init(2730, 4020, 490);
    m_rangedEnemy->SetPlayerController(player.get());

    //Tang 2
    m_ghostWarriorRed2 = std::make_shared<GhostWarriorRed>();
    m_ghostWarriorRed2->Init(290, 1520, 1070);
    m_evilCreature3 = std::make_shared<EvilCreature>();
    m_evilCreature3->Init(3000, 3500, 1065);
    m_ghostWarrior4 = std::make_shared<GhostWarrior>();
    m_ghostWarrior4->Init(3600, 4820, 1070);

    //Tang 3
    m_evilCreature4 = std::make_shared<EvilCreature>();
    m_evilCreature4->Init(3550, 5420, 1675);

    m_ghostWarriorRed3 = std::make_shared<GhostWarriorRed>();
    m_ghostWarriorRed3->Init(3550, 5420, 1670);

    //Tang 4
    m_ghostWarriorRed4 = std::make_shared<GhostWarriorRed>();
    m_ghostWarriorRed4->Init(280, 1620, 2000);
    //Tang 5
    m_evilCreature5 = std::make_shared<EvilCreature>();
    m_evilCreature5->Init(350, 2500, 2445);
    m_ghostWarrior5 = std::make_shared<GhostWarrior>();
    m_ghostWarrior5->Init(350, 2500, 2450);
    m_rangedEnemy2 = std::make_shared<RangedEnemy>();
    m_rangedEnemy2->Init(350, 2500, 2450);
    m_rangedEnemy2->SetPlayerController(player.get());

    m_rangedEnemy3 = std::make_shared<RangedEnemy>();
    m_rangedEnemy3->Init(4390, 6380, 2820);
    m_rangedEnemy3->SetPlayerController(player.get());
    // Cài đặt player collider

    m_evilCreature->SetPlayerCollider(&player->GetCollider());
    m_evilCreature2->SetPlayerCollider(&player->GetCollider());
    m_evilCreature3->SetPlayerCollider(&player->GetCollider());
    m_evilCreature4->SetPlayerCollider(&player->GetCollider());
    m_evilCreature5->SetPlayerCollider(&player->GetCollider());
    m_ghostWarrior->SetPlayerCollider(&player->GetCollider());
    m_ghostWarrior2->SetPlayerCollider(&player->GetCollider());
    m_ghostWarrior3->SetPlayerCollider(&player->GetCollider());
    m_ghostWarrior4->SetPlayerCollider(&player->GetCollider());
    m_ghostWarrior5->SetPlayerCollider(&player->GetCollider());
    m_ghostWarriorRed->SetPlayerCollider(&player->GetCollider());
    m_ghostWarriorRed2->SetPlayerCollider(&player->GetCollider());
    m_ghostWarriorRed3->SetPlayerCollider(&player->GetCollider());
    m_ghostWarriorRed4->SetPlayerCollider(&player->GetCollider());
    m_rangedEnemy->SetPlayerCollider(&player->GetCollider());
    m_rangedEnemy2->SetPlayerCollider(&player->GetCollider());
    m_rangedEnemy3->SetPlayerCollider(&player->GetCollider());

    // Thiết lập các callback
    SetupCallbacks(player);
    SetupSpawnExp(player);

    // Lưu collider
    m_enemyColliders = {
        m_evilCreature->GetCollider(),
        m_evilCreature2->GetCollider(),
        m_evilCreature3->GetCollider(),
        m_evilCreature4->GetCollider(),
        m_evilCreature5->GetCollider(),
        m_ghostWarrior->GetCollider(),
        m_ghostWarrior2->GetCollider(),
        m_ghostWarrior3->GetCollider(),
        m_ghostWarrior4->GetCollider(),
        m_ghostWarrior5->GetCollider(),
        m_ghostWarriorRed->GetCollider(),
        m_ghostWarriorRed2->GetCollider(),
        m_ghostWarriorRed3->GetCollider(),
        m_ghostWarriorRed4->GetCollider(),
        m_rangedEnemy->GetCollider(),
        m_rangedEnemy2->GetCollider(),
        m_rangedEnemy3->GetCollider()
    };
}

Level2::~Level2() {}

void Level2::Update(float dt)
{
    m_gate->Update(dt);
    m_shop->Update(dt);
    m_evilCreature->Update(dt);
    m_evilCreature2->Update(dt);
    m_evilCreature3->Update(dt);
    m_evilCreature4->Update(dt);
    m_evilCreature5->Update(dt);
    m_ghostWarrior->Update(dt);
    m_ghostWarrior2->Update(dt);
    m_ghostWarrior3->Update(dt);
    m_ghostWarrior4->Update(dt);
    m_ghostWarrior5->Update(dt);
    m_ghostWarriorRed->Update(dt);
    m_ghostWarriorRed2->Update(dt);
    m_ghostWarriorRed3->Update(dt);
    m_ghostWarriorRed4->Update(dt);
    m_rangedEnemy->Update(dt);
    m_rangedEnemy2->Update(dt);
    m_rangedEnemy3->Update(dt);
}

void Level2::Draw(SDL_Renderer* renderer)
{
    m_gate->Draw(renderer);
    m_shop->Draw(renderer);
    for (auto& decor : m_decorList)
    {
        decor->Draw(renderer);
    }
    m_evilCreature->Draw(renderer);
    m_evilCreature2->Draw(renderer);
    m_evilCreature3->Draw(renderer);
    m_evilCreature4->Draw(renderer);
    m_evilCreature5->Draw(renderer);
    m_ghostWarrior->Draw(renderer);
    m_ghostWarrior2->Draw(renderer);
    m_ghostWarrior3->Draw(renderer);
    m_ghostWarrior4->Draw(renderer);
    m_ghostWarrior5->Draw(renderer);
    m_ghostWarriorRed->Draw(renderer);
    m_ghostWarriorRed2->Draw(renderer);
    m_ghostWarriorRed3->Draw(renderer);
    m_ghostWarriorRed4->Draw(renderer);
    m_rangedEnemy->Draw(renderer);
    m_rangedEnemy2->Draw(renderer);
    m_rangedEnemy3->Draw(renderer);
} 

std::vector<SDL_Rect*> Level2::GetEnemyColliders()
{
    return m_enemyColliders;
}

std::vector<std::function<void(int)>> Level2::GetHitEnemyCallbacks()
{
    return m_hitCallbacks;
}

void Level2::SetupCallbacks(std::shared_ptr<PlayerController> player)
{
    // Callback khi enemy đánh player
    auto createHitPlayerCallback = [player](std::function<SDL_Rect* ()> getCollider) {
        return [player, getCollider](int damage) {
            int dir = (getCollider()->x < player->GetCollider().x) ? 1 : -1;
            player->TakeDamage(damage, dir);
            };
        };

    m_evilCreature->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_evilCreature->GetCollider(); }));
    m_evilCreature2->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_evilCreature2->GetCollider(); }));
    m_evilCreature3->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_evilCreature3->GetCollider(); }));
    m_evilCreature4->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_evilCreature4->GetCollider(); }));
    m_evilCreature5->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_evilCreature5->GetCollider(); }));
    m_ghostWarrior->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_ghostWarrior->GetCollider(); }));
    m_ghostWarrior2->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_ghostWarrior2->GetCollider(); }));
    m_ghostWarrior3->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_ghostWarrior3->GetCollider(); }));
    m_ghostWarrior4->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_ghostWarrior4->GetCollider(); }));
    m_ghostWarrior5->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_ghostWarrior5->GetCollider(); }));
    m_ghostWarriorRed->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_ghostWarriorRed->GetCollider(); }));
    m_ghostWarriorRed2->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_ghostWarriorRed2->GetCollider(); }));
    m_ghostWarriorRed3->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_ghostWarriorRed3->GetCollider(); }));
    m_ghostWarriorRed4->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_ghostWarriorRed4->GetCollider(); }));
    m_rangedEnemy->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_rangedEnemy->GetCollider(); }));
    m_rangedEnemy2->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_rangedEnemy2->GetCollider(); }));
    m_rangedEnemy3->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_rangedEnemy3->GetCollider(); }));

    // Callback khi player đánh enemy
    auto createHitEnemyCallback = [player](std::function<SDL_Rect* ()> getCollider, std::function<void(int, int)> takeDamageFunc) {
        return [player, getCollider, takeDamageFunc](int damage) {
            int dir = (player->GetCollider().x > getCollider()->x) ? -1 : 1;
            takeDamageFunc(damage, dir);
            };
        };

    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_evilCreature->GetCollider(); }, [this](int d, int dir) { m_evilCreature->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_evilCreature2->GetCollider(); }, [this](int d, int dir) { m_evilCreature2->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_evilCreature3->GetCollider(); }, [this](int d, int dir) { m_evilCreature3->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_evilCreature4->GetCollider(); }, [this](int d, int dir) { m_evilCreature4->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_evilCreature5->GetCollider(); }, [this](int d, int dir) { m_evilCreature5->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_ghostWarrior->GetCollider(); }, [this](int d, int dir) { m_ghostWarrior->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_ghostWarrior2->GetCollider(); }, [this](int d, int dir) { m_ghostWarrior2->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_ghostWarrior3->GetCollider(); }, [this](int d, int dir) { m_ghostWarrior3->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_ghostWarrior4->GetCollider(); }, [this](int d, int dir) { m_ghostWarrior4->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_ghostWarrior5->GetCollider(); }, [this](int d, int dir) { m_ghostWarrior5->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_ghostWarriorRed->GetCollider(); }, [this](int d, int dir) { m_ghostWarriorRed->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_ghostWarriorRed2->GetCollider(); }, [this](int d, int dir) { m_ghostWarriorRed2->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_ghostWarriorRed3->GetCollider(); }, [this](int d, int dir) { m_ghostWarriorRed3->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_ghostWarriorRed4->GetCollider(); }, [this](int d, int dir) { m_ghostWarriorRed4->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_rangedEnemy->GetCollider(); }, [this](int d, int dir) { m_rangedEnemy->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_rangedEnemy2->GetCollider(); }, [this](int d, int dir) { m_rangedEnemy2->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_rangedEnemy3->GetCollider(); }, [this](int d, int dir) { m_rangedEnemy3->TakeDamage(d, dir); }));
}


void Level2::SetupSpawnExp(std::shared_ptr<PlayerController> player)
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

    setupSpawn(m_evilCreature);
    setupSpawn(m_evilCreature2);
    setupSpawn(m_evilCreature3);
    setupSpawn(m_evilCreature4);
    setupSpawn(m_evilCreature5);
    setupSpawn(m_ghostWarrior);
    setupSpawn(m_ghostWarrior2);
    setupSpawn(m_ghostWarrior3);
    setupSpawn(m_ghostWarrior4);
    setupSpawn(m_ghostWarrior5);
    setupSpawn(m_ghostWarriorRed);
    setupSpawn(m_ghostWarriorRed2);
    setupSpawn(m_ghostWarriorRed3);
    setupSpawn(m_ghostWarriorRed4);
    setupSpawn(m_rangedEnemy);
    setupSpawn(m_rangedEnemy2);
    setupSpawn(m_rangedEnemy3);
}

std::vector<std::shared_ptr<Sprite2D>> Level2::GetBackgrounds()
{
    return m_backgroundList;
}

