#include "pch.h"
#include "Mellee.h"
#include "Bullet.h"
#include "EnemyManager.h"
#include "Player.h"
#include "Level.h"
#include "SoundEffect.h"
#include "ResourceManager.h"

#include <iostream>

Mellee::Mellee(int attackFrames, int walkFrames, int walkfps, int attackfps, std::string soundEffect, float range, float attackAngle)
	: Weapon(attackFrames, walkFrames, walkfps, attackfps, false, soundEffect)
	, m_Range{range}
	, m_AttackAngle{ attackAngle * utils::g_Deg2Rad }
	, m_pHit{ResourceManager::GetSoundEffect("Resources/Audio/Hit.wav")}
{
}

void Mellee::Update(float elapsedSec)
{
	
}

void Mellee::Attack(const Vector2f& pos, float elapsedSec, const Vector2f& forward, Level& level)
{
	PlaySoundEffect(0);

	float minAngle = -forward.AngleWith(Vector2f(1, 0)) - m_AttackAngle;
	float maxAngle = -forward.AngleWith(Vector2f(1, 0)) + m_AttackAngle;

	if (pos == level.GetPlayer()->GetPosition()) 
	{
		if (level.GetEnemyManager()->EnemyHit(pos, minAngle, maxAngle, m_Range, level))
			m_pHit->Play(0);
	}
	else 
	{
		if (AttackPlayer(pos, forward, level, minAngle, maxAngle))
			m_pHit->Play(0);
	}
}

std::string Mellee::ToString() const
{
	std::string s;

	s += "type=\"mellee\" \n";
	s += "attackframes=\"" + std::to_string(GetAttackFrames()) + "\" \n";
	s += "walkframes=\"" + std::to_string(GetWalkFrames()) + "\" \n";
	s += "walkfps=\"" + std::to_string(GetWalkFramesPerSec()) + "\" \n";
	s += "attackfps=\"" + std::to_string(GetAttackFramesPerSec()) + "\" \n";
	s += "soundeffect=\"" + GetSoundEffectName() + "\" \n";
	s += "range=\"" + std::to_string(m_Range) + "\" \n";
	s += "attackangle=\"" + std::to_string(m_AttackAngle * utils::g_Rad2Deg) + "\" \n";

	return s;
}

bool Mellee::AttackPlayer(const Vector2f& pos, const Vector2f& forward, Level& level, float minAngle, float maxAngle)
{
	Player* player = level.GetPlayer();

	if (player->GetState() != Player::PlayerState::Dead)
	{
		Vector2f dir{ (player->GetPosition() - pos) };

		if (utils::GetDistance(player->GetPosition() , pos) < m_Range)
		{
			float angle = -dir.Normalized().AngleWith(Vector2f{ 1,0 });
			if (angle > minAngle && angle < maxAngle)
			{
				player->SetState(Player::PlayerState::Dead);
				return true;
			}
		}
	}
	return false;
}
