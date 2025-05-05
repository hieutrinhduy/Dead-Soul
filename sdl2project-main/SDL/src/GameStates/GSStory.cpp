#include "GSStory.h"

#include "ResourceManagers.h"

#include "GameObject/MouseButton.h"
#include "GameObject/Sprite2D.h"
#include "GameObject/SpriteAnimation.h"
#include "GameObject/Camera.h"
#include "KeyState.h"
#include "Sound.h"


constexpr int SPRITE_COLS = 8;
constexpr int SPRITE_ROWS = 12;
constexpr int NARRATION_TEXT_HEIGHT = 60;
constexpr const char* LINE_1 = "The war between humans and ghosts took place for thousands of years";
#define LINE_1_SIZE  SCREEN_WIDTH * 0.75f, NARRATION_TEXT_HEIGHT
constexpr const char* LINE_2 = "Wisdom and resilience give humans an advantage over the fierce monsters.";
#define LINE_2_SIZE  SCREEN_WIDTH * 0.85f, NARRATION_TEXT_HEIGHT
constexpr const char* LINE_3 = "But the monsters kept appearing more frequently...";
#define LINE_3_SIZE  SCREEN_WIDTH * 0.6f, NARRATION_TEXT_HEIGHT
constexpr const char* LINE_4 = "... their sheer numbers are slowly becoming a horrifying nightmare.";
#define LINE_4_SIZE  SCREEN_WIDTH * 0.75f, NARRATION_TEXT_HEIGHT
constexpr const char* LINE_5 = "They must be ELIMINATED at their very ROOT!";
#define LINE_5_SIZE  SCREEN_WIDTH * 0.6f, NARRATION_TEXT_HEIGHT
#define LINE_POSITION (SCREEN_WIDTH - narrationLine->GetWidth()) / 2.0f, SCREEN_HEIDHT - NARRATION_TEXT_HEIGHT - 5

/*	DETAILS OF STORY'S ANIMATION
	The animation consists of (8 x 12) frames.
	Frames 0 -> 19 (line 1): run at 20 FPS, frame 20: runs in 3s, frames 21->40: run at 20 FPS;
	frame 41 (line 2): runs in 4s, frames 42 -> 47: run at 10 FPS;
	frame 48 (line 3): runs in 3s;
	frames 49 -> 55 (line 4): run at 15 FPS;
	frame 56: runs in 3s;
	frames 57 -> 94: run at 20 FPS, stop narrating at frame 80
	frame 95 (line 5): runs in 2.5s
*/

GSStory::GSStory() : GameStateBase(StateType::STATE_STORY),
	textColor({ 0, 0, 0 }), isNarrating(false), frameRateFlag(false), newRow(false)
{
}


GSStory::~GSStory()
{
}


void GSStory::Init()
{
	// STORY SPRITE:
	auto texture = ResourceManagers::GetInstance()->GetTexture("story.png");
	projector = std::make_shared<SpriteAnimation>(texture, 1, SPRITE_COLS, SPRITE_ROWS, 4.0f);
	projector->SetFlip(SDL_FLIP_NONE);
	projector->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT);
	projector->Set2DPosition(0, 0);

	// set NARRATION TEXT:
	auto font = ResourceManagers::GetInstance()->GetFont("arial.ttf", 48);
	narrationLine = std::make_shared<Text>(LINE_1, font, textColor);
	narrationLine->SetSize(LINE_1_SIZE);
	narrationLine->Set2DPosition(LINE_POSITION);

	// SOUND:
	Sound::GetInstance()->LoadSound("story_music.mp3");
	Sound::GetInstance()->PlaySound("story_music.mp3");
}

void GSStory::Exit()
{
}


void GSStory::Pause()
{
}

void GSStory::Resume()
{

}


void GSStory::HandleEvents()
{
}

void GSStory::HandleKeyEvents(SDL_Event& e)
{
}

void GSStory::HandleTouchEvents(SDL_Event& e)
{
}

void GSStory::HandleMouseMoveEvents(SDL_Event& e)
{
}

