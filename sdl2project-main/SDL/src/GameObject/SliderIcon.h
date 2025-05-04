#pragma once

#include <functional>
#include"Sprite2D.h"

class SliderIcon : public Sprite2D
{
public:

	//Initializes internal variable
	SliderIcon(std::shared_ptr<TextureManager> texture, SDL_RendererFlip flip);

	//Handles mouse event
	bool HandleTouchEvent(SDL_Event* e);
	void SetRange(int min, int max);


private:
	int m_min, m_max;
	bool isDraggable;
};
