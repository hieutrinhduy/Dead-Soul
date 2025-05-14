/*
	m_timeScale == -2 => LOADING SCENE
	m_timeScale == -1 => CHANGE LEVEL
	m_timeScale == 0 => PAUSE GAME
	m_timeScale == 1 => RESUME GAME
	m_timeScale == 2 => LEVEL UP
*/
// ----------------------------------------------------------------------
/*
	upgrade 1: increase 25% of max HP, then refill 10% of max HP
	upgrade 2: increase 10% of max MP, and increase refill speed
	upgrade 3: increase 4 - 6 ATK dmg
*/
// ----------------------------------------------------------------------

#include "UIManager.h"

#include <thread>
#include <mutex>
#include <cmath>

#include "DataSaver.h"
#include "ResourceManagers.h"

#include "Camera.h"
#include "Renderer.h"
#include "Define.h"
#include "Sound.h"

#include "GameStateMachine.h"


std::mutex mtx;

#pragma region Constant expression evaluation
constexpr int HP_BACKGROUND_BAR_WIDTH_IN_SPRITE = 512;
constexpr int BACKGROUND_BAR_HEIGHT = 16;
constexpr int BAR_LEFT = 10;
constexpr int BAR_GAP = 8;

constexpr int LOADING_TEXT_WIDTH = 300, LOADING_TEXT_HEIGHT = 48;

constexpr int STAT_TEXT_WIDTH = 100;
constexpr int HP_BACKGROUND_BAR_POSITION_X = BAR_LEFT, HP_BACKGROUND_BAR_POSITION_Y = 4;
constexpr int SP_BACKGROUND_BAR_POSITION_X = BAR_LEFT,
			  SP_BACKGROUND_BAR_POSITION_Y = HP_BACKGROUND_BAR_POSITION_Y + BACKGROUND_BAR_HEIGHT + BAR_GAP;
constexpr int MP_BACKGROUND_BAR_POSITION_X = BAR_LEFT,
			  MP_BACKGROUND_BAR_POSITION_Y = SP_BACKGROUND_BAR_POSITION_Y + BACKGROUND_BAR_HEIGHT + BAR_GAP;
constexpr int EXP_BAR_HEIGHT = 20;

constexpr int FRAGMENT_COUNT = 20;

constexpr int SLOT_SIZE = 75, SLOT_GAP = 5;
constexpr int SLOT_0_POSITION_X = 10;
constexpr int SLOT_1_POSITION_X = SLOT_0_POSITION_X + SLOT_SIZE + SLOT_GAP;
constexpr int SLOT_Y = EXP_BAR_HEIGHT + SLOT_SIZE + 5;

constexpr float SETTINGS_BAR_WIDTH_RATIO = 114.0f / 256, SETTINGS_BAR_HEIGHT_RATIO = 3.0f / 144;
constexpr float SETTINGS_BAR_POSITION_X_RATIO = 87.0f / 256;
constexpr float SETTINGS_BAR_1_POSITION_Y_RATIO = 45.0f / 144, SETTINGS_BAR_2_POSITION_Y_RATIO = 69.0f / 144;

constexpr float SETTINGS_ICON_WIDTH_RATIO = 9.0f / 256, SETTINGS_ICON_HEIGHT_RATIO = 9.0f / 144;
constexpr float SETTINGS_SOUND_START_X = 82.0f / 256, SETTINGS_SOUND_END_X = 197.0f / 256;
constexpr float SETTINGS_SENSITIVITY_START_X = 51.0f / 256, SETTINGS_SENSITIVITY_END_X = 196.0f / 256;
constexpr float SETTINGS_ICON_1_Y = 42.0f / 144, SETTINGS_ICON_2_Y = 66.0f /144, SETTINGS_ICON_3_Y = 97.0f / 144;

constexpr float RESUME_BUTTON_LEFT_RATIO = 51.0f / 256, RESUME_BUTTON_RIGHT_RATIO = 97.0f / 256;
constexpr float QUIT_BUTTON_LEFT_RATIO = 159.0f / 256, QUIT_BUTTON_RIGHT_RATIO = 205.0f / 256;
constexpr float PAUSE_BUTTON_TOP_RATIO = 117.0f / 144, PAUSE_BUTTON_BOTTOM_RATIO = 131.0f / 144;

// statistic about: selection border, hover border, underline, confirm button
constexpr float SELECTION_WIDTH_RATIO = 62.0f / 256, SELECTION_HEIGHT_RATIO = 83.0f / 144;
constexpr float SELECTION_TOP_RATIO = 27.0f / 144, SELECTION_BOTTOM_RATIO = 110.0f / 144;
constexpr float SELECTION_1_LEFT_RATIO = 32.0f / 256, SELECTION_1_RIGHT_RATIO = 94.0f / 256;
constexpr float SELECTION_2_LEFT_RATIO = 97.0f / 256, SELECTION_2_RIGHT_RATIO = 159.0f / 256;
constexpr float SELECTION_3_LEFT_RATIO = 162.0f / 256, SELECTION_3_RIGHT_RATIO = 224.0f / 256;
constexpr float UP_BUTTON_TOP_RATIO = 116.0f / 144, UP_BUTTON_BOTTOM_RATIO = 130.0f / 144;
constexpr float UP_BUTTON_LEFT_RATIO = 138.0f / 256, UP_BUTTON_RIGHT_RATIO = 160.0f / 256;

