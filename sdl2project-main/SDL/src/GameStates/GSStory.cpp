#include "GSStory.h"
#include "ResourceManagers.h"
#include "GameObject/MouseButton.h"
#include "GameObject/Sprite2D.h"
#include "GameObject/SpriteAnimation.h"
#include "GameObject/Camera.h"
#include "KeyState.h"
#include "Sound.h"
GSStory::GSStory() : GameStateBase(StateType::STATE_INTRO), m_textColor({ 255, 255, 255 }), m_time(0.0f), sentence1(true), sentence2(true)
{
}


GSStory::~GSStory()
{
}

void GSStory::Init()
{
	auto texture = ResourceManagers::GetInstance()->GetTexture("player.png");
	projector = std::make_shared<SpriteAnimation>(texture, 4, 6, 5, 0.1f);
	projector->SetFlip(SDL_FLIP_NONE);
	projector->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT * 16 / 10);
	projector->Set2DPosition(0, -SCREEN_HEIDHT * 3 / 10);

	// set text
	auto font = ResourceManagers::GetInstance()->GetFont("arial.ttf", 48);
	m_textGameName = std::make_shared<Text>("The war between humans and ghosts takes place for thousands year...", font, m_textColor);
	m_textGameName->SetSize(SCREEN_WIDTH * 0.8f, 60);
	m_textGameName->Set2DPosition((SCREEN_WIDTH - m_textGameName->GetWidth()) / 2, SCREEN_HEIDHT - 60);
}

void GSStory::Exit()
{
}


void GSStory::Pause()
{
}

void GSStory::Resume()
{

}


void GSStory::HandleEvents()
{
}

void GSStory::HandleKeyEvents(SDL_Event& e)
{
}

void GSStory::HandleTouchEvents(SDL_Event& e)
{
}

void GSStory::HandleMouseMoveEvents(SDL_Event& e)
{
}

void GSStory::Update(float deltaTime)
{
	projector->Update(deltaTime);

	if (projector->GetCurrentFrame() == 1 && sentence1 == sentence2) sentence2 = !sentence2;
	if (projector->GetCurrentFrame() == 0 && sentence1 != sentence2) {
		if (!sentence2) projector->SetCurrentAction(5);
		else projector->SetCurrentAction(4);
		sentence1 = sentence2;
	}

	/*if (sentence1 && projector->GetCurrentFrame() == 1) {
		m_textGameName->SetText("Wisdom and resilience give humans an advantage over the fierce monsters...");
		sentence1 = false;
	}
	else if (sentence2 && projector->GetCurrentFrame() == 2) {
		m_textGameName->SetText("But the relentless proliferation of countless monsters is gradually becoming a nightmare for us....");
		sentence2 = false;
	}*/

	m_time += deltaTime;
	if (m_time > 14.5f)
	{
		GameStateMachine::GetInstance()->ChangeState(StateType::STATE_MENU);
		m_time = 0;
	}
}

void GSStory::Draw(SDL_Renderer* renderer)
{
	projector->Draw(renderer);
	m_textGameName->Draw(renderer);
}
