#pragma once
#include "GameObject.h"
#include <vector>

class Animation;
class Pickup;
class Player;
class PickupManager;
class Enemy: public GameObject
{
public:
	enum class EnemyState {
		Idle,
		Moving,
		NextWaypoint,
		Unconsious,
		Dead,
		Getup,
		Attack,
		Takeout
	};

	Enemy(const Vector2f& position, float m_Angle, std::vector<Vector2f> path, EnemyState enemyState = EnemyState::Moving);

	virtual void Draw() const = 0;
	virtual void Update(float elapsedSec, Level& level) = 0;
	virtual void HandleCollision(const utils::HitInfo& hit) override;
	virtual std::string ToString() const override;

	bool IsOverlapping(std::vector<Point2f> p, bool stateImportant) const;

	//Public Get&Set functions
	virtual std::vector<Point2f> GetShape() const override;

	void SetEnemyState(EnemyState enemyState);
	Enemy::EnemyState GetEnemyState() const;
	bool IsDead() const;

	Vector2f GetForward() const;
	Vector2f GetRight() const;

protected:
	std::vector<Vector2f> m_Points;
	int m_PointIndex;

	EnemyState m_EnemyState{ EnemyState::Moving };

	float m_Speed{ 100 };
	float m_FieldOfView{ 90 * utils::g_Deg2Rad };
	float m_AttackWait{ 0.2f };
	int m_Score;
	bool m_Died{ false };

	float m_Timer{};
	float m_AttackTimer{};
	float m_UnconsiousTime{ 4 };
	float m_IdleAngle;
	
	float m_UnconsiousScore{ 300 };

	Player* m_pPlayer;

	bool FoundPlayer(Player* p, const Level& level);

	virtual void UpdateIdle(float elapsedSec, const Level& level) = 0;
	virtual void UpdateMoving(float elapsedSec, const Level& level) = 0;
	virtual void UpdateUnconsious(float elapsedSec, const Level& level) = 0;
	virtual void UpdateGetUp(float elapsedSec, const Level& level) = 0;
	virtual void UpdateAttack(float elapsedSec, Level& level) = 0;
	virtual void UpdateDead(float elapsedSec, Level& level) = 0;

private:

};

