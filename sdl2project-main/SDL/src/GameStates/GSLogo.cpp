#include "GSLogo.h"

#include "ResourceManagers.h"

#include "Define.h"


#define LOGO_SIZE 800, 800
constexpr int SPRITE_COLS = 7;
constexpr int SPRITE_ROWS = 4;

/*	DETAILS OF LOGO'S ANIMATION
	The animation consists of (7 x 4) frames.
	The first 11 frames (0 -> 10) and the last 11 frames (17 -> 27): run in 0.06s,
	frames 11 -> 16: run in 0.7s.
*/

GSLogo::GSLogo() : GameStateBase(StateType::STATE_LOGO), m_readyToChangeState(false), m_newRow(true), m_reduceFPS(true)
{
}


GSLogo::~GSLogo()
{
}


void GSLogo::Init()
{
	auto texture = ResourceManagers::GetInstance()->GetTexture("black_background.png");
	sceneBackground = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	sceneBackground->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	sceneBackground->Set2DPosition(0, 0);

	texture = ResourceManagers::GetInstance()->GetTexture("logo.png");
	logoAnimation = std::make_shared<SpriteAnimation>(texture, 0, SPRITE_COLS, SPRITE_ROWS, 0.06f);
	logoAnimation->SetFlip(SDL_FLIP_NONE);
	logoAnimation->SetSize(LOGO_SIZE);
	logoAnimation->Set2DPosition( (SCREEN_WIDTH - logoAnimation->GetWidth()) / 2.0f,
								  (SCREEN_HEIDHT - logoAnimation->GetHeight()) / 3.0f );
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
	logoAnimation->Update(deltaTime);

	if (m_newRow && logoAnimation->GetCurrentFrame() == 0) {
		logoAnimation->SetCurrentAction(logoAnimation->GetCurrentAction() + 1);
		m_newRow = false;
	}
	if (!m_newRow && logoAnimation->GetCurrentFrame() == 1) m_newRow = true;

	// frame 11
	if (m_reduceFPS && logoAnimation->GetCurrentFrame() == 4 && logoAnimation->GetCurrentAction() == 2) {
		logoAnimation->SetFrameTime(0.7f);
		m_reduceFPS = false;
	}
	// frame 17
	else if (!m_reduceFPS && logoAnimation->GetCurrentFrame() == 3 && logoAnimation->GetCurrentAction() == 3) {
		logoAnimation->SetFrameTime(0.06f);
		m_reduceFPS = true;
	}
	// last frame
	else if (logoAnimation->GetCurrentFrame() == SPRITE_COLS - 1 && logoAnimation->GetCurrentAction() == SPRITE_ROWS) {
		GameStateMachine::GetInstance()->ChangeState(StateType::STATE_STORY);
	}
}


void GSLogo::Draw(SDL_Renderer* renderer)
{
	sceneBackground->Draw(renderer);
	logoAnimation->Draw(renderer);
}
