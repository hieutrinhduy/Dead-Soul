#include "GSMenu.h"
#include "ResourceManagers.h"
#include "MouseButton.h"
#include "Sound.h"


// unit: pixel
constexpr float SPRITE_WIDTH = 128;
constexpr float SPRITE_HEIGHT = 82;
constexpr int BG_SPRITE_COLS = 4, BG_SPRITE_ROWS = 1;
constexpr int BTN_PLAY_POS_X = 49, BTN_PLAY_POS_Y = 69, BTN_PLAY_WIDTH = 30, BTN_PLAY_HEIGHT = 8;
constexpr int BTN_CREDIT_POS_X = 4, BTN_CREDIT_POS_Y = 65, BTN_CREDIT_WIDTH = 13, BTN_CREDIT_HEIGHT = 15;
constexpr int BTN_SETTINGS_POS_X = 108, BTN_SETTINGS_POS_Y = 61, BTN_SETTINGS_WIDTH = 19, BTN_SETTINGS_HEIGHT = 19;

#define BTN_PLAY_POSITION (SCREEN_WIDTH - btnPlay->GetWidth())/2, SCREEN_HEIDHT - 100
#define BTN_EXIT_SIZE 75,75
#define BTN_EXIT_POSITION SCREEN_WIDTH - 80, 16


GSMenu::GSMenu() : GameStateBase(StateType::STATE_MENU),
				   m_background(nullptr), m_listButton(std::list<std::shared_ptr<MouseButton>>{}), m_textGameName(nullptr),
				   widthScaling(SCREEN_WIDTH / SPRITE_WIDTH), heightScaling(SCREEN_HEIDHT / SPRITE_HEIGHT), buttonIndex(0)
{
	btnPlayLeft = widthScaling * BTN_PLAY_POS_X; btnPlayRight = btnPlayLeft + widthScaling * BTN_PLAY_WIDTH;
	btnPlayTop = heightScaling * BTN_PLAY_POS_Y; btnPlayBottom = btnPlayTop + heightScaling * BTN_PLAY_HEIGHT;

	btnCreditLeft = widthScaling * BTN_CREDIT_POS_X; btnCreditRight = btnCreditLeft + widthScaling * BTN_CREDIT_WIDTH;
	btnCreditTop = heightScaling * BTN_CREDIT_POS_Y; btnCreditBottom = btnCreditTop + heightScaling * BTN_CREDIT_HEIGHT;

	btnSettingsLeft = widthScaling * BTN_SETTINGS_POS_X; btnSettingsRight = btnSettingsLeft + widthScaling * BTN_SETTINGS_WIDTH;
	btnSettingsTop = heightScaling * BTN_SETTINGS_POS_Y; btnSettingsBottom = btnSettingsTop + heightScaling * BTN_SETTINGS_HEIGHT;
}


GSMenu::~GSMenu()
{
}



void GSMenu::Init()
{
	//auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D.nfg");

	// background
	//auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
	auto texture = ResourceManagers::GetInstance()->GetTexture("bg_menu.png");
	m_background = std::make_shared<SpriteAnimation>(texture, 1, BG_SPRITE_COLS, BG_SPRITE_ROWS, 0.25f);
	m_background->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	m_background->Set2DPosition(0, 0);


	// PLAY button
	texture = ResourceManagers::GetInstance()->GetTexture("btn_play.png");
	btnPlay = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
	btnPlay->SetSize(widthScaling * BTN_PLAY_WIDTH, heightScaling * BTN_PLAY_HEIGHT);
	btnPlay->Set2DPosition(btnPlayLeft, btnPlayTop);
	btnPlay->SetOnClick([]() {
		GameStateMachine::GetInstance()->ChangeState(StateType::STATE_PLAY);
	});
	m_listButton.push_back(btnPlay);


	// EXIT button
	texture = ResourceManagers::GetInstance()->GetTexture("btn_close.png");
	btnExit = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
	//btnClose = std::make_shared<MouseButton>(texture);
	btnExit->SetSize(BTN_EXIT_SIZE);
	btnExit->Set2DPosition(BTN_EXIT_POSITION);
	btnExit->SetOnClick([]() {
		if (Mix_PlayingMusic() > 0) {
			Mix_HaltMusic();
		}
		exit(0);
	});
	m_listButton.push_back(btnExit);


	//CREDIT button
	texture = ResourceManagers::GetInstance()->GetTexture("btn_credit.png");
	btnCredit = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
	btnCredit->SetSize(widthScaling * BTN_CREDIT_WIDTH, heightScaling * BTN_CREDIT_HEIGHT);
	btnCredit->Set2DPosition(btnCreditLeft, btnCreditTop);
	btnCredit->SetOnClick([]() {
		GameStateMachine::GetInstance()->ChangeState(StateType::STATE_CREDIT);
	});
	m_listButton.push_back(btnCredit);
	

	// SETTINGS button
	texture = ResourceManagers::GetInstance()->GetTexture("btn_settings.png");
	btnSettings = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
	btnSettings->SetSize(widthScaling * BTN_SETTINGS_WIDTH, heightScaling * BTN_SETTINGS_HEIGHT);
	btnSettings->Set2DPosition(btnSettingsLeft, btnSettingsTop);
	btnSettings->SetOnClick([]() {
		GameStateMachine::GetInstance()->ChangeState(StateType::STATE_SETTINGS);
	});
	m_listButton.push_back(btnSettings);


	/// scene's SOUND
	Sound::GetInstance()->LoadSound("background_music.mp3");
	Sound::GetInstance()->PlaySound("background_music.mp3");

	Sound::GetInstance()->LoadSfx("fire_sound.wav");
	Sound::GetInstance()->PlaySfx("fire_sound.wav", -1);

	Sound::GetInstance()->LoadSfx("ui_pop.wav");
}


