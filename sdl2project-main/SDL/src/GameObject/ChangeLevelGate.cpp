#include "ChangeLevelGate.h"
#include "ResourceManagers.h"

ChangeLevelGate::ChangeLevelGate() {}

ChangeLevelGate::~ChangeLevelGate() {}

void ChangeLevelGate::Init(float posX, float posY)
{
    auto texture = ResourceManagers::GetInstance()->GetTexture("NextLevelPortal.png");
    m_animation = std::make_shared<SpriteAnimation>(texture, 1, 6, 1, 0.05f);
    m_animation->SetSize(96 * 2, 64 * 2);
    m_animation->Set2DPosition(posX, posY);
}

void ChangeLevelGate::Update(float deltaTime)
{
    if (m_animation)
        m_animation->Update(deltaTime);
}

void ChangeLevelGate::Draw(SDL_Renderer* renderer)
{
    if (m_animation)
        m_animation->Draw(renderer);
}

Vector2 ChangeLevelGate::GetGateSize() const
{
    if (!m_animation) return Vector2(0, 0);
    return Vector2(m_animation->GetWidth(), m_animation->GetHeight());
}

Vector2 ChangeLevelGate::GetGatePos() const
{
    return m_animation ? m_animation->Get2DPosition() : Vector2(0, 0);
}

