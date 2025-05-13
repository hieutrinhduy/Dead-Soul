#pragma once
// ExpOrb.h
#pragma once
#include "Sprite2D.h"
#include "SpriteAnimation.h"

class ExpOrb {
public:
    ExpOrb(Vector2 pos);
    void Update(float dt);
    void Draw(SDL_Renderer* renderer);
    bool IsCollected() const;

private:
    std::shared_ptr<SpriteAnimation> m_sprite;
    bool m_isCollected;
};
