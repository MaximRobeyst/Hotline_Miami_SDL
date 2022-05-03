#pragma once
#include "Weapon.h"

class Level;
class SoundEffect;
class Mellee final: public Weapon
{
public:
	Mellee(int attackFrames, int walkFrames, int walkfps, int attackfps, std::string soundEffect, float range, float attackAngle);

	virtual void Update(float elapsedSec) override;
	virtual void Attack(const Vector2f& pos, float elapsedSec, const Vector2f& forward, Level& level) override;
	virtual std::string ToString() const;
	
	bool AttackPlayer(const Vector2f& pos, const Vector2f& forward, Level& level, float minAngle, float maxAngle);
private:
	float m_AttackAngle;
	float m_Range;

	SoundEffect* m_pHit;
};

