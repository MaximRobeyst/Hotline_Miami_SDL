#include "pch.h"
#include "Game.h"
#include "Door.h"
#include "Camera.h"
#include "Level.h"
#include "utils.h"
#include "HUD.h"
#include "PickupManager.h"
#include "Player.h"
#include "Enemy.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Pickup.h"
#include "Car.h"

#include <iostream>

HUD* Game::m_pHUD;
float Game::m_ElapsedTime;

Game::GameState Game::m_GameState{ GameState::MaskSelection };
bool Game::m_StateChanged{ false };

Game::Game( const Window& window ) 
	:m_Window{ window }
	,m_pMusic{ new SoundStream("Resources/Audio/MOON_Crystals.mp3")}
{
	Initialize( );
}

Game::~Game( )
{
	Cleanup( );
}

void Game::Initialize( )
{
	m_pCamera = new Camera(m_Window.width, m_Window.height, 3);
	m_pLevel = new Level(m_LevelLocation);
	m_pCamera->SetLevelBoundries(m_pLevel->GetLevelBoundries());
	m_pHUD = new HUD();

	m_pMusic->Play(true);
}

void Game::Cleanup( )
{
	delete m_pCamera;
	delete m_pLevel;
	delete m_pHUD;

	delete m_pStartPickup;
	delete m_pMusic;

	ResourceManager::ClearTextures();
	ResourceManager::ClearSoundEffects();
}

void Game::Update( float elapsedSec )
{
	if (m_StateChanged) 
	{
		switch (m_GameState)
		{
		case GameState::Playing:
			break;
		case GameState::MaskSelection:
			delete m_pMusic;
			m_pMusic = new SoundStream("Resources/Audio/MOON_Crystals.mp3");
			m_pMusic->Play(true);
			m_pMusic->SetVolume(m_Volume);
			break;
		case GameState::EndScreen:
			delete m_pMusic;
			m_pMusic = new SoundStream("Resources/Audio/ScoreScreenMusic.mp3");
			m_pMusic->Play(true);
			m_pMusic->SetVolume(m_Volume);
			break;
		}

		m_StateChanged = false;
	}

	switch (m_GameState)
	{
	case GameState::Playing:
		NormalUpdate(elapsedSec);
		break;
	case GameState::MaskSelection:
		MaskUpdate(elapsedSec, *m_pLevel);
		break;
	case GameState::EndScreen:
		m_pHUD->Update(elapsedSec, *m_pLevel, m_Window);
		break;
	}

	m_ElapsedTime += elapsedSec;
}

