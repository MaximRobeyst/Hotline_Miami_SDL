#pragma once
#include "Vector2f.h"
#include "Weapon.h"
#include "Level.h"
#include "Player.h"

class Gun final : public Weapon
{
public:
	Gun(int attackFrames, int walkFrames, int walkfps, int attackfps, std::string soundEffect, int clip, int ammo, float range, float m_FireRate,float spreadAngle, int spreadPieces, float bulletOffset);

	virtual void Update(float elapsedSec) override;
	virtual void Attack(const Vector2f& pos, float elapsedSec, const Vector2f& forward, Level& level) override;
	virtual std::string ToString() const override;

	std::string DisplayAmmo();
private:
	int m_Clip;
	int m_Ammo;
	float m_Range;
	float m_FireRate;
	float m_SpreadAngle;
	int m_SpreadPieces;
	float m_BulletOffset;

	float m_TimeToShoot{ 0 };

	bool HitPlayer(const Vector2f& startpos, const Vector2f& velocity, float distance, const Level& level, Player::PlayerState switchState = Player::PlayerState::Dead);
};

