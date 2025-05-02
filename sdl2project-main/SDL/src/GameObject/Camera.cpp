#include"Camera.h"

SDL_Rect Camera::GetViewBox()
{
	return m_ViewBox;
}

Vector2 Camera::GetPosition()
{
	return m_Position;
}

void Camera::SetTarget(std::shared_ptr<BaseObject> target) 
{
	m_target = target;
}

void Camera::Shake(float duration, float magnitude)
{
	m_shakeTimer = duration;
	m_shakeMagnitude = magnitude;
	m_originalPosition = m_Position;
}

void Camera::Update(float deltaTime)
{
	if (m_target != nullptr)
	{
		//Center the camera over the dot
		m_ViewBox.x = (m_target->GetPosition().x - SCREEN_WIDTH / 2);
		m_ViewBox.y = (m_target->GetPosition().y  - SCREEN_HEIDHT / 2) ;
		
		//Keep the camera in bounds
		if (m_ViewBox.x < 0)
		{
			m_ViewBox.x = 0;
		}
		if (m_ViewBox.y < 0)
		{
			m_ViewBox.y = 0;
		}
		if (m_ViewBox.x > (GetLevelWidth() - m_ViewBox.w))
		{
			m_ViewBox.x = (GetLevelWidth() - m_ViewBox.w);
		}
		if (m_ViewBox.y > (GetLevelHeight() - m_ViewBox.h))
		{
			m_ViewBox.y = (GetLevelHeight() - m_ViewBox.h);
		}
		m_Position = Vector2(m_ViewBox.x, m_ViewBox.y);
	}

	if (m_shakeTimer > 0)
	{
		float offsetX = (rand() % 1000 / 1000.0f * 2.0f - 1.0f) * m_shakeMagnitude;
		float offsetY = (rand() % 1000 / 1000.0f * 2.0f - 1.0f) * m_shakeMagnitude;

		m_Position.x += offsetX;
		m_Position.y += offsetY;

		m_shakeTimer -= deltaTime;

		if (m_shakeTimer <= 0.0f)
		{
			m_Position = m_originalPosition;
			m_shakeTimer = 0.0f;
			m_shakeMagnitude = 0.0f;
		}
	}
}

void Camera::SetLevelDimension(int width, int height)
{
	m_levelWidth = width;
	m_levelHeight = height;
}


void Camera::ResetCameraPosition() {
	m_Position.x = 0;
	m_Position.y = 0;
}



int Camera::GetLevelWidth()
{
	return m_levelWidth;
}

int Camera::GetLevelHeight()
{
	return m_levelHeight;
}
