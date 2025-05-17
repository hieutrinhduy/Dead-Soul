#include "Level1.h"
#include "ExpOrb.h"
#include "Camera.h"
#include "ResourceManagers.h"
#include "Sprite2D.h"
#include "SpriteAnimation.h"

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

    m_gate = std::make_shared<ChangeLevelGate>();
    m_gate->Init(3190, 2650);

    m_shop = std::make_shared<Shop>();
    m_shop->Init(300, 0);

#pragma region Decor

    auto level1Decor = ResourceManagers::GetInstance()->GetTexture("decorations/Level1Decor.png");
    if (level1Decor != nullptr)
    {
        auto decor = std::make_shared<SpriteAnimation>(level1Decor, 1, 1, 1, 10);
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

#pragma endregion
    // Khởi tạo enemy
    m_enemyController = std::make_shared<EnemyController>();
    m_enemyController->Init(10, 1200, 865);

    m_enemyController2 = std::make_shared<EnemyController>();
    m_enemyController2->Init(1220, 1790, 385);

    m_enemyController3 = std::make_shared<EnemyController>();
    m_enemyController3->Init(1600, 3900, 865);

    m_enemyController4 = std::make_shared<EnemyController>();
    m_enemyController4->Init(1850, 3100, 325);

    m_enemyController5 = std::make_shared<EnemyController>();
    m_enemyController5->Init(4050, 4650, 320);

    m_enemyController6 = std::make_shared<EnemyController>();
    m_enemyController6->Init(3450, 4030, 450);

    m_enemyController7 = std::make_shared<EnemyController>();
    m_enemyController7->Init(4700, 5420, 450);

    m_enemyController8 = std::make_shared<EnemyController>();
    m_enemyController8->Init(4200, 5820, 865);

    m_enemyController9 = std::make_shared<EnemyController>();
    m_enemyController9->Init(10, 2750, 1795);

    m_enemyController10 = std::make_shared<EnemyController>();
    m_enemyController10->Init(420, 4198, 1315);

    m_enemyController11 = std::make_shared<EnemyController>();
    m_enemyController11->Init(580, 5800, 2270);

    m_enemyController12 = std::make_shared<EnemyController>();
    m_enemyController12->Init(580, 5800, 2690);

    m_evilCreature = std::make_shared<EvilCreature>();
    m_evilCreature->Init(10, 2750, 1800);

    m_evilCreature2 = std::make_shared<EvilCreature>();
    m_evilCreature2->Init(420, 4198, 1320);

    m_evilCreature3 = std::make_shared<EvilCreature>();
    m_evilCreature3->Init(580, 5800, 2280);

    m_evilCreature4 = std::make_shared<EvilCreature>();
    m_evilCreature4->Init(580, 5800, 2700);

    //m_ghostWarrior = std::make_shared<GhostWarrior>();
    //m_ghostWarrior->Init(1850, 3000, 325);

    //m_ghostWarriorRed = std::make_shared<GhostWarriorRed>();
    //m_ghostWarriorRed->Init(3550, 5700, 300);

    m_rangedEnemy = std::make_shared<RangedEnemy>();
    m_rangedEnemy->Init(1580, 4800, 2280);
    m_rangedEnemy->SetPlayerController(player.get());

    m_rangedEnemy2 = std::make_shared<RangedEnemy>();
    m_rangedEnemy2->Init(1580, 4800, 2700);
    m_rangedEnemy2->SetPlayerController(player.get());

    //m_boss1 = std::make_shared<Boss1>();
    //m_boss1->Init(600, 1900, 960);

    //m_boss2 = std::make_shared<ScecretBoss>();
    //m_boss2->Init(600, 1900, 1030);

    // Cài đặt player collider
    m_enemyController->SetPlayerCollider(&player->GetCollider());
    m_enemyController2->SetPlayerCollider(&player->GetCollider());
    m_enemyController3->SetPlayerCollider(&player->GetCollider());
    m_enemyController4->SetPlayerCollider(&player->GetCollider());
    m_enemyController5->SetPlayerCollider(&player->GetCollider());
    m_enemyController6->SetPlayerCollider(&player->GetCollider());
    m_enemyController7->SetPlayerCollider(&player->GetCollider());
    m_enemyController8->SetPlayerCollider(&player->GetCollider());
    m_enemyController9->SetPlayerCollider(&player->GetCollider());
    m_enemyController10->SetPlayerCollider(&player->GetCollider());
    m_enemyController11->SetPlayerCollider(&player->GetCollider());
    m_enemyController12->SetPlayerCollider(&player->GetCollider());
    m_evilCreature->SetPlayerCollider(&player->GetCollider());
    m_evilCreature2->SetPlayerCollider(&player->GetCollider());
    m_evilCreature3->SetPlayerCollider(&player->GetCollider());
    m_evilCreature4->SetPlayerCollider(&player->GetCollider());
    //m_ghostWarrior->SetPlayerCollider(&player->GetCollider());
    //m_ghostWarriorRed->SetPlayerCollider(&player->GetCollider());
    m_rangedEnemy->SetPlayerCollider(&player->GetCollider());
    m_rangedEnemy2->SetPlayerCollider(&player->GetCollider());
    //m_boss1->SetPlayerCollider(&player->GetCollider());
    //m_boss2->SetPlayerCollider(&player->GetCollider());

    // Thiết lập các callback
    SetupCallbacks(player);
    SetupSpawnExp(player);

    // Lưu collider
    m_enemyColliders = {
        m_enemyController->GetEnemyCollider(),
        m_enemyController2->GetEnemyCollider(),
        m_enemyController3->GetEnemyCollider(),
        m_enemyController4->GetEnemyCollider(),
        m_enemyController5->GetEnemyCollider(),
        m_enemyController6->GetEnemyCollider(),
        m_enemyController7->GetEnemyCollider(),
        m_enemyController8->GetEnemyCollider(),
        m_enemyController9->GetEnemyCollider(),
        m_enemyController10->GetEnemyCollider(),
        m_enemyController11->GetEnemyCollider(),
        m_enemyController12->GetEnemyCollider(),
        m_evilCreature->GetCollider(),
        m_evilCreature2->GetCollider(),
        m_evilCreature3->GetCollider(),
        m_evilCreature4->GetCollider(),
        //m_ghostWarrior->GetCollider(),
        //m_ghostWarriorRed->GetCollider(),
        m_rangedEnemy->GetCollider(),
        m_rangedEnemy2->GetCollider()
        //m_boss1->GetEnemyCollider(),
        //m_boss2->GetEnemyCollider()
    };
}

