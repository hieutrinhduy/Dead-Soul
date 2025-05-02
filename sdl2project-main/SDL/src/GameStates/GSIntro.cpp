#include "GSIntro.h"

#include "DataSaver.h"
#include "ResourceManagers.h"


constexpr int INTRO_SPRITE_COLS = 6;
constexpr int INTRO_SPRITE_ROWS = 3;

constexpr int LOGO_SPRITE_COLS = 5;
constexpr int LOGO_SPRITE_ROWS = 4;
#define LOGO_SIZE 600,600

GSIntro::GSIntro() : GameStateBase(StateType::STATE_INTRO)
{
	if (DataSaver::GetInstance()->LoadGame("save.dat").isWatchedStory) introType = 2;
	else {
		introType = 1;
		DataSaver::GetInstance()->SetSaveData(true, 1);
		DataSaver::GetInstance()->SaveGame("save.dat");
	}

	newRow1 = true; newRow2 = true;
	runIntro = false; runLogo = true;
	introFrameRateFlag = true; logoFrameRateFlag = true;
}


GSIntro::~GSIntro()
{
}


void GSIntro::Init()
{
	auto texture = ResourceManagers::GetInstance()->GetTexture("black_background.png");
	sceneBackground = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	sceneBackground->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	sceneBackground->Set2DPosition(0, 0);

	if (introType == 1) {
		texture = ResourceManagers::GetInstance()->GetTexture("intro.png");
		intro = std::make_shared<SpriteAnimation>(texture, 0, INTRO_SPRITE_COLS, INTRO_SPRITE_ROWS, 0.1f);
		intro->SetFlip(SDL_FLIP_NONE);
		intro->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
		intro->Set2DPosition(0, 0);
	}
	else {
		texture = ResourceManagers::GetInstance()->GetTexture("intro.png");
		intro = std::make_shared<SpriteAnimation>(texture, 0, INTRO_SPRITE_COLS, INTRO_SPRITE_ROWS, 0.125f);
		intro->SetFlip(SDL_FLIP_NONE);
		intro->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
		intro->Set2DPosition(0, 0);

		texture = ResourceManagers::GetInstance()->GetTexture("logo_2.png");
		logo = std::make_shared<SpriteAnimation>(texture, 0, LOGO_SPRITE_COLS, LOGO_SPRITE_ROWS, 0.1f);
		logo->SetFlip(SDL_FLIP_NONE);
		logo->SetSize(LOGO_SIZE);
		logo->Set2DPosition((SCREEN_WIDTH - logo->GetWidth()) / 2.0f, -50);
	}
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
	if (introType == 1) {
		intro->Update(deltaTime);

		if (newRow1 && intro->GetCurrentFrame() == 0) {
			intro->SetCurrentAction(intro->GetCurrentAction() + 1);
			newRow1 = false;
		}
		else if (!newRow1 && intro->GetCurrentFrame() == 1) newRow1 = true;

		if (intro->GetCurrentFrame() == INTRO_SPRITE_COLS - 1 && intro->GetCurrentAction() == INTRO_SPRITE_ROWS) {
			GameStateMachine::GetInstance()->ChangeState(StateType::STATE_MENU);
		}
	}
	
	else {
		if (runIntro) {
			intro->Update(deltaTime);

			if (newRow1 && intro->GetCurrentFrame() == 0) {
				intro->SetCurrentAction(intro->GetCurrentAction() + 1);
				newRow1 = false;
			}
			else if (!newRow1 && intro->GetCurrentFrame() == 1) newRow1 = true;

			// frame 10
			if (introFrameRateFlag && intro->GetCurrentFrame() == 4 && intro->GetCurrentAction() == 2) {
				intro->SetFrameTime(2.0f);
				logo->SetCurrentFrame(logo->GetCurrentFrame() + 1);
				runLogo = true;
				introFrameRateFlag = false;
			}
			// frame 11
			if (!introFrameRateFlag && intro->GetCurrentFrame() == 5 && intro->GetCurrentAction() == 2) {
				intro->SetFrameTime(0.125f);
				introFrameRateFlag = true;
			}
			// last frame
			if (intro->GetCurrentFrame() == INTRO_SPRITE_COLS - 1 && intro->GetCurrentAction() == INTRO_SPRITE_ROWS) {
				GameStateMachine::GetInstance()->ChangeState(StateType::STATE_MENU);
			}
		}

		if (runLogo) {
			logo->Update(deltaTime);

			if (newRow2 && logo->GetCurrentFrame() == 0) {
				logo->SetCurrentAction(logo->GetCurrentAction() + 1);
				newRow2 = false;
			}
			else if (!newRow2 && logo->GetCurrentFrame() == 1) newRow2 = true;

			// frame 11
			if (logoFrameRateFlag && logo->GetCurrentFrame() == 1 && logo->GetCurrentAction() == 3) {
				logo->SetFrameTime(0.7f);
				logoFrameRateFlag = false;
			}
			// frame 17: activate intro animation and deactivate logo animation
			else if (logo->GetCurrentFrame() == 2 && logo->GetCurrentAction() == 4) {
				runIntro = true; runLogo = false;
			}
			// frame 18
			else if (logo->GetCurrentFrame() == 3 && logo->GetCurrentAction() == 4) {
				logo->SetFrameTime(1.0f);
			}
			// last frame
			else if (logo->GetCurrentFrame() == 4 && logo->GetCurrentAction() == 4) {
				runLogo = false;
			}
		}
	}
}

void GSIntro::Draw(SDL_Renderer* renderer)
{
	sceneBackground->Draw(renderer);
	intro->Draw(renderer);
	if (introType == 2) logo->Draw(renderer);
}
