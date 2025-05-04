#include "SliderIcon.h"

SliderIcon::SliderIcon(std::shared_ptr<TextureManager> texture, SDL_RendererFlip flip)
	: Sprite2D(texture, flip), m_min(0), m_max(1), isDraggable(false)
{
}



bool SliderIcon::HandleTouchEvent(SDL_Event* e)
{
	int x, y;
	SDL_GetMouseState(&x, &y);

	if (!isDraggable) {
		if (e->type == SDL_MOUSEBUTTONDOWN)
		{
			if (x >= m_position.x && x <= m_position.x + m_iWidth && y >= m_position.y && y <= m_position.y + m_iHeight)
			{
				isDraggable = true;
			}
		}
	}

	else {
		if (e->type == SDL_MOUSEBUTTONUP) {
			isDraggable = false;
		}


		if (e->type == SDL_MOUSEMOTION) {
			m_position.x += e->motion.xrel;
			if (m_position.x < m_min) {
				m_position.x = m_min;
			}
			else if (m_position.x > m_max) {
				m_position.x = m_max;
			}
		}
	}

	return isDraggable;
}



void SliderIcon::SetRange(int min, int max)
{
	m_min = min;
	m_max = max;
}
