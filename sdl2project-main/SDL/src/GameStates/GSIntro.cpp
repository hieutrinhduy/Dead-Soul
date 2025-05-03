#include "GSIntro.h"

#include "DataSaver.h"
#include "ResourceManagers.h"

#include "Sound.h"


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

	newRow1 = false; newRow2 = false;
	runIntro = false; runLogo = true;
	introFrameRateFlag = true; logoFrameRateFlag = true;
}


GSIntro::~GSIntro()
{
}


void GSIntro::Init()
{
	// BLACK BACKGROUND:
	auto texture = ResourceManagers::GetInstance()->GetTexture("black_background.png");
	sceneBackground = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	sceneBackground->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	sceneBackground->Set2DPosition(0, 0);


	// load INTRO and LOGO:
	if (introType == 1) {
		texture = ResourceManagers::GetInstance()->GetTexture("intro.png");
		intro = std::make_shared<SpriteAnimation>(texture, 1, INTRO_SPRITE_COLS, INTRO_SPRITE_ROWS, 2.0f);
		intro->SetFlip(SDL_FLIP_NONE);
		intro->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
		intro->Set2DPosition(0, 0);
	}
	else {
		texture = ResourceManagers::GetInstance()->GetTexture("intro.png");
		intro = std::make_shared<SpriteAnimation>(texture, 1, INTRO_SPRITE_COLS, INTRO_SPRITE_ROWS, 0.125f);
		intro->SetFlip(SDL_FLIP_NONE);
		intro->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
		intro->Set2DPosition(0, 0);

		texture = ResourceManagers::GetInstance()->GetTexture("logo_2.png");
		logo = std::make_shared<SpriteAnimation>(texture, 1, LOGO_SPRITE_COLS, LOGO_SPRITE_ROWS, 0.1f);
		logo->SetFlip(SDL_FLIP_NONE);
		logo->SetSize(LOGO_SIZE);
		logo->Set2DPosition((SCREEN_WIDTH - logo->GetWidth()) / 2.0f, -50);
	}


	// SOUNDS:
	Sound::GetInstance()->LoadSfx("pop_1.wav");
	Sound::GetInstance()->LoadSfx("pop_2.wav");
	Sound::GetInstance()->LoadSfx("fire_whoosh.wav");
	Sound::GetInstance()->LoadSfx("fire_blow_out.wav");
	Sound::GetInstance()->LoadSfx("fire_ignite.wav");
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

		// frame 1: fire whoosh sfx
		if (introFrameRateFlag && intro->GetCurrentFrame() == 1 && intro->GetCurrentAction() == 1) {
			Sound::GetInstance()->PlaySfx("fire_whoosh.wav");
			intro->SetFrameTime(0.1f);
			introFrameRateFlag = false;
		}
		// frame 5: fire blow out sfx
		else if (!introFrameRateFlag && intro->GetCurrentFrame() == 5 && intro->GetCurrentAction() == 1) {
			Sound::GetInstance()->PlaySfx("fire_blow_out.wav");
			introFrameRateFlag = true;
		}
		// frame 11: ignite fire
		else if (introFrameRateFlag && intro->GetCurrentFrame() == 5 && intro->GetCurrentAction() == 2) {
			Sound::GetInstance()->PlaySfx("fire_ignite.wav");
			intro->SetFrameTime(0.125f);
			introFrameRateFlag = false;
		}
		// last frame
		else if (intro->GetCurrentFrame() == INTRO_SPRITE_COLS - 1 && intro->GetCurrentAction() == INTRO_SPRITE_ROWS) {
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

			// frame 1: fire whoosh sfx
			if (introFrameRateFlag && intro->GetCurrentFrame() == 1 && intro->GetCurrentAction() == 1) {
				Sound::GetInstance()->PlaySfx("fire_whoosh.wav");
				introFrameRateFlag = false;
			}
			// frame 5: fire blow out sfx
			else if (!introFrameRateFlag && intro->GetCurrentFrame() == 5 && intro->GetCurrentAction() == 1) {
				Sound::GetInstance()->PlaySfx("fire_blow_out.wav");
				introFrameRateFlag = true;
			}
			// frame 10
			else if (introFrameRateFlag && intro->GetCurrentFrame() == 4 && intro->GetCurrentAction() == 2) {
				intro->SetFrameTime(2.0f);
				logo->SetCurrentFrame(logo->GetCurrentFrame() + 1);
				runLogo = true;
				introFrameRateFlag = false;
			}
			// frame 11: ignite fire
			else if (!introFrameRateFlag && intro->GetCurrentFrame() == 5 && intro->GetCurrentAction() == 2) {
				Sound::GetInstance()->PlaySfx("fire_ignite.wav");
				intro->SetFrameTime(0.125f);
				introFrameRateFlag = true;
			}
			// last frame
			else if (intro->GetCurrentFrame() == INTRO_SPRITE_COLS - 1 && intro->GetCurrentAction() == INTRO_SPRITE_ROWS) {
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

			// frame 11: completed fade in transition
			if (logoFrameRateFlag && logo->GetCurrentFrame() == 1 && logo->GetCurrentAction() == 3) {
				logo->SetFrameTime(0.7f);
				logoFrameRateFlag = false;
			}
			// frame 14: open eyes
			else if (!logoFrameRateFlag && logo->GetCurrentFrame() == 4 && logo->GetCurrentAction() == 3) {
				Sound::GetInstance()->PlaySfx("pop_1.wav");
				logoFrameRateFlag = true;
			}
			// frame 15: look sideways
			else if (logoFrameRateFlag && logo->GetCurrentFrame() == 0 && logo->GetCurrentAction() == 4) {
				Sound::GetInstance()->PlaySfx("pop_2.wav");
				logoFrameRateFlag = false;
			}
			// frame 16: look sideways
			else if (!logoFrameRateFlag && logo->GetCurrentFrame() == 1 && logo->GetCurrentAction() == 4) {
				Sound::GetInstance()->PlaySfx("pop_2.wav");
				logoFrameRateFlag = true;
			}

			// frame 17: activate intro animation and deactivate logo animation
			else if (logoFrameRateFlag && logo->GetCurrentFrame() == 2 && logo->GetCurrentAction() == 4) {
				runIntro = true; runLogo = false;
				logoFrameRateFlag = false;
			}
			// frame 18
			else if (!logoFrameRateFlag && logo->GetCurrentFrame() == 3 && logo->GetCurrentAction() == 4) {
				logo->SetFrameTime(1.0f);
				logoFrameRateFlag = true;
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
