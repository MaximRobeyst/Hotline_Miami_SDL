#pragma once
#include "GameObject.h"

#include <vector>

struct Vector2f;
class Animation;
class Texture;
class Pickup;
class PickupManager;
class Enemy;
class SoundEffect;
class Player final : public GameObject
{
public:
	enum class PlayerState {
		Idle,
		Moving,
		Attack,
		Dead,
		TakeDown
	};

	//constructor & destructor
	explicit Player(const Vector2f& position);
	Player(const Player& other) = delete;
	Player& operator=(const Player& other) = delete;
	Player(Player&& other) = delete;
	Player& operator=(Player&& other) = delete;
	~Player();

	//Overrirden functions
	virtual void Draw() const override;
	virtual void Update(float elapsedSec, Level& level) override;
	virtual void HandleCollision(const utils::HitInfo& hit) override;
	virtual std::string ToString() const override;

	//Input functions
	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e, Level& level);
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e);
	void SetMousePosition(const Point2f& mousePosition);
	void MouseDownEvent(const SDL_MouseButtonEvent& e, Level& level);
	void MouseUpEvent(const SDL_MouseButtonEvent& e);

	bool IsOverlapping(std::vector<Point2f> p);

	//Public Get&Set functions
	virtual std::vector<Point2f> GetShape() const override;
	PlayerState GetState() const;
	Pickup* GetCurrentPickup() const;

	void SetState(PlayerState playerState);
	void SetCurrentPickup(Pickup* pickup, PickupManager& pickupManger);
	void SetMaskRect(int maskIndex);
	void SetPosition(const Vector2f& pos);

	Vector2f GetForward() const;

	int m_GetSchapeCalls{};
private:
	Texture* m_pMaskTexture;
	Animation* m_pLegAnimation;
	Animation* m_pTorsoAnimation;
	Animation* m_pTakeDownAnim;

	Enemy* m_pTakedownTarget;

	PlayerState m_Playerstate{ PlayerState::Idle };

	//Vector2f m_Velocity;
	Point2f m_MousePosition;
	float m_Speed{200.0f};
	float m_ThrowForce{ 1000 };

	Pickup* m_pCurrentPickup{nullptr};

	float m_AttackSec{};
	float m_MaxAttackSec{};

	// Punch
	float m_PunchAngle{45 * utils::g_Deg2Rad};
	float m_PunchRange{50};
	int m_Bashes{};
	int m_MaxBashes{ 3 };

	bool m_HoldingLeftMouse{ false };
	bool m_InAnimation{ false };
	bool m_LocalMovement{ false };

	//Sound Effects
	SoundEffect* m_pThrowSound;
	SoundEffect* m_pSwingSound;
	SoundEffect* m_pPunchSound;

	//Mask
	int m_MaskIndex{};
	Rectf m_MaskSrcRect{};

	//Helper Fucntions
	Vector2f GetRight() const;

	bool GetMovementInput(float elapsedSec);

	void UpdateIdle(float elapsedSec, const Level& level);
	void UpdateMoving(float elapsedSec, const Level& level);
	void UpdateAttack(float elapsedSec, Level& level);
	void UpdateDead(float elapsedSec, Level& level);
	void UpdateTakeDown(float elapsedSec, Level& level);

	void AttackWeapon(float elapsedSec, Level& level);
	void AttackUnarmed(float elapsedSec, Level& level);

	void ThrowPickup(PickupManager& pickupManager, float throwForce);
};

