// ExpOrb.cpp
#include "ExpOrb.h"
#include "ResourceManagers.h"
#include "Camera.h"
#include "SpriteAnimation.h"

ExpOrb::ExpOrb(Vector2 pos)
    : m_isCollected(false)
{
    std::shared_ptr<TextureManager> texture = ResourceManagers::GetInstance()->GetTexture("exp.png");
    m_sprite = std::make_shared<SpriteAnimation>(texture, 1,1,1,0.05f);
    m_sprite->SetFrameTime(INFINITY);
    m_sprite->SetSize(32, 32);
    float x = pos.x;
    float y = pos.y;
    m_sprite->Set2DPosition(x, y);
}

void ExpOrb::Update(float dt) {

}

void ExpOrb::Draw(SDL_Renderer* renderer) {
    if (!m_isCollected && m_sprite) {
        m_sprite->Draw(renderer);
    }
}

bool ExpOrb::IsCollected() const {
    return m_isCollected;
}
