#pragma once
// ExpOrb.h
#pragma once
#include "Sprite2D.h"
#include "SpriteAnimation.h"
#include <functional>

class ExpOrb {
public:
    ExpOrb(Vector2 pos);
    void Update(float dt, Vector2 playerPos);
    void Draw(SDL_Renderer* renderer);
    bool IsCollected() const;
    void SetOnCollectedCallback(std::function<void()> callback);

private:
    std::shared_ptr<SpriteAnimation> m_sprite;
    std::function<void()> m_onCollected = nullptr;
    bool m_isCollected;

    float m_speed = INT_MAX;
};
