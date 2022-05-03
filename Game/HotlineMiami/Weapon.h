#pragma once
#include "Vector2f.h"

class Level;
class SoundEffect;
class Weapon
{
public:
	Weapon(int attackFrames, int walkFrames, int walkfps, int attackfps, bool gun, std::string soundEffect);

	virtual void Update(float elapsedSec) = 0;
	virtual void Attack(const Vector2f& pos, float elapsedSec, const Vector2f& forward, Level& level) = 0;
	virtual std::string ToString() const = 0;

	int GetAttackFrames() const;
	int GetWalkFrames() const;

	int GetWalkFramesPerSec() const;
	int GetAttackFramesPerSec() const;

	bool IsGun() const;
	std::string GetSoundEffectName() const;

	void PlaySoundEffect(int loops) const;

private:
	int m_AttackFrames;
	int m_WalkFrames;

	int m_WalkFramesPerSec;
	int m_AttackFramesPerSec;
	bool m_Gun;

	std::string m_SoundEffectName;
	SoundEffect* m_pSoundEffect;
};

