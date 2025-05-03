#include "GSSettings.h"

#include "DataSaver.h"
#include "ResourceManagers.h"

#include "Sound.h"


// unit: pixel
constexpr int BACKGROUND_SPRITES_WIDTH = 128, BACKGROUND_SPRITES_HEIGHT = 72;
constexpr int TXT_MUSIC_WIDTH = 23, TXT_MUSIC_HEIGHT = 7;
constexpr int TXT_SFX_WIDTH = 13, TXT_SFX_HEIGHT = 11;
constexpr int BAR_WIDTH = 16, BAR_HEIGHT = 5;
constexpr int ICON_SIZE = 9; // altered compared to reality ( 19x19 px )

constexpr float DECAY_FACTOR = 0.8f;


GSSettings::GSSettings() : GameStateBase(StateType::STATE_SETTINGS)
{
	widthScaling = SCREEN_WIDTH * DECAY_FACTOR / BACKGROUND_SPRITES_WIDTH ;
	heightScaling = SCREEN_HEIDHT * DECAY_FACTOR / BACKGROUND_SPRITES_HEIGHT;

	milestone = SCREEN_WIDTH * 1 / 4;
	bar1Top = SCREEN_HEIDHT * 1 / 4;
	bar2Top = SCREEN_HEIDHT * 1 / 2;

	// volumn in the range of 0 to 128, read from 'save.dat'
	musicVolume = 128;
	sfxVolume = 90;

	fullVolumeBarWidth = SCREEN_WIDTH * 3 / 4 - milestone;

	float ratio = (float) fullVolumeBarWidth / BAR_WIDTH;
	iconStartPoint = milestone + ratio - ICON_SIZE * widthScaling / 2;
	iconEndPoint = milestone + fullVolumeBarWidth - ratio - ICON_SIZE * widthScaling / 2;
}


GSSettings::~GSSettings()
{
}


