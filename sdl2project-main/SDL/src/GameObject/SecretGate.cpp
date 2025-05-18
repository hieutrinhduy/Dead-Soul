#include "SecretGate.h"
#include "ResourceManagers.h"
#include "iostream"

SecretGate::SecretGate() {}

SecretGate::~SecretGate() {}

void SecretGate::Init(float posX, float posY)
{
    auto texture = ResourceManagers::GetInstance()->GetTexture("SecretPortal.png");
    m_animation = std::make_shared<SpriteAnimation>(texture, 1, 5, 1, 0.1f);
    m_animation->SetSize(96 * 3, 64 * 3);
    m_animation->Set2DPosition(posX, posY);
}

void SecretGate::Update(float deltaTime)
{
    if (m_animation)
        m_animation->Update(deltaTime);
}

void SecretGate::Draw(SDL_Renderer* renderer)
{
    if (m_animation)
        m_animation->Draw(renderer);
}

Vector2 SecretGate::GetGateSize() const
{
    if (!m_animation) return Vector2(0, 0);
    return Vector2(m_animation->GetWidth(), m_animation->GetHeight());
}

Vector2 SecretGate::GetGatePos() const
{
    return m_animation ? m_animation->Get2DPosition() : Vector2(0, 0);
}

void SecretGate::SetGatePos(Vector2 pos)
{
    m_animation->Set2DPosition(pos.x, pos.y);
}