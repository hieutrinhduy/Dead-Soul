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
	if (m_isLerpingToGate)
	{
		SetLerpingRoutineState(true);
		m_lerpTime += deltaTime;
		float t = m_lerpTime / m_lerpDuration;
		if (t >= 1.0f)
		{
			t = 1.0f;
			m_isLerpingToGate = false;
		}

		m_Position.x = m_lerpStartX * (1 - t) + m_lerpTargetX * t;
		m_Position.y = m_lerpStartY * (1 - t) + m_lerpTargetY * t;

		m_ViewBox.x = static_cast<int>(m_Position.x);
		m_ViewBox.y = static_cast<int>(m_Position.y);
		return;
	}

	if (m_isLerpingRoutine) { 
		m_shakeTimer = INT_MIN;
		return;
	}

	if (m_target != nullptr)
	{ 
		//Center the camera over the dot
		m_ViewBox.x = (m_target->GetPosition().x - SCREEN_WIDTH / 2);
		m_ViewBox.y = (m_target->GetPosition().y  - SCREEN_HEIDHT / 2) ;

		//Lui 2 tiles
		int marginTiles = 3;
		int tileSize = 32;

		//Tinh toan gioi han
		int minX = marginTiles * tileSize;
		int minY = marginTiles * tileSize;
		int maxX = GetLevelWidth() - m_ViewBox.w - minX;
		int maxY = GetLevelHeight() - m_ViewBox.h - minY;
		
		////Keep the camera in bounds
		//if (m_ViewBox.x < 0)
		//{
		//	m_ViewBox.x = 0;
		//}
		//if (m_ViewBox.y < 0)
		//{
		//	m_ViewBox.y = 0;
		//}
		//if (m_ViewBox.x > (GetLevelWidth() - m_ViewBox.w))
		//{
		//	m_ViewBox.x = (GetLevelWidth() - m_ViewBox.w);
		//}
		//if (m_ViewBox.y > (GetLevelHeight() - m_ViewBox.h))
		//{
		//	m_ViewBox.y = (GetLevelHeight() - m_ViewBox.h);
		//}

		//Gioi han 2 ô dimemsion
		if (m_ViewBox.x < minX)
			m_ViewBox.x = minX;
		if (m_ViewBox.y < minY)
			m_ViewBox.y = minY;
		if (m_ViewBox.x > maxX)
			m_ViewBox.x = maxX;
		if (m_ViewBox.y > maxY)
			m_ViewBox.y = maxY;
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

void Camera::LerpToPosition(float targetX, float targetY, float duration)
{
	m_isLerpingToGate = true;
	m_lerpStartX = m_Position.x;
	m_lerpStartY = m_Position.y;
	m_lerpTargetX = targetX;
	m_lerpTargetY = GetPosition().y;
	m_lerpTime = 0.0f;
	m_lerpDuration = duration;
	SetTarget(nullptr);
}

void Camera::SetManualPosition(float centerX, float centerY)
{
	m_ViewBox.x = static_cast<int>(centerX - SCREEN_WIDTH / 2);
	m_ViewBox.y = static_cast<int>(centerY - SCREEN_HEIDHT / 2);
	m_Position.x = static_cast<float>(m_ViewBox.x);
	m_Position.y = static_cast<float>(m_ViewBox.y);
}

bool Camera::IsLerping() const
{
	return m_isLerpingToGate;
}

