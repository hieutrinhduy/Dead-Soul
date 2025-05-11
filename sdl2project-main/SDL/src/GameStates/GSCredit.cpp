#include "GSCredit.h"

#include "DataSaver.h"
#include "ResourceManagers.h"

#include "Sound.h"


constexpr auto NARRATION_1 = "Hoang Trung Tien - VETERAN KNIGHT";
constexpr auto NARRATION_2 = "Trinh Duy Hieu - VETERAN KNIGHT";
#define NARRATION_1_SIZE 480,40
#define NARRATION_2_SIZE 440,40



GSCredit::GSCredit() : GameStateBase(StateType::STATE_CREDIT), narrationColor({ 0, 0, 0 })
{
	widthScaling = SCREEN_WIDTH / 128.0f;
	heightScaling = SCREEN_HEIDHT / 82.0f;
}


GSCredit::~GSCredit()
{
}


void GSCredit::Init()
{
	// BOOK BACKGROUND:
	auto texture = ResourceManagers::GetInstance()->GetTexture("bg_credit.png");
	sceneBackground = std::make_shared<Sprite2D>(texture, SDL_FLIP_NONE);
	sceneBackground->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	sceneBackground->Set2DPosition(0, 0);


	// NARRATIONS:
	auto font = ResourceManagers::GetInstance()->GetFont("FVF Fernando 08.ttf", 28);
	narration1 = std::make_shared<Text>(NARRATION_1, font, narrationColor);
	narration1->SetSize(NARRATION_1_SIZE);
	narration1->Set2DPosition( (SCREEN_WIDTH / 2.0f - narration1->GetWidth()) / 2, heightScaling * 48 );

	font = ResourceManagers::GetInstance()->GetFont("FVF Fernando 08.ttf", 28);
	narration2 = std::make_shared<Text>(NARRATION_2, font, narrationColor);
	narration2->SetSize(NARRATION_2_SIZE);
	narration2->Set2DPosition((SCREEN_WIDTH * 3 / 2.0f - narration2->GetWidth()) / 2, heightScaling * 48 );

	
	// BUTTON:
	texture = ResourceManagers::GetInstance()->GetTexture("btn_close_credit.png");
	auto button = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
	button->SetSize(250, 80);
	button->Set2DPosition(10, SCREEN_HEIDHT - 100);
	button->SetOnClick([this]() {
		Sound::GetInstance()->PlaySfx("book_close_sound.wav");
		GameStateMachine::GetInstance()->PopState();
	});
	m_listButton.push_back(button);


	// SOUNDS:
	Sound::GetInstance()->LoadSfx("book_open_sound.wav");
	Sound::GetInstance()->PlaySfx("book_open_sound.wav");

	Sound::GetInstance()->LoadSfx("book_close_sound.wav");
}



void GSCredit::Exit()
{
}


void GSCredit::Pause()
{
}

void GSCredit::Resume()
{
}


void GSCredit::HandleEvents()
{
}

void GSCredit::HandleKeyEvents(SDL_Event& e)
{
}

void GSCredit::HandleTouchEvents(SDL_Event& e)
{
	for (auto button : m_listButton)
	{
		if (button->HandleTouchEvent(&e))
		{
			break;
		}
	}
}

void GSCredit::HandleMouseMoveEvents(SDL_Event& e)
{
}

void GSCredit::Update(float deltaTime)
{
	
}

void GSCredit::Draw(SDL_Renderer* renderer)
{
	// draw BACKGROUND:
	sceneBackground->Draw(renderer);

	// draw NARRATIONS:
	narration1->Draw(renderer);
	narration2->Draw(renderer);

	// draw BUTTONS:
	for (auto& it : m_listButton) it->Draw(renderer);
}
