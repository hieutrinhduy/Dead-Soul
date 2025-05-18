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

    void SetGatePos(Vector2 pos);

    Vector2 GetGateSize() const;
    Vector2 GetGatePos() const;

    Vector2 GetFinalGatePos() 
    {
        Vector2 pos = Vector2(finalPosX, finalPosY);
        return pos;
    }
private:
    std::shared_ptr<SpriteAnimation> m_animation;

    float finalPosX = 3000;
    float finalPosY = 400;
};
