#pragma once
#include "Enemy.h"
#include <vector>

class Mobster final: public Enemy
{
public:
	Mobster(const Vector2f& position, float m_Angle, std::vector<Vector2f> path, EnemyState enemyState = EnemyState::Moving);
	Mobster(const Enemy& go) = delete;
	Mobster& operator=(const Enemy& go) = delete;
	Mobster(Mobster&& go) = delete;
	Mobster& operator=(Mobster&& go) = delete;
	virtual ~Mobster();

	virtual void Draw() const override;
	virtual void Update(float elapsedSec, Level& level) override;
	virtual std::string ToString() const override;

	virtual std::vector<Point2f> GetShape() const override;

	void SetCurrentPickup(Pickup* pickup, PickupManager& pickupManger);
	void RemovePickup(PickupManager& pickupManager);

protected:

	virtual void UpdateIdle(float elapsedSec, const Level& level) override;
	virtual void UpdateMoving(float elapsedSec, const Level& level) override;
	virtual void UpdateUnconsious(float elapsedSec, const Level& level) override;
	virtual void UpdateGetUp(float elapsedSec, const Level& level) override;
	virtual void UpdateAttack(float elapsedSec, Level& level) override;
	virtual void UpdateDead(float elapsedSec, Level& level) override;
private:
	Animation* m_pLegAnimation;
	Animation* m_pTorsoAnimation;

	Pickup* m_pCurrentPickup;
};

