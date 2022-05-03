#pragma once
#include <vector>
#include <map>

#include "SoundStream.h"

class Camera;
class Player;
class Level;
class HUD;
class GameObject;
class PickupManager;
class Pickup;
class Texture;

class Game final
{
public:
	enum class GameState {
		Playing,
		MaskSelection,
		EndScreen
	};

	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	~Game();

	void Update( float elapsedSec );
	void Draw( ) const;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e );
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e );
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e );
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e );
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e );

	static HUD* GetHud();
	static const float GetElapsedTime();
	Level* GetLevel();

	static void SetState(Game::GameState state);
	static const Game::GameState GetState();

private:
	// DATA MEMBERS
	const Window m_Window;

	std::string m_FirstLevel{ "Resources/Level.txt" };
	std::string m_LevelLocation{ "Resources/Level.txt" };

	Camera* m_pCamera;
	Level* m_pLevel;
	static HUD* m_pHUD;
	static float m_ElapsedTime;

	Point2f m_MousePosition;
	static GameState m_GameState;
	static bool m_StateChanged;

	int m_Volume{ 128 };

	bool m_Paused{ false };
	bool m_LevelDone{ false };

	Pickup* m_pStartPickup{ nullptr };
	int m_StartScore{ 0 };

	const SoundStream* m_pMusic;

	// FUNCTIONS
	void Initialize( );
	void Cleanup( );
	void ClearBackground( ) const;

	void Reset();
	void LoadLevel(const std::string& filename);

	void NormalUpdate(float elapsedSec);
	void MaskUpdate(float elapsedSec, const Level& level);
};