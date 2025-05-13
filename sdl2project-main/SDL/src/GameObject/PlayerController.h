#pragma once

#include <memory>
#include <SDL.h>
#include <functional>
#include <thread>
#include <mutex> 
#include "SpriteAnimation.h"
#include "iostream"


class PlayerController
{
public:
    PlayerController();
    ~PlayerController();

    void Init();
    void HandleKeyEvents(SDL_Event& e);
    void HandleMouseEvents(SDL_Event& e);
    void Update(float deltaTime);
    void Draw(SDL_Renderer* renderer);


    /* -------- GETTER: -------- */
    const SDL_Rect& GetCollider() const { return m_playerCollider; }

    // get movement ability data:
    bool IsJumping() const { return m_velocityY < 0; };
    bool IsFalling() const { return m_velocityY > 0; };
    bool CanGoLeft() const { return m_canGoLeft; };
    bool CanGoRight() const { return m_canGoRight; };
    bool BumpTheHead() const { return m_bumpTheHead; };
    bool OnTheGround() const { return m_isGrounded; };

    // get player's stats data:
    int GetHpLose() {
        std::lock_guard<std::mutex> lock(mtx);
        return m_HpLose;
    };
    int GetSpLose() {
        std::lock_guard<std::mutex> lock(mtx);
        return m_SpLose;
    };
    int GetMpLose() {
        std::lock_guard<std::mutex> lock(mtx);
        return m_MpLose;
    };
    int GetExpGained() {
        std::lock_guard<std::mutex> lock(mtx);
        return m_ExpGained;
    }; 
    int GetGoldGained() {
        std::lock_guard<std::mutex> lock(mtx);
        return m_GoldGained;
    };

    // get weapon index:
    int GetWeaponIndex() {
        std::lock_guard<std::mutex> lock(mtx);
        return m_currentWeaponIndex;
    };

    int GetPlayerDamage() {
        std::lock_guard<std::mutex> lock(mtx);
        return m_damage;
    }

    /* -------- SETTER: -------- */
    // set movement ability data:
    void SetMovementState(bool canGoLeft, bool canGoRight, bool bumpTheHead, bool isGrounded) {
        m_canGoLeft = canGoLeft; m_canGoRight = canGoRight;
        m_bumpTheHead = bumpTheHead; m_isGrounded = isGrounded;
    };
    void SetMovementLimit(float left, float right, float top, float bottom) {
        leftLimit = left; rightLimit = right;
        topLimit = top; bottomLimit = bottom;
    };

    // set player status:
    void SetSpStatus(bool enoughSp) {
        std::lock_guard<std::mutex> lock(mtx);
        m_enoughSP = enoughSp;
    };
    void SetMpStatus(bool enoughMp) {
        std::lock_guard<std::mutex> lock(mtx);
        m_enoughMP = enoughMp;
    };
    void SetHpLose(int damage) {
        std::lock_guard<std::mutex> lock(mtx);
        m_HpLose += damage;
    };
    void SetMpLose(int mpLose) {
        std::lock_guard<std::mutex> lock(mtx);
        m_MpLose += mpLose;
    };
    void SetSpLose(int spLose) {
        std::lock_guard<std::mutex> lock(mtx);
        m_SpLose += spLose;
    };

    void SetExpGained(int amount) {
        std::cout << "Amount Exp Gained" << amount << std::endl;
        std::lock_guard<std::mutex> lock(mtx);
        m_ExpGained += amount;
    }

    void SetGoldGained(int amount) {
        std::cout << "Amount Gold Gained" << amount << std::endl;
        std::lock_guard<std::mutex> lock(mtx);
        m_GoldGained += amount;
    }

    void ResetChangeInValue() {
        std::lock_guard<std::mutex> lock(mtx);
        m_HpLose = 0;
        m_SpLose = 0;
        m_MpLose = 0;
    };

    void SetPlayerDamage(int damage) {
        std::lock_guard<std::mutex> lock(mtx);
        m_damage = damage;
    }

    // set Weapon Index:
    void SetWeaponIndex(int index) 
    {
        std::lock_guard<std::mutex> lock(mtx);
        m_currentWeaponIndex = index; 
        switch (index)
        {
        case 0://Default Sword
            m_attackDuration = 0.39f;
            break;
        case 1://Arrow
            m_attackDuration = 0.59;
            break;
        }
    };

