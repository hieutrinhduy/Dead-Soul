#include "GSSettings.h"

#include "DataSaver.h"
#include "ResourceManagers.h"

#include "Sound.h"


constexpr int BACKGROUND_SPRITES_WIDTH = 128, BACKGROUND_SPRITES_HEIGHT = 72;
constexpr int TXT_MUSIC_WIDTH = 23, TXT_MUSIC_HEIGHT = 7;
constexpr int TXT_SFX_WIDTH = 13, TXT_SFX_HEIGHT = 11;
constexpr int BAR_WIDTH = 16, BAR_HEIGHT = 5;
constexpr int ICON_SIZE = 9; // altered compared to reality ( 19x19 px )

constexpr float DECAY_FACTOR = 0.8f;

constexpr float BAR_WIDTH_RATIO = 66.0f / 128, BAR_HEIGHT_RATIO = 3.0f / 72;
constexpr float BAR_POSITION_X_RATIO = 45.0f / 128;
constexpr float MUSIC_BAR_POSITION_Y_RATIO = 18.0f / 72, SFX_BAR_POSITION_Y_RATIO = 28.0f / 72;

constexpr float VOLUME_SETTINGS_ICON_WIDTH_RATIO = 7.0f / 128;
constexpr float VOLUME_SETTINGS_ICON_HEIGHT_RATIO = 7.0f / 72;
constexpr float MOUSE_SETTINGS_ICON_WIDTH_RATIO = 5.0f / 128;
constexpr float MOUSE_SETTINGS_ICON_HEIGHT_RATIO = 5.0f / 72;

constexpr float MOUSE_SETTINGS_ICON_TOP = 50.0f / 72;
constexpr float MOUSE_SETTINGS_ICON_LEFT = 13.0f / 128, MOUSE_SETTINGS_ICON_RIGHT = 110.0f / 128;

constexpr int RESET_BUTTON_WIDTH = 300, RESET_BUTTON_HEIGHT = 100;


GSSettings::GSSettings() : GameStateBase(StateType::STATE_SETTINGS)
{
	// GET SAVE DATA:
	auto saveData = DataSaver::GetInstance()->LoadGame("save.dat");
	// volumn in the range of 0 to 128
	musicVolume = saveData.musicVolume;
	sfxVolume = saveData.sfxVolume;
	// mousewheel sensitivity:
	mousewheelSensitivity = saveData.mousewheelSensitivity;

	fullVolumeBarWidth = BAR_WIDTH_RATIO * SCREEN_WIDTH;
	iconStartPoint = (BAR_POSITION_X_RATIO - 4.0f / 128) * SCREEN_WIDTH;
	iconEndPoint = iconStartPoint + fullVolumeBarWidth + SCREEN_WIDTH / 128.0f;
}


GSSettings::~GSSettings()
{
}


