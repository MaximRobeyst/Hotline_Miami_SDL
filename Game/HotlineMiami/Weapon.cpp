#include "pch.h"
#include "Weapon.h"
#include "Level.h"
#include "SoundEffect.h"
#include "ResourceManager.h"

Weapon::Weapon(int attackFrames, int walkFrames, int walkfps, int attackfps, bool gun,std::string soundEffect)
	: m_AttackFrames{attackFrames}
	, m_WalkFrames{walkFrames}
	, m_WalkFramesPerSec{walkfps}
	, m_AttackFramesPerSec{attackfps}
	, m_Gun{gun}
	, m_SoundEffectName{soundEffect}
	, m_pSoundEffect{ResourceManager::GetSoundEffect(soundEffect)}
{
}

int Weapon::GetAttackFrames() const
{
	return m_AttackFrames;
}

int Weapon::GetWalkFrames() const
{
	return m_WalkFrames;
}

int Weapon::GetWalkFramesPerSec() const
{
	return m_WalkFramesPerSec;
}

int Weapon::GetAttackFramesPerSec() const
{
	return m_AttackFramesPerSec;
}

bool Weapon::IsGun() const
{
	return m_Gun;
}

std::string Weapon::GetSoundEffectName() const
{
	return m_SoundEffectName;
}

void Weapon::PlaySoundEffect(int loops) const
{
	m_pSoundEffect->Play(loops);
}
