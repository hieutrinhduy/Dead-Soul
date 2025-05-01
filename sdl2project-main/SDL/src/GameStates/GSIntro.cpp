#include "GSIntro.h"
#include "ResourceManagers.h"

constexpr int SPRITE_COLS = 6;
constexpr int SPRITE_ROWS = 3;

GSIntro::GSIntro() : GameStateBase(StateType::STATE_INTRO), newRow(true)
{
}


GSIntro::~GSIntro()
{
}


void GSIntro::Init()
{
	auto texture = ResourceManagers::GetInstance()->GetTexture("intro_1.png");
	intro = std::make_shared<SpriteAnimation>(texture, 0, SPRITE_COLS, SPRITE_ROWS, 0.1f);
	intro->SetFlip(SDL_FLIP_NONE);
	intro->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	intro->Set2DPosition(0, 0);
}


void GSIntro::Exit()
{
}


void GSIntro::Pause()
{
}

void GSIntro::Resume()
{
}


void GSIntro::HandleEvents()
{
}

void GSIntro::HandleKeyEvents(SDL_Event& e)
{
}

void GSIntro::HandleTouchEvents(SDL_Event& e)
{
}

void GSIntro::HandleMouseMoveEvents(SDL_Event& e)
{
}

void GSIntro::Update(float deltaTime)
{
	intro->Update(deltaTime);

	if (newRow && intro->GetCurrentFrame() == 0) {
		intro->SetCurrentAction(intro->GetCurrentAction() + 1);
		newRow = false;
	}
	if (!newRow && intro->GetCurrentFrame() == 1) newRow = true;

	if (intro->GetCurrentFrame() == SPRITE_COLS - 1 && intro->GetCurrentAction() == SPRITE_ROWS) {
		GameStateMachine::GetInstance()->ChangeState(StateType::STATE_MENU);
	}
}

void GSIntro::Draw(SDL_Renderer* renderer)
{
	intro->Draw(renderer);
}
