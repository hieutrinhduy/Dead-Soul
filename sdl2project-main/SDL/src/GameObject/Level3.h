#pragma once
#include <vector>
#include <memory>
#include "EnemyController.h"
#include "RangedEnemy.h"
#include "GhostWarrior.h"
#include "GhostWarriorRed.h"
#include "EvilCreature.h"
#include "Boss1.h"
#include "SecretBoss.h"
#include "PlayerController.h"
#include "Sprite2D.h"
#include "ChangeLevelGate.h"
#include "SecretGate.h"
#include "Shop.h"

class Level3 {
public:
    Level3(std::shared_ptr<PlayerController> playerController);
    ~Level3();

    void Update(float dt);
    void Draw(SDL_Renderer* renderer);

    std::vector<SDL_Rect*> GetEnemyColliders();
    std::vector<std::function<void(int)>> GetHitEnemyCallbacks();

    std::vector<std::shared_ptr<Sprite2D>> GetBackgrounds();
    std::shared_ptr<ChangeLevelGate> GetChangeLevelGate() const { return m_gate; }
    std::shared_ptr<SecretGate> GetSecretGate() const { return m_secretGate; }
    std::shared_ptr<Shop> GetShop() const { return m_shop; }

private:
    std::shared_ptr<PlayerController> m_playerController;
    std::shared_ptr<EnemyController> m_enemyController;
    std::shared_ptr<EnemyController> m_enemyController2;
    std::shared_ptr<EvilCreature> m_evilCreature;
    std::shared_ptr<GhostWarrior> m_ghostWarrior;
    std::shared_ptr<GhostWarriorRed> m_ghostWarriorRed;
    std::shared_ptr<RangedEnemy> m_rangedEnemy;
    std::shared_ptr<Boss1> m_boss1;
    std::shared_ptr<ScecretBoss> m_boss2;

    std::shared_ptr<ChangeLevelGate> m_gate;
    std::shared_ptr<SecretGate> m_secretGate;
    std::shared_ptr<Shop> m_shop;

    std::vector<SDL_Rect*> m_enemyColliders;
    std::vector<std::function<void(int)>> m_hitCallbacks;

    std::vector<std::shared_ptr<SpriteAnimation>> m_decorList;

    //Background
    std::vector<std::shared_ptr<Sprite2D>> m_backgroundList;

    bool m_isCameraMovingToGate = false;
    float m_cameraWaitTimer = 0.0f;
    float m_waitDurationAtGate = 2.0f;
    bool m_isReturningToPlayer = false;
    bool m_isWaitingAtGate = false;

    void SetupCallbacks(std::shared_ptr<PlayerController> player);
    void SetupSpawnExp(std::shared_ptr<PlayerController> player);
};