#pragma endregion


#pragma region Constructor && Deconstructor
UIManager::UIManager(std::shared_ptr<int> timeScale) :
	m_renderer(Renderer::GetInstance()->GetRenderer()), m_timeScale(timeScale),
	hpTextColor({ 0, 0, 0 }), expTextColor({ 255,255,255 }),
	timer(0), spTimer(0), mpTimer(0),
	changeInWheelValue(0)
{
	m_isPlaying = true; m_finishThread = false;
	TTF_Font* font = ResourceManagers::GetInstance()->GetFont("FVF Fernando 08.ttf", 48);

	#pragma region UI WHEN LOAD SCENE
	// LOADING BACKGROUND:
	loadingBackground = std::make_shared<Sprite2D>(ResourceManagers::GetInstance()->GetTexture("bg_loading.png"), SDL_FLIP_NONE);
	loadingBackground->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	loadingBackground->Set2DPosition(0, 0);
	loadingBackground->Draw(m_renderer);

	// LOADING TEXT:
	txtLoading = std::make_shared<Text>("LOADING", font, hpTextColor);
	txtLoading->SetSize(LOADING_TEXT_WIDTH, LOADING_TEXT_HEIGHT);
	txtLoading->Set2DPosition( (SCREEN_WIDTH - LOADING_TEXT_WIDTH) / 2.0f, SCREEN_HEIDHT - LOADING_TEXT_HEIGHT - 75 );
	#pragma endregion

	m_inventorySlot = 0;
	

	// get data from save file:
	SaveData saveData = DataSaver::GetInstance()->LoadGame("save.dat");
	float musicVolume = saveData.musicVolume / 128.0f, sfxVolume = saveData.sfxVolume / 128.0f;
	mousewheelSensitivity = saveData.mousewheelSensitivity;
	hp = 100; sp = 100; mp = 100; exp = 100;
	maxHP = 100; maxSP = 100; maxMP = 100; maxEXP = 100;


	hpStat = "HP: " + std::to_string((int)hp) + "/" + std::to_string((int)maxHP);
	spStat = "SP: " + std::to_string((int)sp) + "/" + std::to_string((int)maxSP);
	mpStat = "MP: " + std::to_string((int)mp) + "/" + std::to_string((int)maxMP);
	expStat = "EXP: " + std::to_string((int)exp) + "/" + std::to_string((int)maxEXP);

	hpLoss = 0; spLoss = 0; mpLoss = 0; expGained = 0;
	hpChangePerFrame = 0; spChangePerFrame = 0; mpChangePerFrame = 0; expChangePerFrame = 0;


	fullHpBarWidth = SCREEN_WIDTH / 3.0f;
	float widthScaling = fullHpBarWidth / HP_BACKGROUND_BAR_WIDTH_IN_SPRITE;
	fullSpBarWidth = fullHpBarWidth * 5 / 6;
	fullMpBarWidth = fullHpBarWidth / 2;


	#pragma region UI WHEN RUN GAME
	// HP BAR:
	auto texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\hp_background_bar.png");
	hpBgBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	hpBgBar->Set2DPosition(HP_BACKGROUND_BAR_POSITION_X, HP_BACKGROUND_BAR_POSITION_Y);
	hpBgBar->SetSize(fullHpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiWhenRunGame.push_back(hpBgBar);

	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\hp_bar.png");
	hpBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	hpBar->Set2DPosition(HP_BACKGROUND_BAR_POSITION_X + widthScaling, HP_BACKGROUND_BAR_POSITION_Y);
	hpBar->SetSize(fullHpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiWhenRunGame.push_back(hpBar);

	txtHP = std::make_shared<Text>(hpStat, font, hpTextColor);
	txtHP->SetSize(STAT_TEXT_WIDTH, BACKGROUND_BAR_HEIGHT);
	txtHP->Set2DPosition( (fullHpBarWidth - txtHP->GetWidth()) / 2, HP_BACKGROUND_BAR_POSITION_Y - 2 );
	uiWhenRunGame.push_back(txtHP);

	
	// SP BAR:
	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\sp_background_bar.png");
	spBgBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	spBgBar->Set2DPosition(SP_BACKGROUND_BAR_POSITION_X, SP_BACKGROUND_BAR_POSITION_Y);
	spBgBar->SetSize(fullSpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiWhenRunGame.push_back(spBgBar);

	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\sp_bar.png");
	spBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	spBar->Set2DPosition(SP_BACKGROUND_BAR_POSITION_X + widthScaling, SP_BACKGROUND_BAR_POSITION_Y);
	spBar->SetSize(fullSpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiWhenRunGame.push_back(spBar);

	txtSP = std::make_shared<Text>(spStat, font, hpTextColor);
	txtSP->SetSize(STAT_TEXT_WIDTH, BACKGROUND_BAR_HEIGHT);
	txtSP->Set2DPosition( (fullSpBarWidth - txtSP->GetWidth()) / 2, SP_BACKGROUND_BAR_POSITION_Y - 2 );
	uiWhenRunGame.push_back(txtSP);


	// MP BAR:
	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\mp_background_bar.png");
	mpBgBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	mpBgBar->Set2DPosition(MP_BACKGROUND_BAR_POSITION_X, MP_BACKGROUND_BAR_POSITION_Y);
	mpBgBar->SetSize(fullMpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiWhenRunGame.push_back(mpBgBar);

	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\mp_bar.png");
	mpBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	mpBar->Set2DPosition(MP_BACKGROUND_BAR_POSITION_X + widthScaling, MP_BACKGROUND_BAR_POSITION_Y);
	mpBar->SetSize(fullMpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiWhenRunGame.push_back(mpBar);

	txtMP = std::make_shared<Text>(mpStat, font, hpTextColor);
	txtMP->SetSize(STAT_TEXT_WIDTH, BACKGROUND_BAR_HEIGHT);
	txtMP->Set2DPosition( (fullMpBarWidth - txtMP->GetWidth()) / 2, MP_BACKGROUND_BAR_POSITION_Y - 2 );
	uiWhenRunGame.push_back(txtMP);


	// EXP BAR:
	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\exp_background_bar.png");
	expBgBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	expBgBar->Set2DPosition(0, SCREEN_HEIDHT - 20);
	expBgBar->SetSize(SCREEN_WIDTH, 20);
	uiWhenRunGame.push_back(expBgBar);

	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\exp_bar.png");
	expBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	expBar->Set2DPosition(0, SCREEN_HEIDHT - 20);
	expBar->SetSize(SCREEN_WIDTH, EXP_BAR_HEIGHT);
	uiWhenRunGame.push_back(expBar);

	txtEXP = std::make_shared<Text>(expStat, font, expTextColor);
	txtEXP->SetSize(STAT_TEXT_WIDTH + 5, expBar->GetHeight());
	txtEXP->Set2DPosition( (SCREEN_WIDTH - txtEXP->GetWidth()) / 2.0f, expBar->Get2DPosition().y - 2 );
	uiWhenRunGame.push_back(txtEXP);


	// INVENTORY:
	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\slot_greatsword.png");
	auto slot = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
	slot->SetSize(SLOT_SIZE, SLOT_SIZE);
	slot->Set2DPosition(SLOT_0_POSITION_X, SCREEN_HEIDHT - SLOT_Y);
	slot->SetOnClick([this]() {
		m_playerController->SetWeaponIndex(0);
	});
	buttons.push_back(slot);

	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\slot_bow.png");
	slot = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
	slot->SetSize(SLOT_SIZE, SLOT_SIZE);
	slot->Set2DPosition(SLOT_1_POSITION_X, SCREEN_HEIDHT - SLOT_Y);
	slot->SetOnClick([this]() {
		m_playerController->SetWeaponIndex(1);
		});
	buttons.push_back(slot);

	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\slot_selected.png");
	selectedSlot = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	selectedSlot->SetSize(SLOT_SIZE, SLOT_SIZE);
	selectedSlot->Set2DPosition(SLOT_0_POSITION_X, SCREEN_HEIDHT - SLOT_Y);
	uiWhenRunGame.push_back(selectedSlot);


	// PAUSE BUTTON:
	btnPause = std::make_shared<MouseButton>(ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\btn_pause.png"), SDL_FLIP_NONE);
	btnPause->SetSize(50, 50);
	btnPause->Set2DPosition((SCREEN_WIDTH - 50) / 2.0f, HP_BACKGROUND_BAR_POSITION_Y);
	btnPause->SetOnClick([this]() {
		*m_timeScale = 0;
	});
	#pragma endregion

	#pragma	region UI WHEN PAUSE GAME
	pressButtonWhenPause = false;
	releaseButtonWhenPause = false;

	// PAUSE BACKGROUND:
	auto background = std::make_shared<Sprite2D>(ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\pause_game.png"), SDL_FLIP_NONE);
	background->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	background->Set2DPosition(0, 0);
	uiWhenPause.push_back(background);


	// SETTINGS FOREGROUND_BAR:
	musicForegroundBar = std::make_shared<Sprite2D>(ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\settings_bar.png"), SDL_FLIP_NONE);
	musicForegroundBar->SetSize(SCREEN_WIDTH * SETTINGS_BAR_WIDTH_RATIO * musicVolume, SCREEN_HEIDHT * SETTINGS_BAR_HEIGHT_RATIO);
	musicForegroundBar->Set2DPosition(SCREEN_WIDTH * SETTINGS_BAR_POSITION_X_RATIO, SCREEN_HEIDHT * SETTINGS_BAR_1_POSITION_Y_RATIO);
	uiWhenPause.push_back(musicForegroundBar);

	sfxForegroundBar = std::make_shared<Sprite2D>(ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\settings_bar.png"), SDL_FLIP_NONE);
	sfxForegroundBar->SetSize(SCREEN_WIDTH * SETTINGS_BAR_WIDTH_RATIO * sfxVolume, SCREEN_HEIDHT * SETTINGS_BAR_HEIGHT_RATIO);
	sfxForegroundBar->Set2DPosition(SCREEN_WIDTH * SETTINGS_BAR_POSITION_X_RATIO, SCREEN_HEIDHT* SETTINGS_BAR_2_POSITION_Y_RATIO);
	uiWhenPause.push_back(sfxForegroundBar);


	// SETTINGS ICON:
	iconMusicSettings = std::make_shared<SliderIcon>(ResourceManagers::GetInstance()->GetTexture("btn_settings.png"), SDL_FLIP_NONE);
	iconMusicSettings->SetSize(SCREEN_WIDTH* SETTINGS_ICON_WIDTH_RATIO, SCREEN_HEIDHT* SETTINGS_ICON_HEIGHT_RATIO);
	iconMusicSettings->Set2DPosition(SCREEN_WIDTH * (SETTINGS_SOUND_START_X + musicVolume * (SETTINGS_SOUND_END_X - SETTINGS_SOUND_START_X)),
									 SCREEN_HEIDHT * SETTINGS_ICON_1_Y);
	iconMusicSettings->SetRange(SCREEN_WIDTH * SETTINGS_SOUND_START_X, SCREEN_WIDTH * SETTINGS_SOUND_END_X);
	uiWhenPause.push_back(iconMusicSettings);
	sliderIcons.push_back(iconMusicSettings);

	iconSfxSettings = std::make_shared<SliderIcon>(ResourceManagers::GetInstance()->GetTexture("btn_settings.png"), SDL_FLIP_NONE);
	iconSfxSettings->SetSize(SCREEN_WIDTH* SETTINGS_ICON_WIDTH_RATIO, SCREEN_HEIDHT* SETTINGS_ICON_HEIGHT_RATIO);
	iconSfxSettings->Set2DPosition(SCREEN_WIDTH* (SETTINGS_SOUND_START_X + sfxVolume * (SETTINGS_SOUND_END_X - SETTINGS_SOUND_START_X)),
													SCREEN_HEIDHT* SETTINGS_ICON_2_Y);
	iconSfxSettings->SetRange(SCREEN_WIDTH* SETTINGS_SOUND_START_X, SCREEN_WIDTH* SETTINGS_SOUND_END_X);
	uiWhenPause.push_back(iconSfxSettings);
	sliderIcons.push_back(iconSfxSettings);

	iconSensitivitySettings = std::make_shared<SliderIcon>(ResourceManagers::GetInstance()->GetTexture("btn_settings.png"), SDL_FLIP_NONE);
	iconSensitivitySettings->SetSize(SCREEN_WIDTH * SETTINGS_ICON_WIDTH_RATIO, SCREEN_HEIDHT * SETTINGS_ICON_HEIGHT_RATIO);
	iconSensitivitySettings->Set2DPosition(SCREEN_WIDTH * (SETTINGS_SENSITIVITY_START_X + (9 - mousewheelSensitivity) * (SETTINGS_SENSITIVITY_END_X - SETTINGS_SENSITIVITY_START_X) / 8),
											SCREEN_HEIDHT* SETTINGS_ICON_3_Y);
	iconSensitivitySettings->SetRange(SCREEN_WIDTH * SETTINGS_SENSITIVITY_START_X, SCREEN_WIDTH* SETTINGS_SENSITIVITY_END_X);
	uiWhenPause.push_back(iconSensitivitySettings);
	sliderIcons.push_back(iconSensitivitySettings);
	#pragma endregion

	#pragma region UI WHEN LEVEL UP
	// BACKGROUND:
	background = std::make_shared<Sprite2D>(ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\level_up.png"), SDL_FLIP_NONE);
	background->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	background->Set2DPosition(0, 0);
	uiWhenLevelUp.push_back(background);

	// HOVERING SELECTION:
	hoverBorder = std::make_shared<Sprite2D>(ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\hover_border.png"), SDL_FLIP_NONE);
	hoverBorder->SetSize((SELECTION_2_RIGHT_RATIO - SELECTION_2_LEFT_RATIO)* SCREEN_WIDTH,
						 (SELECTION_BOTTOM_RATIO - SELECTION_TOP_RATIO)* SCREEN_HEIDHT);
	hoverBorder->Set2DPosition(SCREEN_WIDTH, 0);
	uiWhenLevelUp.push_back(hoverBorder);

	// SELECTED SELECTION:
	selectionBorder = std::make_shared<Sprite2D>(ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\selected_border.png"), SDL_FLIP_NONE);
	selectionBorder->SetSize(hoverBorder->GetWidth(), hoverBorder->GetHeight());
	selectionBorder->Set2DPosition(SCREEN_WIDTH, 0);
	uiWhenLevelUp.push_back(selectionBorder);

	// UNDERLINE:
	underline = std::make_shared<Sprite2D>(ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\underline.png"), SDL_FLIP_NONE);
	underline->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	underline->Set2DPosition(SCREEN_WIDTH, 0);
	uiWhenLevelUp.push_back(underline);

	// CONFIRM BUTTON:
	confirmButton = std::make_shared<Sprite2D>(ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\confirm_level_up.png"), SDL_FLIP_NONE);
	confirmButton->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	confirmButton->Set2DPosition(SCREEN_WIDTH, 0);
	uiWhenLevelUp.push_back(confirmButton);

	#pragma endregion

	#pragma region UI WHEN CHANGE MAP
	animationMapTransition = std::make_shared<SpriteAnimation>(ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\map_transition_1.png"), 1, 9, 6, 0.02f);
	animationMapTransition->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	animationMapTransition->Set2DPosition(0, 0);
	#pragma endregion
}


UIManager::~UIManager() {}
#pragma endregion


#pragma region Handle events
void UIManager::HandleTouchEvents(SDL_Event& e)
{
	// when run game:
	if (*m_timeScale == 1) {
		for (auto button : buttons)
		{
			if (button->HandleTouchEvent(&e))
			{
				break;
			}
		}
		
		btnPause->HandleTouchEvent(&e);

		return;
	}



	// when pause game:
	if (*m_timeScale == 0) {
		for (auto icon : sliderIcons)
		{
			if (icon->HandleTouchEvent(&e))
			{
				break;
			}
		}

		if (e.type == SDL_MOUSEBUTTONDOWN) {
			pressButtonWhenPause = true;
		}
		else if (e.type == SDL_MOUSEBUTTONUP) {
			releaseButtonWhenPause = true;
		}
	}

	
	// when level up:
	if (*m_timeScale == 2) {
		if (e.type == SDL_MOUSEBUTTONDOWN) {
			pressButtonWhenLevelUp = true;
		}
	}
}



void UIManager::HandleMouseEvents(SDL_Event& e) {
	if (*m_timeScale == 1) {
		if (e.type == SDL_MOUSEWHEEL) {
			std::lock_guard<std::mutex> lock(mtx);
			if (timer <= 0) timer = 300;
			changeInWheelValue += e.wheel.y;
		}
	}
}
#pragma endregion


void UIManager::Update(float deltaTime) {
	if (*m_timeScale >= 0) {
		// UPDATE HP:
		if (!FloatEquals(hpChangePerFrame, 0.0f) && !FloatEquals(hpLoss, 0.0f)) {
			hp -= hpChangePerFrame;
			if (hp < 0) hp = 0;
			else if (hp > maxHP) hp = maxHP;
			hpLoss -= hpChangePerFrame;

			// set the text displaying the HP amount:
			hpStat = "HP: " + std::to_string((int)hp) + "/" + std::to_string((int)maxHP);
			txtHP->SetText(hpStat);
			// set the HP bar
			hpBar->SetSize(hp / maxHP * fullHpBarWidth, hpBar->GetHeight());
			
			// get rid of accumulated error
			if (FloatEquals(hpLoss, 0.0f)) hpLoss = 0;
		}


		// UPDATE SP:
		if (!FloatEquals(spChangePerFrame, 0.0f) && !FloatEquals(spLoss, 0.0f)) {
			sp -= spChangePerFrame;
			spLoss -= spChangePerFrame;

			// get rid of accumulated error
			if (FloatEquals(spLoss, 0.0f)) spLoss = 0;
		}
		// set the text displaying the SP amount:
		spStat = "SP: " + std::to_string((int)sp) + "/" + std::to_string((int)maxSP);
		txtSP->SetText(spStat);
		// set the SP bar
		spBar->SetSize(sp / maxSP * fullSpBarWidth, spBar->GetHeight());


		// UPDATE MP:
		if (!FloatEquals(mpChangePerFrame, 0.0f) && !FloatEquals(mpLoss, 0.0f)) {
			mp -= mpChangePerFrame;
			mpLoss -= mpChangePerFrame;

			// get rid of accumulated error
			if (FloatEquals(mpLoss, 0.0f)) mpLoss = 0;
		}
		// set the text displaying the MP amount:
		mpStat = "MP: " + std::to_string((int)mp) + "/" + std::to_string((int)maxMP);
		txtMP->SetText(mpStat);
		// set the MP bar
		mpBar->SetSize(mp / maxMP * fullMpBarWidth, mpBar->GetHeight());
	}

	if (*m_timeScale == -1) {
		animationMapTransition->Set2DPosition(Camera::GetInstance()->GetPosition().x, Camera::GetInstance()->GetPosition().y);
		m_playerController->ResetPlayerPosition();
		animationMapTransition->Update(deltaTime);
		if (mapTransitionNextRow && animationMapTransition->GetCurrentFrame() == 0) {
			animationMapTransition->SetCurrentAction(animationMapTransition->GetCurrentAction() + 1);
			mapTransitionNextRow = false;
		}
		else if (animationMapTransition->GetCurrentFrame() == 1) mapTransitionNextRow = true;

		if (mapTransitionFlag && animationMapTransition->GetCurrentFrame() == 6 && animationMapTransition->GetCurrentAction() == 6) {
			mapTransitionFlag = false;
			mapTransitionNextRow = false;
			m_tileMap->LoadNewMap();
			Camera::GetInstance()->Update(deltaTime);
			animationMapTransition = std::make_shared<SpriteAnimation>(ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\map_transition_2.png"), 1, 9, 6, 0.02f);
			animationMapTransition->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
			animationMapTransition->Set2DPosition(Camera::GetInstance()->GetPosition().x, Camera::GetInstance()->GetPosition().y);
		}
		else if (!mapTransitionFlag && animationMapTransition->GetCurrentFrame() == 7 && animationMapTransition->GetCurrentAction() == 6) {
			mapTransitionFlag = true;
			mapTransitionNextRow = false;
			animationMapTransition = std::make_shared<SpriteAnimation>(ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\map_transition_1.png"), 1, 9, 6, 0.02f);
			animationMapTransition->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
			*m_timeScale = 1;
		}
	}

	if (*m_timeScale == -2) {
		timer += deltaTime;
		
		std::string loadingText = "LOADING";
		int dotCount = timer / 0.2f;
		if (dotCount == 4) {
			dotCount = 0;
			timer = 0;
		}
		for (int i = 0; i < dotCount; i++) loadingText += ".";
		txtLoading->SetText(loadingText);
		txtLoading->SetSize(LOADING_TEXT_WIDTH + dotCount * 10, LOADING_TEXT_HEIGHT);
		txtLoading->Set2DPosition( (SCREEN_WIDTH - txtLoading->GetWidth()) / 2.0f, txtLoading->Get2DPosition().y );

		return;
	}
}



void UIManager::Draw(SDL_Renderer* renderer) {
	if (*m_timeScale >= 0) {
		for (auto& it : buttons) it->Draw(renderer);
		for (auto& it : uiWhenRunGame) it->Draw(renderer);

		if (*m_timeScale == 1) {
			btnPause->Draw(renderer);
		}
		else if (*m_timeScale == 0) {
			for (auto& it : uiWhenPause) it->Draw(renderer);
		}
		else if (*m_timeScale == 2) {
			for (auto& it : uiWhenLevelUp) it->Draw(renderer);
		}
		return;
	}

	if (*m_timeScale == -1) {
		animationMapTransition->Draw(renderer);
	}

	if (*m_timeScale == -2) {
		loadingBackground->Draw(renderer);
		txtLoading->Draw(renderer);

		return;
	}
}


// sub-thread
void UIManager::UpdateStats() {
	std::thread t([this]() {
		int x, y; // mouse coordination
		int musicVolume, sfxVolume;
		float refillSpTime = 0.1f, spRefilled = 1.0f, refillMpTime = 1.0f, mpRefilled = 30;
		while (m_isPlaying) {
			if (*m_timeScale == 1) {
				if (exp == maxEXP) {
					Camera::GetInstance()->Update(0);
					exp = 0;
					pressButtonWhenLevelUp = false;
					selection = 0;
					*m_timeScale = 2;
				}

				// check if player is hovering an UI element:
				SDL_GetMouseState(&x, &y);
				if (x >= (SCREEN_WIDTH - 50) / 2.0f && x <= (SCREEN_WIDTH - 50) / 2.0f + 50 && y >= 4 && y <= 54 ||
					((x >= SLOT_0_POSITION_X && x <= SLOT_0_POSITION_X + SLOT_SIZE ||
						x >= SLOT_1_POSITION_X && x <= SLOT_1_POSITION_X + SLOT_SIZE) &&
						y >= SCREEN_HEIDHT - SLOT_Y && y <= SCREEN_HEIDHT - SLOT_Y + SLOT_SIZE)) {
					m_playerController->SetClickingStatus(true);
				}
				else m_playerController->SetClickingStatus(false);

				#pragma region Check Player stats
				// check HP_STATUS:
				int lossAmount = m_playerController->GetHpLose();
				if (lossAmount) {
					hpLoss += lossAmount;
					hpChangePerFrame = hpLoss / FRAGMENT_COUNT;

				}


				// check SP_STATUS:
				lossAmount = m_playerController->GetSpLose();
				if (lossAmount) {
					spLoss += lossAmount;
					spChangePerFrame = spLoss / FRAGMENT_COUNT;
				}

				spTimer += 0.01f;
				if (spTimer >= refillSpTime) {
					spTimer = 0;
					sp += 1;
					if (sp >= maxSP) sp = maxSP;
				}
				if (sp - lossAmount < 30 ) m_playerController->SetSpStatus(false);
				else m_playerController->SetSpStatus(true);


				// check MP_STATUS:
				lossAmount = m_playerController->GetMpLose();
				if (lossAmount) {
					mpLoss += lossAmount;
					mpChangePerFrame = mpLoss / FRAGMENT_COUNT;
				}

				mpTimer += 0.01f;
				if (mpTimer >= refillMpTime) {
					mpTimer = 0;
					mp += mpRefilled;
					if (mp >= maxMP) mp = maxMP;
				}
				if (mp - lossAmount < 30) m_playerController->SetMpStatus(false);
				else m_playerController->SetMpStatus(true);


				// reset change in value:
				m_playerController->ResetChangeInValue();
				#pragma endregion

				#pragma region Update Inventory slot
				if (timer <= 0) {
					int slot = m_playerController->GetWeaponIndex();

					if (slot != m_inventorySlot / mousewheelSensitivity) {
						m_inventorySlot = slot * mousewheelSensitivity;

						if (slot == 0) selectedSlot->Set2DPosition(SLOT_0_POSITION_X, SCREEN_HEIDHT - SLOT_Y);
						else selectedSlot->Set2DPosition(SLOT_1_POSITION_X, SCREEN_HEIDHT - SLOT_Y);
						Sound::GetInstance()->PlaySfx("ui_pop.wav");
					}
				}
				else {
					int maxSlotNumber = mousewheelSensitivity;
					m_inventorySlot += changeInWheelValue;
					changeInWheelValue = 0;
					if (m_inventorySlot > maxSlotNumber) m_inventorySlot = maxSlotNumber;
					else if (m_inventorySlot < 0) m_inventorySlot = 0;

					int slot = std::round((float)m_inventorySlot / mousewheelSensitivity);

					if (slot != m_playerController->GetWeaponIndex()) {
						m_playerController->SetWeaponIndex(slot);
						if (slot == 0) selectedSlot->Set2DPosition(SLOT_0_POSITION_X, SCREEN_HEIDHT - SLOT_Y);
						else selectedSlot->Set2DPosition(SLOT_1_POSITION_X, SCREEN_HEIDHT - SLOT_Y);
						Sound::GetInstance()->PlaySfx("ui_pop.wav");
					}

					timer -= 10;
					if (timer <= 0) m_inventorySlot = m_playerController->GetWeaponIndex() * mousewheelSensitivity;
				}
				#pragma endregion

				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}



			else if (*m_timeScale == 0) {
				musicVolume = 128.0f * (iconMusicSettings->Get2DPosition().x - SCREEN_WIDTH * SETTINGS_SOUND_START_X) /
										(SCREEN_WIDTH * (SETTINGS_SOUND_END_X - SETTINGS_SOUND_START_X));
				sfxVolume = 128.0f * (iconSfxSettings->Get2DPosition().x - SCREEN_WIDTH * SETTINGS_SOUND_START_X) /
									  (SCREEN_WIDTH * (SETTINGS_SOUND_END_X - SETTINGS_SOUND_START_X));

				Sound::GetInstance()->SetMusicVolume(musicVolume);
				Sound::GetInstance()->SetSfxVolume(sfxVolume);

				SaveData saveData = DataSaver::GetInstance()->LoadGame("save.dat");
				saveData.musicVolume = musicVolume;
				saveData.sfxVolume = sfxVolume;

				musicForegroundBar->SetSize(SCREEN_WIDTH * SETTINGS_BAR_WIDTH_RATIO * musicVolume / 128,
										    musicForegroundBar->GetHeight());
				sfxForegroundBar->SetSize(SCREEN_WIDTH * SETTINGS_BAR_WIDTH_RATIO * sfxVolume / 128,
										  sfxForegroundBar->GetHeight());

				if (pressButtonWhenPause) {
					pressButtonWhenPause = false;
					SDL_GetMouseState(&x, &y);
					if (x >= SCREEN_WIDTH * RESUME_BUTTON_LEFT_RATIO && x <= SCREEN_WIDTH * RESUME_BUTTON_RIGHT_RATIO &&
						y >= SCREEN_HEIDHT * PAUSE_BUTTON_TOP_RATIO && y <= SCREEN_HEIDHT * PAUSE_BUTTON_BOTTOM_RATIO)
					{
						*m_timeScale = 1;
					}
					else if (x >= SCREEN_WIDTH * QUIT_BUTTON_LEFT_RATIO && x <= SCREEN_WIDTH * QUIT_BUTTON_RIGHT_RATIO &&
							y >= SCREEN_HEIDHT * PAUSE_BUTTON_TOP_RATIO && y <= SCREEN_HEIDHT * PAUSE_BUTTON_BOTTOM_RATIO)
					{
						ClearThread();
					}
				}

				else if (releaseButtonWhenPause) {
					releaseButtonWhenPause = false;
					int a = std::round((iconSensitivitySettings->Get2DPosition().x - SCREEN_WIDTH * SETTINGS_SENSITIVITY_START_X) /
									   (SCREEN_WIDTH * (SETTINGS_SENSITIVITY_END_X - SETTINGS_SENSITIVITY_START_X)) * 4);
					mousewheelSensitivity = 9 - 2 * a;
					saveData.mousewheelSensitivity = mousewheelSensitivity;
					iconSensitivitySettings->Set2DPosition(
						SCREEN_WIDTH * (SETTINGS_SENSITIVITY_START_X + a * (SETTINGS_SENSITIVITY_END_X -
																			 SETTINGS_SENSITIVITY_START_X) / 4),
						iconSensitivitySettings->Get2DPosition().y
					);
				}

				DataSaver::GetInstance()->SetSaveData(saveData);
				DataSaver::GetInstance()->SaveGame("save.dat");

				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}



			else if (*m_timeScale == 2) {
				SDL_GetMouseState(&x, &y);

				// if hovering selection 1:
				if (x >= SELECTION_1_LEFT_RATIO * SCREEN_WIDTH && x <= SELECTION_1_RIGHT_RATIO * SCREEN_WIDTH &&
					y >= SELECTION_TOP_RATIO * SCREEN_HEIDHT && y <= SELECTION_BOTTOM_RATIO * SCREEN_HEIDHT)
				{
					hoverBorder->Set2DPosition(SELECTION_1_LEFT_RATIO * SCREEN_WIDTH, SELECTION_TOP_RATIO * SCREEN_HEIDHT);
					if (pressButtonWhenLevelUp) {
						selection = 1;
						selectionBorder->Set2DPosition(hoverBorder->Get2DPosition().x, hoverBorder->Get2DPosition().y);
					}
				}

				// if hovering selection 2:
				else if (x >= SELECTION_2_LEFT_RATIO * SCREEN_WIDTH && x <= SELECTION_2_RIGHT_RATIO * SCREEN_WIDTH &&
						 y >= SELECTION_TOP_RATIO * SCREEN_HEIDHT && y <= SELECTION_BOTTOM_RATIO * SCREEN_HEIDHT)
				{
					hoverBorder->Set2DPosition(SELECTION_2_LEFT_RATIO * SCREEN_WIDTH, SELECTION_TOP_RATIO * SCREEN_HEIDHT);
					if (pressButtonWhenLevelUp) {
						selection = 2;
						selectionBorder->Set2DPosition(hoverBorder->Get2DPosition().x, hoverBorder->Get2DPosition().y);
					}
				}

				// if hovering selection 3:
				else if (x >= SELECTION_3_LEFT_RATIO * SCREEN_WIDTH && x <= SELECTION_3_RIGHT_RATIO * SCREEN_WIDTH &&
						 y >= SELECTION_TOP_RATIO * SCREEN_HEIDHT && y <= SELECTION_BOTTOM_RATIO * SCREEN_HEIDHT)
				{
					hoverBorder->Set2DPosition(SELECTION_3_LEFT_RATIO * SCREEN_WIDTH, SELECTION_TOP_RATIO * SCREEN_HEIDHT);
					if (pressButtonWhenLevelUp) {
						selection = 3;
						selectionBorder->Set2DPosition(hoverBorder->Get2DPosition().x, hoverBorder->Get2DPosition().y);
					}
				}

				// if not hovering any selection:
				else {
					hoverBorder->Set2DPosition(SCREEN_WIDTH, 0);
					if (x >= UP_BUTTON_LEFT_RATIO * SCREEN_WIDTH && x <= UP_BUTTON_RIGHT_RATIO * SCREEN_WIDTH &&
						y >= UP_BUTTON_TOP_RATIO * SCREEN_HEIDHT && y <= UP_BUTTON_BOTTOM_RATIO * SCREEN_HEIDHT && selection != 0)
						underline->Set2DPosition(0, 0);
					else underline->Set2DPosition(SCREEN_WIDTH, 0);
				}


				if (selection != 0) {
					confirmButton->Set2DPosition(0, 0);
				}


				if (pressButtonWhenLevelUp) {
					pressButtonWhenLevelUp = false;
					// check confirm button's location when it's activated:
					if (x >= UP_BUTTON_LEFT_RATIO * SCREEN_WIDTH && x <= UP_BUTTON_RIGHT_RATIO * SCREEN_WIDTH &&
						y >= UP_BUTTON_TOP_RATIO * SCREEN_HEIDHT && y <= UP_BUTTON_BOTTOM_RATIO * SCREEN_HEIDHT) {
						if (selection == 1) {
							maxHP = std::round(maxHP * 1.1f);
							hpLoss -= 0.15f * maxHP;
							hpChangePerFrame = hpLoss / FRAGMENT_COUNT;
						}
						if (selection == 2) {
							maxSP += 5;
							spRefilled += 0.1f;
							maxMP += 10;
							mpRefilled += 5;
						}
						if (selection == 3) {
							m_playerController->SetPlayerDamage(m_playerController->GetPlayerDamage() + 5);
						}
						*m_timeScale = 1;
					}
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}

			

			else if (*m_timeScale == -1) {
			}
		}
		m_finishThread = true;
		GameStateMachine::GetInstance()->PopState();
	});

	t.detach();
}

void UIManager::ClearThread() {
	m_isPlaying = false;
}


bool UIManager::FloatEquals(float a, float b, float epsilon) {
	return std::fabs(a - b) < epsilon;
}