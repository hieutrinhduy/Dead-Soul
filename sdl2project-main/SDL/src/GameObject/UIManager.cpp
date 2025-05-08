#include "UIManager.h"

#include "ResourceManagers.h"
#include "Define.h"


#pragma region Constant expression evaluation
constexpr int HP_BACKGROUND_BAR_WIDTH_IN_SPRITE = 16;
constexpr int BACKGROUND_BAR_HEIGHT = 16;
constexpr int BAR_LEFT = 10;
constexpr int BAR_GAP = 8;

constexpr int HP_BACKGROUND_BAR_POSITION_X = BAR_LEFT, HP_BACKGROUND_BAR_POSITION_Y = 2;
constexpr int SP_BACKGROUND_BAR_POSITION_X = BAR_LEFT,
			  SP_BACKGROUND_BAR_POSITION_Y = HP_BACKGROUND_BAR_POSITION_Y + BACKGROUND_BAR_HEIGHT + BAR_GAP;
constexpr int MP_BACKGROUND_BAR_POSITION_X = BAR_LEFT,
			  MP_BACKGROUND_BAR_POSITION_Y = SP_BACKGROUND_BAR_POSITION_Y + BACKGROUND_BAR_HEIGHT + BAR_GAP;
#pragma endregion



UIManager::UIManager() {
	m_inventorySlot = 1;

	fullHpBarWidth = SCREEN_WIDTH / 3;
	float widthScaling = fullHpBarWidth / HP_BACKGROUND_BAR_WIDTH_IN_SPRITE;
	fullSpBarWidth = fullHpBarWidth * 5 / 6;
	fullMpBarWidth = fullHpBarWidth / 2;


	// HP BAR:
	auto texture = ResourceManagers::GetInstance()->GetTexture("img_background_bar.png");
	hpBgBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	hpBgBar->Set2DPosition(HP_BACKGROUND_BAR_POSITION_X, HP_BACKGROUND_BAR_POSITION_Y);
	hpBgBar->SetSize(fullHpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiElements.push_back(hpBgBar);

	texture = ResourceManagers::GetInstance()->GetTexture("img_foreground_bar.png");
	hpBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	hpBar->Set2DPosition(HP_BACKGROUND_BAR_POSITION_X + widthScaling, HP_BACKGROUND_BAR_POSITION_Y);
	hpBar->SetSize(fullHpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiElements.push_back(hpBar);

	
	// SP BAR:
	texture = ResourceManagers::GetInstance()->GetTexture("img_background_bar.png");
	spBgBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	spBgBar->Set2DPosition(SP_BACKGROUND_BAR_POSITION_X, SP_BACKGROUND_BAR_POSITION_Y);
	spBgBar->SetSize(fullSpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiElements.push_back(spBgBar);

	texture = ResourceManagers::GetInstance()->GetTexture("img_foreground_bar.png");
	spBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	spBar->Set2DPosition(SP_BACKGROUND_BAR_POSITION_X + widthScaling, SP_BACKGROUND_BAR_POSITION_Y);
	spBar->SetSize(fullSpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiElements.push_back(spBar);


	// MP BAR:
	texture = ResourceManagers::GetInstance()->GetTexture("img_background_bar.png");
	mpBgBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	mpBgBar->Set2DPosition(MP_BACKGROUND_BAR_POSITION_X, MP_BACKGROUND_BAR_POSITION_Y);
	mpBgBar->SetSize(fullMpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiElements.push_back(mpBgBar);

	texture = ResourceManagers::GetInstance()->GetTexture("img_foreground_bar.png");
	mpBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	mpBar->Set2DPosition(MP_BACKGROUND_BAR_POSITION_X + widthScaling, MP_BACKGROUND_BAR_POSITION_Y);
	mpBar->SetSize(fullMpBarWidth, BACKGROUND_BAR_HEIGHT);
	uiElements.push_back(mpBar);


	// EXP BAR:
	texture = ResourceManagers::GetInstance()->GetTexture("img_foreground_bar.png");
	expBar = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	expBar->Set2DPosition(0, SCREEN_HEIDHT - 30);
	expBar->SetSize(SCREEN_WIDTH, 30);
	uiElements.push_back(expBar);
}


UIManager::~UIManager() {}




void UIManager::Draw(SDL_Renderer* renderer) {
	for (auto& it : uiElements) it->Draw(renderer);
}