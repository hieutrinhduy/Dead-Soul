/*
	m_timeScale == -2 => LOADING SCENE
	m_timeScale == -1 => CHANGE LEVEL
	m_timeScale == 0 => PAUSE GAME (just is pause, or level up, die, victory<complete all levels>) 
	m_timeScale == 1 => RESUME GAME
*/

#include "UIManager.h"

#include <thread>
#include <mutex>
#include <cmath>

#include "ResourceManagers.h"
#include "Renderer.h"
#include "Define.h"


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

constexpr int FRAGMENT_COUNT = 20;
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

	// LOADING SCENE:
	loadingBackground = std::make_shared<Sprite2D>(ResourceManagers::GetInstance()->GetTexture("bg_loading.png"), SDL_FLIP_NONE);
	loadingBackground->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	loadingBackground->Set2DPosition(0, 0);
	loadingBackground->Draw(m_renderer);

	txtLoading = std::make_shared<Text>("LOADING", font, hpTextColor);
	txtLoading->SetSize(LOADING_TEXT_WIDTH, LOADING_TEXT_HEIGHT);
	txtLoading->Set2DPosition( (SCREEN_WIDTH - LOADING_TEXT_WIDTH) / 2.0f, SCREEN_HEIDHT - LOADING_TEXT_HEIGHT - 75 );

	m_inventorySlot = 0;
	

	// get data from save file:
	mousewheelSensitivity = 3;
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


	// HP BAR:
	auto texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\hp_background_bar.png");
	hpBgBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	hpBgBar->Set2DPosition(HP_BACKGROUND_BAR_POSITION_X, HP_BACKGROUND_BAR_POSITION_Y);
	hpBgBar->SetSize(fullHpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiElements.push_back(hpBgBar);

	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\hp_bar.png");
	hpBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	hpBar->Set2DPosition(HP_BACKGROUND_BAR_POSITION_X + widthScaling, HP_BACKGROUND_BAR_POSITION_Y);
	hpBar->SetSize(fullHpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiElements.push_back(hpBar);

	txtHP = std::make_shared<Text>(hpStat, font, hpTextColor);
	txtHP->SetSize(STAT_TEXT_WIDTH, BACKGROUND_BAR_HEIGHT);
	txtHP->Set2DPosition( (fullHpBarWidth - txtHP->GetWidth()) / 2, HP_BACKGROUND_BAR_POSITION_Y - 2 );
	texts.push_back(txtHP);

	
	// SP BAR:
	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\sp_background_bar.png");
	spBgBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	spBgBar->Set2DPosition(SP_BACKGROUND_BAR_POSITION_X, SP_BACKGROUND_BAR_POSITION_Y);
	spBgBar->SetSize(fullSpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiElements.push_back(spBgBar);

	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\sp_bar.png");
	spBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	spBar->Set2DPosition(SP_BACKGROUND_BAR_POSITION_X + widthScaling, SP_BACKGROUND_BAR_POSITION_Y);
	spBar->SetSize(fullSpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiElements.push_back(spBar);

	txtSP = std::make_shared<Text>(spStat, font, hpTextColor);
	txtSP->SetSize(STAT_TEXT_WIDTH, BACKGROUND_BAR_HEIGHT);
	txtSP->Set2DPosition( (fullSpBarWidth - txtSP->GetWidth()) / 2, SP_BACKGROUND_BAR_POSITION_Y - 2 );
	texts.push_back(txtSP);


	// MP BAR:
	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\mp_background_bar.png");
	mpBgBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	mpBgBar->Set2DPosition(MP_BACKGROUND_BAR_POSITION_X, MP_BACKGROUND_BAR_POSITION_Y);
	mpBgBar->SetSize(fullMpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiElements.push_back(mpBgBar);

	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\mp_bar.png");
	mpBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	mpBar->Set2DPosition(MP_BACKGROUND_BAR_POSITION_X + widthScaling, MP_BACKGROUND_BAR_POSITION_Y);
	mpBar->SetSize(fullMpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiElements.push_back(mpBar);

	txtMP = std::make_shared<Text>(mpStat, font, hpTextColor);
	txtMP->SetSize(STAT_TEXT_WIDTH, BACKGROUND_BAR_HEIGHT);
	txtMP->Set2DPosition( (fullMpBarWidth - txtMP->GetWidth()) / 2, MP_BACKGROUND_BAR_POSITION_Y - 2 );
	texts.push_back(txtMP);


	// EXP BAR:
	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\exp_background_bar.png");
	expBgBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	expBgBar->Set2DPosition(0, SCREEN_HEIDHT - 20);
	expBgBar->SetSize(SCREEN_WIDTH, 20);
	uiElements.push_back(expBgBar);

	texture = ResourceManagers::GetInstance()->GetTexture("GSPlay_UI\\exp_bar.png");
	expBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	expBar->Set2DPosition(0, SCREEN_HEIDHT - 20);
	expBar->SetSize(SCREEN_WIDTH, 20);
	uiElements.push_back(expBar);

	txtEXP = std::make_shared<Text>(expStat, font, expTextColor);
	txtEXP->SetSize(STAT_TEXT_WIDTH + 5, expBar->GetHeight());
	txtEXP->Set2DPosition( (SCREEN_WIDTH - txtEXP->GetWidth()) / 2.0f, expBar->Get2DPosition().y - 2 );
	texts.push_back(txtEXP);
}


UIManager::~UIManager() {}
#pragma endregion


#pragma region Handle events
void UIManager::HandleTouchEvents(SDL_Event& e)
{
	
}



void UIManager::HandleMouseEvents(SDL_Event& e) {
	if (*m_timeScale == 1) {
		if (e.type == SDL_MOUSEWHEEL) {
			std::lock_guard<std::mutex> lock(mtx);
			if (timer <= 0) timer = 500;
			changeInWheelValue += e.wheel.y;
		}
	}
}
#pragma endregion


void UIManager::Update(float deltaTime) {
	if (*m_timeScale == 1) {
		// UPDATE HP:
		if (!FloatEquals(hpChangePerFrame, 0.0f) && !FloatEquals(hpLoss, 0.0f)) {
			hp -= hpChangePerFrame;
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
	if (*m_timeScale == 1) {
		for (auto& it : uiElements) it->Draw(renderer);
		for (auto& it : texts) it->Draw(renderer);
		return;
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
		while (m_isPlaying) {
			if (*m_timeScale == 1) {
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
				if (spTimer >= 0.01f) {
					spTimer = 0;
					sp += 0.5;
					if (sp > maxSP) sp = maxSP;
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
				if (mpTimer >= 2.0f) {
					mpTimer = 0;
					mp += 20;
					if (mp > maxMP) mp = maxMP;
				}
				if (mp - lossAmount < 30) m_playerController->SetMpStatus(false);
				else m_playerController->SetMpStatus(true);


				// reset change in value:
				m_playerController->ResetChangeInValue();
				#pragma endregion


				#pragma region Update Inventory slot
				if (timer <= 0) m_inventorySlot = m_playerController->GetWeaponIndex() * mousewheelSensitivity;
				else {
				int maxSlotNumber = mousewheelSensitivity;
				m_inventorySlot += changeInWheelValue;
				changeInWheelValue = 0;
				if (m_inventorySlot > maxSlotNumber) m_inventorySlot = maxSlotNumber;
				else if (m_inventorySlot < 0) m_inventorySlot = 0;

				int slot = m_inventorySlot / mousewheelSensitivity;

				if (m_inventorySlot / mousewheelSensitivity != m_playerController->GetWeaponIndex()) {
					m_playerController->SetWeaponIndex(m_inventorySlot / mousewheelSensitivity);
				}

				timer -= 10;
				if (timer <= 0) m_inventorySlot = m_inventorySlot / mousewheelSensitivity * mousewheelSensitivity;
				}
				#pragma endregion
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		m_finishThread = true;
	});

	t.detach();
}

void UIManager::ClearThread() {
	m_isPlaying = false;
}


bool UIManager::FloatEquals(float a, float b, float epsilon) {
	return std::fabs(a - b) < epsilon;
}