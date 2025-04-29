#include "GSLogo.h"
#include "ResourceManagers.h"
#include "Define.h"
#include "Sprite2D.h"

#define INTRO_TIME 3.0f

GSLogo::GSLogo() : GameStateBase(StateType::STATE_LOGO), m_time(0.0f)
{
}


GSLogo::~GSLogo()
{
}

void GSLogo::Init()
{
	auto texture = ResourceManagers::GetInstance()->GetTexture("logo.tga");
	m_logo = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	m_logo->SetSize(450, 300);
	m_logo->Set2DPosition((float)(SCREEN_WIDTH - m_logo->GetWidth()) / 2, (float)(SCREEN_HEIDHT - m_logo->GetHeight()) / 2);

}

void GSLogo::Exit()
{
}


void GSLogo::Pause()
{
}

void GSLogo::Resume()
{

}


void GSLogo::HandleEvents()
{
}

void GSLogo::HandleKeyEvents(SDL_Event& e)
{
}

void GSLogo::HandleTouchEvents(SDL_Event& e)
{
}

void GSLogo::HandleMouseMoveEvents(SDL_Event& e)
{
}

void GSLogo::Update(float deltaTime)
{
	m_time += deltaTime;
	if (m_time > INTRO_TIME)
	{
		GameStateMachine::GetInstance()->ChangeState(StateType::STATE_STORY);
		m_time = 0;
	}
}

void GSLogo::Draw(SDL_Renderer* renderer)
{
	m_logo->Draw(renderer);
}