Level1::~Level1() {}

void Level1::Update(float dt)
{
    m_gate->Update(dt);
    m_shop->Update(dt);
    m_enemyController->Update(dt);
    m_enemyController2->Update(dt);
    m_enemyController3->Update(dt);
    m_enemyController4->Update(dt);
    m_enemyController5->Update(dt);
    m_enemyController6->Update(dt);
    m_enemyController7->Update(dt);
    m_enemyController8->Update(dt);
    m_enemyController9->Update(dt);
    m_enemyController10->Update(dt);
    m_enemyController11->Update(dt);
    m_enemyController12->Update(dt);
    m_evilCreature->Update(dt);
    m_evilCreature2->Update(dt);
    m_evilCreature3->Update(dt);
    m_evilCreature4->Update(dt);
    //m_ghostWarrior->Update(dt);
    //m_ghostWarriorRed->Update(dt);
    m_rangedEnemy->Update(dt);
    m_rangedEnemy2->Update(dt);
    //m_boss1->Update(dt);
    //m_boss2->Update(dt);
}

void Level1::Draw(SDL_Renderer* renderer)
{
    m_gate->Draw(renderer);
    m_shop->Draw(renderer);
    for (auto& decor : m_decorList)
    {
        decor->Draw(renderer);
    }
    m_enemyController->Draw(renderer);
    m_enemyController2->Draw(renderer);
    m_enemyController3->Draw(renderer);
    m_enemyController4->Draw(renderer);
    m_enemyController5->Draw(renderer);
    m_enemyController6->Draw(renderer);
    m_enemyController7->Draw(renderer);
    m_enemyController8->Draw(renderer);
    m_enemyController9->Draw(renderer);
    m_enemyController10->Draw(renderer);
    m_enemyController11->Draw(renderer);
    m_enemyController12->Draw(renderer);
    m_evilCreature->Draw(renderer);
    m_evilCreature2->Draw(renderer);
    m_evilCreature3->Draw(renderer);
    m_evilCreature4->Draw(renderer);
    //m_ghostWarrior->Draw(renderer);
    //m_ghostWarriorRed->Draw(renderer);
    m_rangedEnemy->Draw(renderer);
    m_rangedEnemy2->Draw(renderer);
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
    m_enemyController3->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_enemyController3->GetEnemyCollider(); }));
    m_enemyController4->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_enemyController4->GetEnemyCollider(); }));
    m_enemyController5->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_enemyController5->GetEnemyCollider(); }));
    m_enemyController6->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_enemyController6->GetEnemyCollider(); }));
    m_enemyController7->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_enemyController7->GetEnemyCollider(); }));
    m_enemyController8->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_enemyController8->GetEnemyCollider(); }));
    m_enemyController9->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_enemyController9->GetEnemyCollider(); }));
    m_enemyController10->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_enemyController10->GetEnemyCollider(); }));
    m_enemyController11->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_enemyController11->GetEnemyCollider(); }));
    m_enemyController12->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_enemyController12->GetEnemyCollider(); }));
    m_evilCreature->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_evilCreature->GetCollider(); }));
    m_evilCreature2->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_evilCreature2->GetCollider(); }));
    m_evilCreature3->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_evilCreature3->GetCollider(); }));
    m_evilCreature4->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_evilCreature4->GetCollider(); }));
    //m_ghostWarrior->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_ghostWarrior->GetCollider(); }));
    //m_ghostWarriorRed->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_ghostWarriorRed->GetCollider(); }));
    m_rangedEnemy->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_rangedEnemy->GetCollider(); }));
    m_rangedEnemy2->SetOnHitPlayerCallback(createHitPlayerCallback([this]() { return m_rangedEnemy2->GetCollider(); }));
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
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_enemyController3->GetEnemyCollider(); }, [this](int d, int dir) { m_enemyController3->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_enemyController4->GetEnemyCollider(); }, [this](int d, int dir) { m_enemyController4->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_enemyController5->GetEnemyCollider(); }, [this](int d, int dir) { m_enemyController5->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_enemyController6->GetEnemyCollider(); }, [this](int d, int dir) { m_enemyController6->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_enemyController7->GetEnemyCollider(); }, [this](int d, int dir) { m_enemyController7->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_enemyController8->GetEnemyCollider(); }, [this](int d, int dir) { m_enemyController8->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_enemyController9->GetEnemyCollider(); }, [this](int d, int dir) { m_enemyController9->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_enemyController10->GetEnemyCollider(); }, [this](int d, int dir) { m_enemyController10->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_enemyController11->GetEnemyCollider(); }, [this](int d, int dir) { m_enemyController11->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_enemyController12->GetEnemyCollider(); }, [this](int d, int dir) { m_enemyController12->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_evilCreature->GetCollider(); }, [this](int d, int dir) { m_evilCreature->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_evilCreature2->GetCollider(); }, [this](int d, int dir) { m_evilCreature2->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_evilCreature3->GetCollider(); }, [this](int d, int dir) { m_evilCreature3->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_evilCreature4->GetCollider(); }, [this](int d, int dir) { m_evilCreature4->TakeDamage(d, dir); }));
    //m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_ghostWarrior->GetCollider(); }, [this](int d, int dir) { m_ghostWarrior->TakeDamage(d, dir); }));
    //m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_ghostWarriorRed->GetCollider(); }, [this](int d, int dir) { m_ghostWarriorRed->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_rangedEnemy->GetCollider(); }, [this](int d, int dir) { m_rangedEnemy->TakeDamage(d, dir); }));
    m_hitCallbacks.push_back(createHitEnemyCallback([this]() { return m_rangedEnemy2->GetCollider(); }, [this](int d, int dir) { m_rangedEnemy2->TakeDamage(d, dir); }));

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
    setupSpawn(m_enemyController3);
    setupSpawn(m_enemyController4);
    setupSpawn(m_enemyController5);
    setupSpawn(m_enemyController6);
    setupSpawn(m_enemyController7);
    setupSpawn(m_enemyController8);
    setupSpawn(m_enemyController9);
    setupSpawn(m_enemyController10);
    setupSpawn(m_enemyController11);
    setupSpawn(m_enemyController12);
    setupSpawn(m_evilCreature);
    setupSpawn(m_evilCreature2);
    setupSpawn(m_evilCreature3);
    setupSpawn(m_evilCreature4);
    //setupSpawn(m_ghostWarrior);
    //setupSpawn(m_ghostWarriorRed);
    setupSpawn(m_rangedEnemy);
    setupSpawn(m_rangedEnemy2);
    //setupSpawn(m_boss1);
    //setupSpawn(m_boss2);
}

std::vector<std::shared_ptr<Sprite2D>> Level1::GetBackgrounds()
{
    return m_backgroundList;
}