    void ResetPlayerPosition();

    // set collider:
    void SetGroundCollider(SDL_Rect* ground);
    void SetEnemyCollider(SDL_Rect* enemy);
    void SetEnemyColliders(const std::vector<SDL_Rect*>& enemies);

    // set callback:
    void SetOnHitEnemyCallbacks(const std::vector<std::function<void(int)>>& callbacks);
    void SetOnScreenShakeCallback(std::function<void()> callback) { m_onScreenShake = callback; }


    void TakeDamage(int damage, int knockbackDir);
    void Die();

    bool IsDead() const { return m_isDead; }

    void LoseTheirFooting();


private:
    std::shared_ptr<SpriteAnimation> m_player;
    SDL_Rect m_playerCollider;

    // variables that detemine MOVEMENT_ABILITY:
    bool m_canGoLeft, m_canGoRight, m_bumpTheHead, m_isGrounded;
    float leftLimit, rightLimit, topLimit, bottomLimit;

    //jump variables
    bool m_isJumping;
    bool m_isFalling;

    int m_currentWeaponIndex = 0;   
    // 0 = Default Sword
    // 1 = Arrow

    float m_gravityForce;
    float m_jumpForce;
    float m_velocityX;
    float m_velocityY;
    float m_speed;
    static float m_airTime;
    int m_jumpCount;
    const int m_maxJump;

    bool m_forceJumpAnimation;
    float m_forceJumpTimer;
    const float m_forceJumpDuration;

    SDL_RendererFlip m_playerFlip;
    int m_KeyPress;

    bool m_isAttacking;
    float m_attackTimer;
    float m_attackDuration;
    float m_lastAttackTime;
    bool m_justDidAttack2;
    bool m_justDidAttack3;

    bool m_isRolling;
    float m_rollTimer;
    const float m_rollDuration;
    const float m_rollSpeed;

    SDL_Rect* m_groundCollider;
    std::vector<SDL_Rect*> m_enemyColliders;

    //// HP và trạng thái chiến đấu
    //int m_maxHP = 5;
    //int m_currentHP = 5;

    bool m_isDead = false;
    bool m_isHit = false;
    float m_hitTimer = 0.0f;
    bool m_didDamage = false;
    bool m_canDealDamage;
    int m_damage = 10;

    //MP and SP
    bool m_enoughSP;
    bool m_enoughMP;

    //Ingame HP lose, SP, MP
    int m_HpLose = 0;
    int m_SpLose = 0;
    int m_MpLose = 0;
    int m_ExpGained = 0;
    int m_GoldGained = 0;
    public: std::mutex mtx;

    //Knock Back
    bool m_isKnockback = false;
    float m_knockbackTimer = 0.0f;
    float m_knockbackDuration = 0.1f;
    float m_knockbackSpeed = 1000.0f;
    int m_knockbackDir = 0; // -1 = trái, 1 = phải

    // TEXTURES:
    std::shared_ptr<TextureManager> idleTexture;
    std::shared_ptr<TextureManager> runTexture;
    std::shared_ptr<TextureManager> jumpTexture;
    std::shared_ptr<TextureManager> jumpOnAirTexture;
    std::shared_ptr<TextureManager> jumpFallTexture;
    std::shared_ptr<TextureManager> landTexture;
    std::shared_ptr<TextureManager> fallTexture;
    std::shared_ptr<TextureManager> rollTexture;
    std::shared_ptr<TextureManager> attackTexture, attack2Texture, attack3Texture; //DEFAULT WEAPON
    std::shared_ptr<TextureManager> arrowAttackTexture;// ARROW WEAPON
    std::shared_ptr<TextureManager> hurtTexture;
    std::shared_ptr<TextureManager> deadTexture;

    //std::function<void(int damage)> m_onHitEnemy;
    std::vector<std::function<void(int)>> m_onHitEnemyList;
    std::function<void()> m_onScreenShake;

    //Check Attack Move flag
    bool m_didAttackMove = false;

    //Kiem tra da goi ham die hay chua
    bool m_hasDied = false;


    void ShootProjectile();
    void ChangeAnimation(std::shared_ptr<TextureManager> texture, int frameCount, float frameTime);
    bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b);
    bool GroundCheck();
    void DestroyPlayer();
};

