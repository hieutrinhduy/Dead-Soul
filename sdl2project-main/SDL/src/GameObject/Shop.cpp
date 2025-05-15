#include "Shop.h"
#include "ResourceManagers.h"

Shop::Shop() {}

Shop::~Shop() {}

void Shop::Init(float posX, float posY)
{
    auto texture = ResourceManagers::GetInstance()->GetTexture("shop_anim.png");
    m_animation = std::make_shared<SpriteAnimation>(texture, 1, 6, 1, 0.05f);
    m_animation->SetSize(96 * 6, 96 * 6);
    m_animation->Set2DPosition(posX, posY);
}

void Shop::Update(float deltaTime)
{
    if (m_animation)
        m_animation->Update(deltaTime);
}

void Shop::Draw(SDL_Renderer* renderer)
{
    if (m_animation)
        m_animation->Draw(renderer);
}

Vector2 Shop::GetShopSize() const
{
    if (!m_animation) return Vector2(0, 0);
    return Vector2(m_animation->GetWidth(), m_animation->GetHeight());
}

Vector2 Shop::GetShopPos() const
{
    return m_animation ? m_animation->Get2DPosition() : Vector2(0, 0);
}

