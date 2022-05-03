#pragma once
#include "Enemy.h"

class Animation;
class Dog final : public Enemy
{
public:
	Dog(const Vector2f& position, float m_Angle, std::vector<Vector2f> path, EnemyState enemyState = EnemyState::Moving);
	Dog(const Dog& go) = delete;
	Dog& operator=(const Dog& go) = delete;
	Dog(Dog&& go) = delete;
	Dog& operator=(Dog&& go) = delete;
	~Dog();

	virtual void Draw() const override;
	virtual void Update(float elapsedSec, Level& level) override;
	virtual std::vector<Point2f> GetShape() const override;
	virtual std::string ToString() const override;

protected:

	virtual void UpdateIdle(float elapsedSec, const Level& level) override;
	virtual void UpdateMoving(float elapsedSec, const Level& level) override;
	virtual void UpdateUnconsious(float elapsedSec, const Level& level) override;
	virtual void UpdateGetUp(float elapsedSec, const Level& level) override;
	virtual void UpdateAttack(float elapsedSec, Level& level) override;
	virtual void UpdateDead(float elapsedSec, Level& level) override;
private:
	Animation* m_pDogAnimation;

};

