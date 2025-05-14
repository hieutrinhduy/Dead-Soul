#pragma once
#include "SpriteAnimation.h"
#include <memory>

class ChangeLevelGate
{
public:
    ChangeLevelGate();
    ~ChangeLevelGate();

    void Init(float posX, float posY);
    void Update(float deltaTime);
    void Draw(SDL_Renderer* renderer);

    Vector2 GetGateSize() const;
    Vector2 GetGatePos() const;
private:
    std::shared_ptr<SpriteAnimation> m_animation;
};
