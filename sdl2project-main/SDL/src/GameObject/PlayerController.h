#pragma once
#include "SpriteAnimation.h"
#include <memory>
#include <SDL.h>

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

    void SetGroundCollider(SDL_Rect* ground);
    void SetEnemyCollider(SDL_Rect* enemy);

private:
    std::shared_ptr<SpriteAnimation> m_player;
    SDL_Rect m_playerCollider;

    float m_gravityForce;
    float m_jumpForce;
    float m_velocityY;
    float m_speed;
    static float m_airTime;
    int m_jumpCount;
    const int m_maxJump;
    bool m_isGrounded;

    bool m_forceJumpAnimation;
    float m_forceJumpTimer;
    const float m_forceJumpDuration;

    SDL_RendererFlip m_playerFlip;
    int m_KeyPress;

    bool m_isAttacking;
    float m_attackTimer;
    const float m_attackDuration;
    float m_lastAttackTime;
    bool m_justDidAttack2;

    bool m_isRolling;
    float m_rollTimer;
    const float m_rollDuration;
    const float m_rollSpeed;

    SDL_Rect* m_groundCollider;
    SDL_Rect* m_enemyCollider;

    // Texture Manager
    std::shared_ptr<TextureManager> idleTexture;
    std::shared_ptr<TextureManager> runTexture;
    std::shared_ptr<TextureManager> jumpTexture;
    std::shared_ptr<TextureManager> jumpFallTexture;
    std::shared_ptr<TextureManager> fallTexture;
    std::shared_ptr<TextureManager> rollTexture;
    std::shared_ptr<TextureManager> attackTexture;
    std::shared_ptr<TextureManager> attack2Texture;

private:
    void ChangeAnimation(std::shared_ptr<TextureManager> texture, int frameCount, float frameTime);
    bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b);
    bool GroundCheck();
};