void GSSettings::Init()
{
	// SETTINGS BACKGROUND:
	auto texture = ResourceManagers::GetInstance()->GetTexture("bg_settings.png");
	auto sceneBackground = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	sceneBackground->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	sceneBackground->Set2DPosition(0, 0);
	elements.push_back(sceneBackground);


	// text MUSIC:
	texture = ResourceManagers::GetInstance()->GetTexture("txt_music.png");
	auto txt_music = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	txt_music->SetSize(widthScaling * TXT_MUSIC_WIDTH, heightScaling * TXT_MUSIC_HEIGHT);
	txt_music->Set2DPosition(milestone - txt_music->GetWidth() - 20, bar1Top - (TXT_MUSIC_HEIGHT - BAR_HEIGHT) * heightScaling);
	elements.push_back(txt_music);

	// text SFX:
	texture = ResourceManagers::GetInstance()->GetTexture("txt_sfx.png");
	auto txt_sfx = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	txt_sfx->SetSize(widthScaling * TXT_SFX_WIDTH, heightScaling * TXT_SFX_HEIGHT);
	txt_sfx->Set2DPosition(milestone - txt_sfx->GetWidth() - 20, bar2Top - (TXT_SFX_HEIGHT - BAR_HEIGHT) * heightScaling);
	elements.push_back(txt_sfx);


	// SETTINGS BACKGROUND BARS:
	texture = ResourceManagers::GetInstance()->GetTexture("img_background_bar.png");
	auto backgroundBar1 = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	backgroundBar1->SetSize(fullVolumeBarWidth, heightScaling * BAR_HEIGHT);
	backgroundBar1->Set2DPosition(milestone, bar1Top);
	elements.push_back(backgroundBar1);

	auto backgroundBar2 = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	backgroundBar2->SetSize(fullVolumeBarWidth, heightScaling * BAR_HEIGHT);
	backgroundBar2->Set2DPosition(milestone, bar2Top);
	elements.push_back(backgroundBar2);

	
	// SETTINGS FOREGROUND BARS:
	texture = ResourceManagers::GetInstance()->GetTexture("img_foreground_bar.png");
	musicForegroundBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	musicForegroundBar->SetSize(fullVolumeBarWidth * musicVolume / 128, heightScaling * BAR_HEIGHT);
	musicForegroundBar->Set2DPosition(milestone, bar1Top);
	elements.push_back(musicForegroundBar);

	sfxForegroundBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	sfxForegroundBar->SetSize(fullVolumeBarWidth * sfxVolume / 128, heightScaling * BAR_HEIGHT);
	sfxForegroundBar->Set2DPosition(milestone, bar2Top);
	elements.push_back(sfxForegroundBar);


	// SETTINGS ICONS:
	texture = ResourceManagers::GetInstance()->GetTexture("btn_settings.png");
	iconMusicSettings = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	iconMusicSettings->SetSize(ICON_SIZE * widthScaling, ICON_SIZE * heightScaling);
	iconMusicSettings->Set2DPosition(iconStartPoint + musicVolume / 128.0f * (iconEndPoint - iconStartPoint),
									 bar1Top - (ICON_SIZE - BAR_HEIGHT) / 2.0f * heightScaling);
	elements.push_back(iconMusicSettings);

	texture = ResourceManagers::GetInstance()->GetTexture("btn_settings.png");
	iconSfxSettings = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	iconSfxSettings->SetSize(ICON_SIZE * widthScaling, ICON_SIZE * heightScaling);
	iconSfxSettings->Set2DPosition(iconStartPoint + sfxVolume / 128.0f * (iconEndPoint - iconStartPoint),
								   bar2Top - (ICON_SIZE - BAR_HEIGHT) / 2.0f * heightScaling);
	elements.push_back(iconSfxSettings);


	// button EXIT:
	texture = ResourceManagers::GetInstance()->GetTexture("btn_close.tga");
	auto btnExit = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
	btnExit->SetSize(50, 50);
	btnExit->Set2DPosition(SCREEN_WIDTH - 50, 10);
	btnExit->SetOnClick([this]() {
		Sound::GetInstance()->PlaySfx("book_close_sound.wav");
		GameStateMachine::GetInstance()->PopState();
	});
	m_listButton.push_back(btnExit);


	// button RESET:
	texture = ResourceManagers::GetInstance()->GetTexture("btn_close.tga");
	auto btnReset = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
	btnReset->SetSize(50, 50);
	btnReset->Set2DPosition(SCREEN_WIDTH - 50, 10);
	btnReset->SetOnClick([]() {
		// reset game:

	});
	m_listButton.push_back(btnReset);


	// SOUNDS:
	Sound::GetInstance()->LoadSfx("book_open_sound.wav");
	Sound::GetInstance()->PlaySfx("book_open_sound.wav");

	Sound::GetInstance()->LoadSfx("book_close_sound.wav");
}



void GSSettings::Exit()
{
}


void GSSettings::Pause()
{
}

void GSSettings::Resume()
{
}


void GSSettings::HandleEvents()
{
}

void GSSettings::HandleKeyEvents(SDL_Event& e)
{
}

void GSSettings::HandleTouchEvents(SDL_Event& e)
{
	for (auto button : m_listButton)
	{
		if (button->HandleTouchEvent(&e))
		{
			break;
		}
	}
}

void GSSettings::HandleMouseMoveEvents(SDL_Event& e)
{
}

void GSSettings::Update(float deltaTime)
{

}

void GSSettings::Draw(SDL_Renderer* renderer)
{
	// draw FOREGROUND BARS:
	musicForegroundBar->Draw(renderer);
	sfxForegroundBar->Draw(renderer);

	// draw other UI ELEMENTS:
	for (auto& it : elements) it->Draw(renderer);

	// draw BUTTONS:
	for (auto& it : m_listButton) it->Draw(renderer);
}
