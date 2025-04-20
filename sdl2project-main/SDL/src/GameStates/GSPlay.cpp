#include "GSPlay.h"
#include "ResourceManagers.h"
#include "GameObject/MouseButton.h"
#include "GameObject/Sprite2D.h"
#include "GameObject/SpriteAnimation.h"
#include "GameObject/Camera.h"
#include "KeyState.h"
#include "Sound.h"
GSPlay::GSPlay()
{
}


GSPlay::~GSPlay()
{
}


void GSPlay::Init()
{
	// Layer 1
	auto tex1 = ResourceManagers::GetInstance()->GetTexture("background_layer_1.png");
	auto bg1 = std::make_shared<Sprite2D>(tex1, SDL_FLIP_NONE);
	bg1->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	bg1->Set2DPosition(0, 0);
	m_backgroundList.push_back(bg1);

	// Layer 2
	auto tex2 = ResourceManagers::GetInstance()->GetTexture("background_layer_2.png");
	auto bg2 = std::make_shared<Sprite2D>(tex2, SDL_FLIP_NONE);
	bg2->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	bg2->Set2DPosition(0, 0);
	m_backgroundList.push_back(bg2);

	// Layer 3
	auto tex3 = ResourceManagers::GetInstance()->GetTexture("background_layer_3.png");
	auto bg3 = std::make_shared<Sprite2D>(tex3, SDL_FLIP_NONE);
	bg3->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	bg3->Set2DPosition(0, 0);
	m_backgroundList.push_back(bg3);

	// button close
	auto texture = ResourceManagers::GetInstance()->GetTexture("btn_close.tga");
	button = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
	button->SetSize(50, 50);
	button->Set2DPosition(SCREEN_WIDTH - 50, 10);
	button->SetOnClick([this]() {
		GameStateMachine::GetInstance()->PopState();
		});
	m_listButton.push_back(button);

	// Animation
	texture = ResourceManagers::GetInstance()->GetTexture("Actor1_2.tga");
	obj = std::make_shared<SpriteAnimation>(texture, 2, 9, 6, 0.2f);
	obj->SetFlip(SDL_FLIP_HORIZONTAL);
	obj->SetSize(40, 50);
	obj->Set2DPosition(240, 400);
	m_listAnimation.push_back(obj);

	m_KeyPress = 0;
}


void GSPlay::Exit()
{
}


void GSPlay::Pause()
{

}
void GSPlay::Resume()
{
}


void GSPlay::HandleEvents()
{
}

void GSPlay::HandleKeyEvents(SDL_Event& e)
{
	//If a key was pressed
	if (e.type == SDL_KEYDOWN )//&& e.key.repeat == 0) //For e.key.repeat it's because key repeat is enabled by default and if you press and hold a key it will report multiple key presses. That means we have to check if the key press is the first one because we only care when the key was first pressed.
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)                                                                                            
		{                                                                                                                     
		case SDLK_LEFT:
			printf("MOVE LEFT");
			m_KeyPress |= 1;
			break;
		case SDLK_DOWN:
			printf("MOVE BACK");
			m_KeyPress |= 1 << 1;
			break;
		case SDLK_RIGHT:
			m_KeyPress |= 1 << 2;
			break;
		case SDLK_UP:
			m_KeyPress |= 1 << 3;
			break;
		default:
			break;
		}
	}
	////Key Up
	else if (e.type == SDL_KEYUP )//&& e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_LEFT:
			m_KeyPress ^= 1;
			break;
		case SDLK_DOWN:
			m_KeyPress ^= 1 << 1;
			break;
		case SDLK_RIGHT:
			m_KeyPress ^= 1 << 2;
			break;
		case SDLK_UP:
			m_KeyPress ^= 1 << 3;
			break;
		default:
			break;
		}
	}
}

void GSPlay::HandleTouchEvents(SDL_Event& e)
{
	for (auto button : m_listButton)
	{
		if (button->HandleTouchEvent(&e))
		{
			break;
		}
	}
}

void GSPlay::HandleMouseMoveEvents(int x, int y)
{
}

void GSPlay::Update(float deltaTime)
{
	switch (m_KeyPress)//Handle Key event start
	{
	default:
		break;
	}
	// Key State event end

	for (auto it : m_listButton)
	{
		it->Update(deltaTime);
	}
	for (auto it : m_listAnimation)
	{
		it->Update(deltaTime);
	}
}

void GSPlay::Draw(SDL_Renderer* renderer)
{
	// Vẽ tất cả lớp background
	for (auto& bg : m_backgroundList)
	{
		bg->Draw(renderer);
	}

	// Vẽ button
	for (auto& it : m_listButton)
	{
		it->Draw(renderer);
	}

	// Vẽ animation
	for (auto& it : m_listAnimation)
	{
		it->Draw(renderer);
	}
}