void GSSettings::Init()
{
	// SETTINGS BACKGROUND:
	auto texture = ResourceManagers::GetInstance()->GetTexture("GSSettings_UI\\bg_settings.png");
	auto sceneBackground = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	sceneBackground->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	sceneBackground->Set2DPosition(0, 0);
	elements.push_back(sceneBackground);


	// VOLUME BARS:
	texture = ResourceManagers::GetInstance()->GetTexture("GSSettings_UI\\settings_bar.png");
	musicForegroundBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	musicForegroundBar->SetSize(fullVolumeBarWidth, BAR_HEIGHT_RATIO * SCREEN_HEIDHT);
	musicForegroundBar->Set2DPosition(BAR_POSITION_X_RATIO * SCREEN_WIDTH, MUSIC_BAR_POSITION_Y_RATIO * SCREEN_HEIDHT);
	elements.push_back(musicForegroundBar);

	sfxForegroundBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	sfxForegroundBar->SetSize(fullVolumeBarWidth, musicForegroundBar->GetHeight());
	sfxForegroundBar->Set2DPosition(BAR_POSITION_X_RATIO * SCREEN_WIDTH, SFX_BAR_POSITION_Y_RATIO * SCREEN_HEIDHT);
	elements.push_back(sfxForegroundBar);


	// SETTINGS ICONS:
	texture = ResourceManagers::GetInstance()->GetTexture("btn_settings.png");
	iconMusicSettings = std::make_shared<SliderIcon>(texture, SDL_FLIP_NONE);
	iconMusicSettings->SetSize(VOLUME_SETTINGS_ICON_WIDTH_RATIO * SCREEN_WIDTH, VOLUME_SETTINGS_ICON_HEIGHT_RATIO * SCREEN_HEIDHT);
	iconMusicSettings->Set2DPosition(iconStartPoint + musicVolume / 128.0f * fullVolumeBarWidth,
									 (MUSIC_BAR_POSITION_Y_RATIO - 2.0f / 72) * SCREEN_HEIDHT  );
	iconMusicSettings->SetRange(iconStartPoint, iconEndPoint);
	m_listSliderIcon.push_back(iconMusicSettings);

	iconSfxSettings = std::make_shared<SliderIcon>(texture, SDL_FLIP_NONE);
	iconSfxSettings->SetSize(VOLUME_SETTINGS_ICON_WIDTH_RATIO * SCREEN_WIDTH, VOLUME_SETTINGS_ICON_HEIGHT_RATIO * SCREEN_HEIDHT);
	iconSfxSettings->Set2DPosition(iconStartPoint + sfxVolume / 128.0f * fullVolumeBarWidth,
								   (SFX_BAR_POSITION_Y_RATIO - 2.0f / 72) * SCREEN_HEIDHT  );
	iconSfxSettings->SetRange(iconStartPoint, iconEndPoint);
	m_listSliderIcon.push_back(iconSfxSettings);

	iconMouseSettings = std::make_shared<SliderIcon>(texture, SDL_FLIP_NONE);
	iconMouseSettings->SetSize(MOUSE_SETTINGS_ICON_WIDTH_RATIO * SCREEN_WIDTH, MOUSE_SETTINGS_ICON_HEIGHT_RATIO * SCREEN_HEIDHT);
	iconMouseSettings->Set2DPosition((MOUSE_SETTINGS_ICON_LEFT + mousewheelSensitivity * (MOUSE_SETTINGS_ICON_RIGHT - MOUSE_SETTINGS_ICON_LEFT) / 4) * SCREEN_WIDTH,
									 MOUSE_SETTINGS_ICON_TOP * SCREEN_HEIDHT);
	iconMouseSettings->SetRange(MOUSE_SETTINGS_ICON_LEFT * SCREEN_WIDTH, MOUSE_SETTINGS_ICON_RIGHT * SCREEN_WIDTH);
	m_listSliderIcon.push_back(iconMouseSettings);


	// button EXIT:
	texture = ResourceManagers::GetInstance()->GetTexture("GSSettings_UI\\btn_close_settings.png");
	auto btnExit = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
	btnExit->SetSize(RESET_BUTTON_HEIGHT * 2, RESET_BUTTON_HEIGHT);
	btnExit->Set2DPosition(SCREEN_WIDTH - RESET_BUTTON_HEIGHT * 2 - 50, SCREEN_HEIDHT - RESET_BUTTON_HEIGHT - 30);
	btnExit->SetOnClick([this]() {
		Sound::GetInstance()->PlaySfx("book_close_sound.wav");
		GameStateMachine::GetInstance()->PopState();
	});
	m_listButton.push_back(btnExit);


	// button RESET:
	texture = ResourceManagers::GetInstance()->GetTexture("GSSettings_UI\\btn_reset.png");
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
	
	if (e.type == SDL_MOUSEBUTTONUP) releaseMouseButton = true;
}



void GSSettings::HandleMouseMoveEvents(SDL_Event& e)
{
}




void GSSettings::Update(float deltaTime)
{
	musicVolume = 128.0f * (iconMusicSettings->Get2DPosition().x - iconStartPoint) / fullVolumeBarWidth;
	sfxVolume = 128.0f * (iconSfxSettings->Get2DPosition().x - iconStartPoint) / fullVolumeBarWidth;
	mousewheelSensitivity = std::round((iconMouseSettings->Get2DPosition().x - MOUSE_SETTINGS_ICON_LEFT * SCREEN_WIDTH) /
									   ((MOUSE_SETTINGS_ICON_RIGHT - MOUSE_SETTINGS_ICON_LEFT) * SCREEN_WIDTH) * 4);

	Sound::GetInstance()->SetMusicVolume(musicVolume);
	Sound::GetInstance()->SetSfxVolume(sfxVolume);

	SaveData saveData = DataSaver::GetInstance()->LoadGame("save.dat");
	saveData.musicVolume = musicVolume;
	saveData.sfxVolume = sfxVolume;
	saveData.mousewheelSensitivity = mousewheelSensitivity;
	DataSaver::GetInstance()->SetSaveData(saveData);
	DataSaver::GetInstance()->SaveGame("save.dat");

	musicForegroundBar->SetSize(fullVolumeBarWidth * musicVolume / 128, musicForegroundBar->GetHeight());
	sfxForegroundBar->SetSize(fullVolumeBarWidth * sfxVolume / 128, musicForegroundBar->GetHeight());
	if (releaseMouseButton) {
		releaseMouseButton = false;
		iconMouseSettings->Set2DPosition((MOUSE_SETTINGS_ICON_LEFT + mousewheelSensitivity * (MOUSE_SETTINGS_ICON_RIGHT - MOUSE_SETTINGS_ICON_LEFT) / 4) * SCREEN_WIDTH,
										  iconMouseSettings->Get2DPosition().y);
	}
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
