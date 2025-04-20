#include "GSPlay.h"
#include "ResourceManagers.h"
#include "GameObject/MouseButton.h"
#include "GameObject/Sprite2D.h"
#include "GameObject/SpriteAnimation.h"
#include "GameObject/Camera.h"
#include "KeyState.h"
#include "Sound.h"
#include "TileMap.h"

GSPlay::GSPlay()
{
	m_tileMap = std::make_shared<TileMap>();
	m_tileMapLoaded = false;
}

GSPlay::~GSPlay()
{
}

void GSPlay::Init()
{
	// Background layer 1
	auto tex1 = ResourceManagers::GetInstance()->GetTexture("background_layer_1.png");
	auto bg1 = std::make_shared<Sprite2D>(tex1, SDL_FLIP_NONE);
	bg1->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	bg1->Set2DPosition(0, 0);
	m_backgroundList.push_back(bg1);

	// Background layer 2
	auto tex2 = ResourceManagers::GetInstance()->GetTexture("background_layer_2.png");
	auto bg2 = std::make_shared<Sprite2D>(tex2, SDL_FLIP_NONE);
	bg2->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	bg2->Set2DPosition(0, 0);
	m_backgroundList.push_back(bg2);

	// Background layer 3
	auto tex3 = ResourceManagers::GetInstance()->GetTexture("background_layer_3.png");
	auto bg3 = std::make_shared<Sprite2D>(tex3, SDL_FLIP_NONE);
	bg3->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	bg3->Set2DPosition(0, 0);
	m_backgroundList.push_back(bg3);

	// Button close
	auto texture = ResourceManagers::GetInstance()->GetTexture("btn_close.tga");
	button = std::make_shared<MouseButton>(texture, SDL_FLIP_NONE);
	button->SetSize(50, 50);
	button->Set2DPosition(SCREEN_WIDTH - 50, 10);
	button->SetOnClick([this]() {
		GameStateMachine::GetInstance()->PopState();
		});
	m_listButton.push_back(button);

	// Actor animation
	texture = ResourceManagers::GetInstance()->GetTexture("Actor1_2.tga");
	obj = std::make_shared<SpriteAnimation>(texture, 2, 9, 6, 0.2f);
	obj->SetFlip(SDL_FLIP_HORIZONTAL);
	obj->SetSize(80, 100);
	obj->Set2DPosition(240, 400);
	m_listAnimation.push_back(obj);

	m_KeyPress = 0;

	/*m_tileMap->LoadMap("D:/SLD2Project/LEAP2025/sdl2project-main/SDL/x64/Debug/Map/map4.txt");*/
	m_tileMap->LoadMap("Data/Map/map4.txt");
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
	if (e.type == SDL_KEYDOWN)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_LEFT:  m_KeyPress |= 1; break;
		case SDLK_DOWN:  m_KeyPress |= 1 << 1; break;
		case SDLK_RIGHT: m_KeyPress |= 1 << 2; break;
		case SDLK_UP:    m_KeyPress |= 1 << 3; break;
		default: break;
		}
	}
	else if (e.type == SDL_KEYUP)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_LEFT:  m_KeyPress ^= 1; break;
		case SDLK_DOWN:  m_KeyPress ^= 1 << 1; break;
		case SDLK_RIGHT: m_KeyPress ^= 1 << 2; break;
		case SDLK_UP:    m_KeyPress ^= 1 << 3; break;
		default: break;
		}
	}
}

void GSPlay::HandleTouchEvents(SDL_Event& e)
{
	for (auto& button : m_listButton)
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
	switch (m_KeyPress)
	{
	default:
		break;
	}

	for (auto& it : m_listButton)
		it->Update(deltaTime);

	for (auto& it : m_listAnimation)
		it->Update(deltaTime);
}

void GSPlay::Draw(SDL_Renderer* renderer)
{
	// Vẽ các lớp background
	for (auto& bg : m_backgroundList)
	{
		bg->Draw(renderer);
	}


	// Vẽ tilemap
	if (m_tileMap)
	{
		m_tileMap->DrawMap(renderer);
	}
	if (!m_tileMapLoaded)
	{
		/*m_tileMap->LoadTiles(renderer, "D://SLD2Project//LEAP2025//sdl2project-main//SDL//x64//Debug//Map//tiles", 32);*/
		m_tileMap->LoadTiles(renderer, "Data/Map/tiles", 32);
		m_tileMapLoaded = true;
	}

	// Vẽ nút
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