void GSMenu::Exit()
{
	
}


void GSMenu::Pause()
{
	if (GameStateMachine::GetInstance()->NextState()->GetGameStateType() != StateType::STATE_CREDIT &&
		GameStateMachine::GetInstance()->NextState()->GetGameStateType() != StateType::STATE_SETTINGS) {
		Mix_HaltMusic();
		Mix_Pause(-1);
	}
}



void GSMenu::Resume()
{
	Camera::GetInstance()->ResetCameraPosition();
	buttonIndex = 0;

	if (GameStateMachine::GetInstance()->PrevState()->GetGameStateType() != StateType::STATE_CREDIT &&
		GameStateMachine::GetInstance()->PrevState()->GetGameStateType() != StateType::STATE_SETTINGS) {
		// Reload and Replay MUSIC:
		Sound::GetInstance()->LoadSound("background_music.mp3");
		Sound::GetInstance()->PlaySound("background_music.mp3");

		// Resume CHUNK:
		Mix_Resume(-1);
	}
}



void GSMenu::HandleEvents()
{
}

void GSMenu::HandleKeyEvents(SDL_Event& e)
{
}


void GSMenu::HandleTouchEvents(SDL_Event& e)
{
	for (auto button : m_listButton)
	{
		if (button ->HandleTouchEvent(&e))
		{
			break;
		}
	}

	if (e.type == SDL_MOUSEMOTION) {
		int x = e.motion.x;
		int y = e.motion.y;

		if (x >= btnPlayLeft && x <= btnPlayRight && y >= btnPlayTop && y <= btnPlayBottom) {
			if (buttonIndex != 1) Sound::GetInstance()->PlaySfx("ui_pop.wav");
			buttonIndex = 1;
		}
		else if (x >= btnCreditLeft && x <= btnCreditRight && y >= btnCreditTop && y <= btnCreditBottom) {
			if (buttonIndex != 2) Sound::GetInstance()->PlaySfx("ui_pop.wav");
			buttonIndex = 2;
		}
		else if (x >= btnSettingsLeft && x <= btnSettingsRight && y >= btnSettingsTop && y <= btnSettingsBottom) {
			if (buttonIndex != 3) Sound::GetInstance()->PlaySfx("ui_pop.wav");
			buttonIndex = 3;
		}
		else {
			buttonIndex = 0;
		}
	}
}


void GSMenu::HandleMouseMoveEvents(SDL_Event& e)
{
}


void GSMenu::Update(float deltaTime)
{
	m_background->Update(deltaTime);
	/*for (auto it : m_listButton)
	{
		it->Update(deltaTime);
	}*/
}


void GSMenu::Draw(SDL_Renderer* renderer)
{
	m_background->Draw(renderer);
	
	if (buttonIndex == 1) btnPlay->Draw(renderer);
	else if (buttonIndex == 2) btnCredit->Draw(renderer);
	else if (buttonIndex == 3) btnSettings->Draw(renderer);

	btnExit->Draw(renderer);
}
