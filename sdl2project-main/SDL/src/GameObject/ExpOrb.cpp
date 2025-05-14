#include "ExpOrb.h"
#include "ResourceManagers.h"
#include "Camera.h"
#include "SpriteAnimation.h"
#include <cmath> // cho sqrt

ExpOrb::ExpOrb(Vector2 pos)
    : m_isCollected(false), m_speed(300.0f)
{
    std::shared_ptr<TextureManager> texture = ResourceManagers::GetInstance()->GetTexture("exp.png");
    m_sprite = std::make_shared<SpriteAnimation>(texture, 1, 1, 1, 0.05f);
    m_sprite->SetFrameTime(INFINITY); // không anim
    m_sprite->SetSize(32, 32);
    m_sprite->Set2DPosition(pos.x, pos.y);
}

void ExpOrb::Update(float dt, Vector2 playerPos) {
    if (m_isCollected) return;

    // Dùng hướng bay + Lerp mềm mại, nhanh, không vọt
    Vector2 orbPos = m_sprite->Get2DPosition();

    float dx = playerPos.x - orbPos.x;
    float dy = playerPos.y - orbPos.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance < 60.0f) {
        m_isCollected = true;
        if (m_onCollected) m_onCollected();
        return;
    }


    // Hệ số Lerp: càng lớn thì bay càng nhanh (0.0f → 1.0f)
    float lerpFactor = 4.0f * dt; // tăng hệ số nếu muốn nhanh hơn
    orbPos.x += dx * lerpFactor;
    orbPos.y += dy * lerpFactor;

    m_sprite->Set2DPosition(orbPos.x, orbPos.y);

}

void ExpOrb::Draw(SDL_Renderer* renderer) {
    if (!m_isCollected && m_sprite) {
        m_sprite->Draw(renderer);
    }
}

bool ExpOrb::IsCollected() const {
    return m_isCollected;
}

void ExpOrb::SetOnCollectedCallback(std::function<void()> callback) {
    m_onCollected = callback;
}