void Game::Draw( ) const
{
	ClearBackground( );

	if (m_GameState == GameState::EndScreen) 
	{
		m_pHUD->DrawWindow(m_MousePosition, *m_pLevel, m_Window);
		return;
	}

	glPushMatrix();
	// Camera sets draw position
	m_pCamera->Draw(m_pLevel->GetPlayer()->GetPosition().ToPoint2f());
	m_pLevel->DrawBackground();
	m_pLevel->DrawLayout();
	m_pLevel->DrawProps();
	m_pHUD->DrawOffset(*m_pLevel);
	
	glPopMatrix();
	//Draw HUD
	m_pHUD->DrawWindow(m_MousePosition, *m_pLevel, m_Window);
	
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	m_pLevel->ProcessKeyDownEvent(e);
	m_pHUD->ProcessKeyDownEvent(e, *this);
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	//m_pLevel->ProcessKeyUpEvent(e);

	switch ( e.keysym.sym )
	{
	case SDLK_r:
		if (m_GameState == GameState::EndScreen)
		{
			SetState(GameState::MaskSelection);
			LoadLevel(m_FirstLevel);
		}
		else 
		{
			Reset();
		}
		break;
	case SDLK_t:
		Reset();
		m_GameState = GameState::MaskSelection;
		break;
	case SDLK_i:
		std::cout << std::endl;
		std::cout << "== How to play ==" << std::endl;
		std::cout << "press WASD to move" << std::endl;
		std::cout << "pickup and throw weapons with the left mousebutton (this only turns the enemy unconsious)" << std::endl;
		std::cout << "press right mousebutton to attack(if unarmed you will knock the enemy unconsious)" << std::endl;
		std::cout << "if an enemy is unconsious press space to finish him off" << std::endl;
		std::cout << "Dogs cant become unconsious you need to finish them off imidiatly before they attack you" << std::endl;
		std::cout << "kill all enemies to finish the level and go to the next one" << std::endl;
		std::cout << "control the volume with the up and down arrows" << std::endl;
		std::cout << std::endl;
		break;
	case SDLK_UP:
		if (m_GameState == GameState::MaskSelection)
			break;
		m_Volume+=10;
		m_pMusic->SetVolume(m_Volume);
		ResourceManager::SetVolume(m_Volume);
		break;
	case SDLK_DOWN:
		if (m_GameState == GameState::MaskSelection)
			break;
		m_Volume-=10;
		m_Volume < 0 ? m_Volume =  0 : m_Volume = m_Volume;
		m_pMusic->SetVolume(m_Volume);
		ResourceManager::SetVolume(m_Volume);
		break;
	}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
	m_MousePosition = Point2f((float)e.x, (float)m_Window.height - e.y);
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	m_pLevel->MouseDownEvent(e);
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	m_pLevel->MouseUpEvent(e);
	//std::cout << "MOUSEBUTTONUP event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

HUD* Game::GetHud()
{
	return m_pHUD;
}

const float Game::GetElapsedTime()
{
	return m_ElapsedTime;
}

Level* Game::GetLevel()
{
	return m_pLevel;
}

void Game::SetState(Game::GameState state)
{
	m_StateChanged = true;
	m_GameState = state;
}

const Game::GameState Game::GetState()
{
	return m_GameState;
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::Reset()
{
	m_LevelDone = false;

	delete m_pLevel;
	m_pLevel = new Level(m_LevelLocation);

	m_pHUD->Reset(m_StartScore);

	if(m_pStartPickup != nullptr)
		m_pLevel->GetPlayer()->SetCurrentPickup(new Pickup(*m_pStartPickup), *m_pLevel->GetPickupManager());
	m_pLevel->GetPlayer()->SetMaskRect(m_pHUD->GetMaskIndex());
}

void Game::LoadLevel(const std::string& filename)
{
	*m_pLevel = Level{ filename };

	m_LevelLocation = filename;

	Pickup* pickup = m_pLevel->GetPlayer()->GetCurrentPickup();

	if (m_pStartPickup != nullptr)
	{
		delete m_pStartPickup;
		m_pStartPickup = nullptr;
	}

	if(m_pLevel->GetPlayer()->GetCurrentPickup() != nullptr)
		m_pStartPickup = new Pickup(*m_pLevel->GetPlayer()->GetCurrentPickup());

	m_StartScore = m_pHUD->GetScore();
	m_pLevel->GetPlayer()->SetMaskRect(m_pHUD->GetMaskIndex());
}

void Game::NormalUpdate(float elapsedSec)
{
	float cameraOffset{ 20 };

	const Uint8* pStates = SDL_GetKeyboardState(nullptr);
	if (pStates[SDL_SCANCODE_LSHIFT])
		cameraOffset = 100.f;

	m_pCamera->Update(elapsedSec, (m_pLevel->GetPlayer()->GetPosition() + m_pLevel->GetPlayer()->GetForward() * cameraOffset).ToPoint2f());
	m_pLevel->Update(elapsedSec, *m_pHUD);
	m_pHUD->Update(elapsedSec, *m_pLevel, m_Window);
	m_pLevel->SetMousePosition(m_pCamera->WindowToGameWorld(m_MousePosition, m_pLevel->GetPlayer()->GetPosition().ToPoint2f()));

	if (m_pLevel->LevelComplete())
	{
		LoadLevel(m_pLevel->GetNextLevel());
	}

	if (!m_LevelDone && m_pHUD->GetBottomRightText() == "Go to Car!") 
	{
		delete m_pMusic;
		m_pMusic = new SoundStream("Resources/Audio/EndLevelMusic.mp3");
		m_pMusic->Play(true);
		m_pMusic->SetVolume(m_Volume);

		m_LevelDone = true;
	}
}

void Game::MaskUpdate(float elapsedSec, const Level& level)
{
	m_pCamera->Update(elapsedSec, m_pLevel->GetPlayer()->GetPosition().ToPoint2f());
	m_pHUD->Update(elapsedSec, level, m_Window);
}
