#pragma once
#include "SpriteAnimation.h"
#include <memory>

class Shop
{
public:
    Shop();
    ~Shop();

    void Init(float posX, float posY);
    void Update(float deltaTime);
    void Draw(SDL_Renderer* renderer);

    Vector2 GetShopSize() const;
    Vector2 GetShopPos() const;
private:
    std::shared_ptr<SpriteAnimation> m_animation;
};
