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

constexpr int RESET_BUTTON_WIDTH = 300, RESET_BUTTON_HEIGHT = 100;


GSSettings::GSSettings() : GameStateBase(StateType::STATE_SETTINGS)
{
	widthScaling = SCREEN_WIDTH * DECAY_FACTOR / BACKGROUND_SPRITES_WIDTH ;
	heightScaling = SCREEN_HEIDHT * DECAY_FACTOR / BACKGROUND_SPRITES_HEIGHT;

	milestone = SCREEN_WIDTH * 1 / 4;
	bar1Top = SCREEN_HEIDHT * 1 / 4;
	bar2Top = SCREEN_HEIDHT * 1 / 2;

	// volumn in the range of 0 to 128, read from 'save.dat'
	musicVolume = DataSaver::GetInstance()->LoadGame("save.dat").musicVolume;
	sfxVolume = DataSaver::GetInstance()->LoadGame("save.dat").sfxVolume;

	int fullBarWidth = SCREEN_WIDTH * 3 / 4 - milestone;
	float ratio = (float) fullBarWidth / BAR_WIDTH;
	fullVolumeBarWidth = fullBarWidth - 2 * ratio;

	iconStartPoint = milestone + ratio - ICON_SIZE * widthScaling / 2;
	iconEndPoint = milestone + fullBarWidth - ratio - ICON_SIZE * widthScaling / 2;
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
	backgroundBar1->SetSize(SCREEN_WIDTH * 3 / 4 - milestone, heightScaling * BAR_HEIGHT);
	backgroundBar1->Set2DPosition(milestone, bar1Top);
	elements.push_back(backgroundBar1);

	auto backgroundBar2 = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	backgroundBar2->SetSize(SCREEN_WIDTH * 3 / 4 - milestone, heightScaling * BAR_HEIGHT);
	backgroundBar2->Set2DPosition(milestone, bar2Top);
	elements.push_back(backgroundBar2);

	
	// SETTINGS FOREGROUND BARS:
	texture = ResourceManagers::GetInstance()->GetTexture("img_foreground_bar.png");
	musicForegroundBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	musicForegroundBar->SetSize(fullVolumeBarWidth * musicVolume / 128, heightScaling * BAR_HEIGHT);
	musicForegroundBar->Set2DPosition(iconStartPoint + ICON_SIZE * widthScaling / 2, bar1Top);
	elements.push_back(musicForegroundBar);

	sfxForegroundBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	sfxForegroundBar->SetSize(fullVolumeBarWidth * sfxVolume / 128, heightScaling * BAR_HEIGHT);
	sfxForegroundBar->Set2DPosition(iconStartPoint + ICON_SIZE * widthScaling / 2, bar2Top);
	elements.push_back(sfxForegroundBar);


	// SETTINGS ICONS:
	texture = ResourceManagers::GetInstance()->GetTexture("btn_settings.png");
	iconMusicSettings = std::make_shared<SliderIcon>(texture, SDL_FLIP_NONE);
	iconMusicSettings->SetSize(ICON_SIZE * widthScaling, ICON_SIZE * heightScaling);
	iconMusicSettings->Set2DPosition(iconStartPoint + musicVolume / 128.0f * fullVolumeBarWidth,
									 bar1Top - (ICON_SIZE - BAR_HEIGHT) / 2.0f * heightScaling);
	iconMusicSettings->SetRange(iconStartPoint, iconEndPoint);
	m_listSliderIcon.push_back(iconMusicSettings);

	texture = ResourceManagers::GetInstance()->GetTexture("btn_settings.png");
	iconSfxSettings = std::make_shared<SliderIcon>(texture, SDL_FLIP_NONE);
	iconSfxSettings->SetSize(ICON_SIZE * widthScaling, ICON_SIZE * heightScaling);
	iconSfxSettings->Set2DPosition(iconStartPoint + sfxVolume / 128.0f * fullVolumeBarWidth,
								   bar2Top - (ICON_SIZE - BAR_HEIGHT) / 2.0f * heightScaling);
	iconSfxSettings->SetRange(iconStartPoint, iconEndPoint);
	m_listSliderIcon.push_back(iconSfxSettings);


	// button EXIT:
	texture = ResourceManagers::GetInstance()->GetTexture("btn_close_settings.png");
	auto btnExit = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
	btnExit->SetSize(RESET_BUTTON_HEIGHT * 2, RESET_BUTTON_HEIGHT);
	btnExit->Set2DPosition(SCREEN_WIDTH - RESET_BUTTON_HEIGHT * 2 - 50, SCREEN_HEIDHT - RESET_BUTTON_HEIGHT - 30);
	btnExit->SetOnClick([this]() {
		Sound::GetInstance()->PlaySfx("book_close_sound.wav");
		GameStateMachine::GetInstance()->PopState();
	});
	m_listButton.push_back(btnExit);


	// button RESET:
	texture = ResourceManagers::GetInstance()->GetTexture("btn_reset.png");
	auto btnReset = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
	btnReset->SetSize(RESET_BUTTON_WIDTH, RESET_BUTTON_HEIGHT);
	btnReset->Set2DPosition((SCREEN_WIDTH - RESET_BUTTON_WIDTH) / 2, SCREEN_HEIDHT - RESET_BUTTON_HEIGHT - 30);
	void ResetGame();
	btnReset->SetOnClick([]() {
		ResetGame();
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

	for (auto icon : m_listSliderIcon)
	{
		if (icon->HandleTouchEvent(&e))
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
	musicVolume = 128.0f * (iconMusicSettings->Get2DPosition().x - iconStartPoint) / fullVolumeBarWidth;
	sfxVolume = 128.0f * (iconSfxSettings->Get2DPosition().x - iconStartPoint) / fullVolumeBarWidth;

	Sound::GetInstance()->SetMusicVolume(musicVolume);
	Sound::GetInstance()->SetSfxVolume(sfxVolume);

	SaveData saveData = DataSaver::GetInstance()->LoadGame("save.dat");
	saveData.musicVolume = musicVolume;
	saveData.sfxVolume = sfxVolume;
	DataSaver::GetInstance()->SetSaveData(saveData);
	DataSaver::GetInstance()->SaveGame("save.dat");

	musicForegroundBar->SetSize(fullVolumeBarWidth * musicVolume / 128, heightScaling * BAR_HEIGHT);
	sfxForegroundBar->SetSize(fullVolumeBarWidth * sfxVolume / 128, heightScaling * BAR_HEIGHT);
}




void GSSettings::Draw(SDL_Renderer* renderer)
{
	// draw FOREGROUND BARS:
	musicForegroundBar->Draw(renderer);
	sfxForegroundBar->Draw(renderer);

	// draw non-functional UI ELEMENTS:
	for (auto& it : elements) it->Draw(renderer);

	// draw BUTTONS:
	for (auto& it : m_listButton) it->Draw(renderer);

	// draw SLIDER ICONS:
	for (auto& it : m_listSliderIcon) it->Draw(renderer);
}



void ResetGame() {
	SaveData saveData;
	saveData.musicVolume = DataSaver::GetInstance()->LoadGame("save.dat").musicVolume;
	saveData.sfxVolume = DataSaver::GetInstance()->LoadGame("save.dat").sfxVolume;
	DataSaver::GetInstance()->SetSaveData(saveData);
	DataSaver::GetInstance()->SaveGame("save.dat");

	Sound::GetInstance()->ClearAudioResource();

	GameStateMachine::GetInstance()->Cleanup();
	GameStateMachine::GetInstance()->PushState(StateType::STATE_LOGO);
}