void GSStory::Update(float deltaTime)
{
	projector->Update(deltaTime);

	if (newRow && projector->GetCurrentFrame() == 0) {
		projector->SetCurrentAction(projector->GetCurrentAction() + 1);
		newRow = false;
	}
	if (!newRow && projector->GetCurrentFrame() == 1) newRow = true;


	// frame 1: story 1 fade in transition
	if (!frameRateFlag && projector->GetCurrentFrame() == 1 && projector->GetCurrentAction() == 1) {
		projector->SetFrameTime(0.05f);
		projector->SetSize(SCREEN_WIDTH, SCREEN_HEIDHT - NARRATION_TEXT_HEIGHT);
		frameRateFlag = true;
		isNarrating = true;
	}
	// frame 20: story 1 projection
	if (frameRateFlag && projector->GetCurrentFrame() == 4 && projector->GetCurrentAction() == 3) {
		projector->SetFrameTime(3.0f);
		frameRateFlag = false;
	}
	// frame 21: story 1 fade out transition
	else if (!frameRateFlag && projector->GetCurrentFrame() == 5 && projector->GetCurrentAction() == 3) {
		projector->SetFrameTime(0.05f);
		frameRateFlag = true;
	}
	// frame 41: story 2 projection
	else if (frameRateFlag && projector->GetCurrentFrame() == 1 && projector->GetCurrentAction() == 6) {
		projector->SetFrameTime(4.0f);
		narrationLine->SetText(LINE_2);
		narrationLine->SetSize(LINE_2_SIZE);
		narrationLine->Set2DPosition(LINE_POSITION);
		frameRateFlag = false;
	}
	// frame 42: story 2 wipe down transition
	else if (!frameRateFlag && projector->GetCurrentFrame() == 2 && projector->GetCurrentAction() == 6) {
		projector->SetFrameTime(0.1f);
		frameRateFlag = true;
	}
	// frame 48: line 3 with black background
	else if (frameRateFlag && projector->GetCurrentFrame() == 0 && projector->GetCurrentAction() == 7) {
		projector->SetFrameTime(3.0f);
		narrationLine->SetText(LINE_3);
		narrationLine->SetSize(LINE_3_SIZE);
		narrationLine->Set2DPosition(LINE_POSITION);
		frameRateFlag = false;
	}
	// frame 49: story 3 wipe up transition
	else if (!frameRateFlag && projector->GetCurrentFrame() == 1 && projector->GetCurrentAction() == 7) {
		projector->SetFrameTime(1.0f / 15);
		narrationLine->SetText(LINE_4);
		narrationLine->SetSize(LINE_4_SIZE);
		narrationLine->Set2DPosition(LINE_POSITION);
		frameRateFlag = true;
	}
	// frame 56: story 3 projection
	else if (frameRateFlag && projector->GetCurrentFrame() == 0 && projector->GetCurrentAction() == 8) {
		projector->SetFrameTime(3.0f);
		frameRateFlag = false;
	}
	// frame 57: story 3 fade out transition
	else if (!frameRateFlag && projector->GetCurrentFrame() == 1 && projector->GetCurrentAction() == 8) {
		projector->SetFrameTime(0.05f);
		frameRateFlag = true;
	}
	// frame 80: stop displaying line 4
	else if (isNarrating && frameRateFlag && projector->GetCurrentFrame() == 0 && projector->GetCurrentAction() == 11) {
		isNarrating = false; // stop narrating
	}
	// frame 95: line 5 with black background
	else if (frameRateFlag && projector->GetCurrentFrame() == 7 && projector->GetCurrentAction() == 12) {
		projector->SetFrameTime(2.5f);
		narrationLine->SetText(LINE_5);
		narrationLine->SetSize(LINE_5_SIZE);
		narrationLine->Set2DPosition(LINE_POSITION);
		frameRateFlag = false;
		isNarrating = true;
	}


	// if reach the last frame
	else if (projector->GetCurrentAction() == 13) {
		Sound::GetInstance()->ClearAudioResource();
		GameStateMachine::GetInstance()->ChangeState(StateType::STATE_INTRO);
	}
}

void GSStory::Draw(SDL_Renderer* renderer)
{
	projector->Draw(renderer);
	if (isNarrating) narrationLine->Draw(renderer);
}
