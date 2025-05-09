/*
	m_timeScale == -2 => LOADING SCENE
	m_timeScale == -1 => CHANGE LEVEL
	m_timeScale == 0 => PAUSE GAME (just is pause, or level up, die, victory<complete all levels>) 
	m_timeScale == 1 => RESUME GAME
*/

#include "UIManager.h"

#include <thread>

#include "ResourceManagers.h"
#include "Renderer.h"
#include "Define.h"


#pragma region Constant expression evaluation
constexpr int HP_BACKGROUND_BAR_WIDTH_IN_SPRITE = 512;
constexpr int BACKGROUND_BAR_HEIGHT = 16;
constexpr int BAR_LEFT = 10;
constexpr int BAR_GAP = 8;

constexpr int STAT_TEXT_WIDTH = 100;
constexpr int HP_BACKGROUND_BAR_POSITION_X = BAR_LEFT, HP_BACKGROUND_BAR_POSITION_Y = 4;
constexpr int SP_BACKGROUND_BAR_POSITION_X = BAR_LEFT,
			  SP_BACKGROUND_BAR_POSITION_Y = HP_BACKGROUND_BAR_POSITION_Y + BACKGROUND_BAR_HEIGHT + BAR_GAP;
constexpr int MP_BACKGROUND_BAR_POSITION_X = BAR_LEFT,
			  MP_BACKGROUND_BAR_POSITION_Y = SP_BACKGROUND_BAR_POSITION_Y + BACKGROUND_BAR_HEIGHT + BAR_GAP;
#pragma endregion



#pragma region Constructor && Deconstructor
UIManager::UIManager(std::shared_ptr<int> timeScale) :
	m_renderer(Renderer::GetInstance()->GetRenderer()), m_timeScale(timeScale),
	hpTextColor({ 0, 0, 0 }), expTextColor({ 255,255,255 })
{
	DisplayLoadingScene();
	m_inventorySlot = 1;
	
	// get player's stats from save file:
	int hp = 100, sp = 100, mp = 100, exp = 100;
	int maxHP = 100, maxSP = 100, maxMP = 100, maxEXP = 100;
	std::string hpStat = "HP: " + std::to_string(hp) + "/" + std::to_string(maxHP);
	std::string spStat = "SP: " + std::to_string(sp) + "/" + std::to_string(maxSP);
	std::string mpStat = "MP: " + std::to_string(mp) + "/" + std::to_string(maxMP);
	std::string expStat = "EXP: " + std::to_string(exp) + "/" + std::to_string(maxEXP);

	fullHpBarWidth = SCREEN_WIDTH / 3.0f;
	float widthScaling = fullHpBarWidth / HP_BACKGROUND_BAR_WIDTH_IN_SPRITE;
	fullSpBarWidth = fullHpBarWidth * 5 / 6;
	fullMpBarWidth = fullHpBarWidth / 2;

	auto font = ResourceManagers::GetInstance()->GetFont("FVF Fernando 08.ttf", 36);

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



#pragma region Multi-thread
void UIManager::DisplayLoadingScene() {
    std::thread t([this]() {
		int dotCount = 0;
		std::string loadingText = "LOADING";

		loadingBackground = std::make_shared<Sprite2D>(ResourceManagers::GetInstance()->GetTexture("bg_settings.png"), SDL_FLIP_NONE);
		loadingBackground->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
		loadingBackground->Set2DPosition(0, 0);

		txtLoading = std::make_shared<Text>(loadingText, ResourceManagers::GetInstance()->GetFont("FVF Fernando 08.ttf", 36), expTextColor);
		txtLoading->SetSize(300, 36);
		txtLoading->Set2DPosition( (SCREEN_WIDTH - txtLoading->GetWidth()) / 2.0f, SCREEN_HEIDHT - txtLoading->GetHeight() - 20);

		/*while (*m_timeScale == -2) {
			dotCount++;
			loadingText += ".";
			if (dotCount == 4) {
				dotCount = 0;
				loadingText = "LOADING";
			}

			loadingBackground->Draw(m_renderer);
			txtLoading->Draw(m_renderer);

			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}*/

		UpdateUI();
    });

    t.detach();
}



void UIManager::UpdateUI() {
	std::thread t([this]() {


		m_finishThread = true;
	});

	t.detach();
}



void UIManager::ClearThread() {
	*m_timeScale = -2;
}
#pragma endregion



void UIManager::Draw(SDL_Renderer* renderer) {
	for (auto& it : uiElements) it->Draw(renderer);
	for (auto& it : texts) it->Draw(renderer);
}