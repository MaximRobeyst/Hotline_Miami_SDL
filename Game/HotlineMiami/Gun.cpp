#include "pch.h"
#include "Gun.h"
#include "Level.h"
#include "utils.h"
#include "Bullet.h"
#include "utils.h"
#include "Weapon.h"
#include "Game.h"
#include "HUD.h"
#include "Player.h"
#include "EnemyManager.h"

#include <iostream>

Gun::Gun(int attackFrames, int walkFrames, int walkfps, int attackfps, std::string soundEffect, int clip, int ammo, float range, float fireRate, float spreadAngle, int spreadPieces, float bulletOffset)
	: Weapon(attackFrames, walkFrames, walkfps, attackfps, true, soundEffect)
	, m_Clip{clip}
	, m_Ammo{ammo}
	, m_Range{range}
	, m_FireRate{fireRate}
	, m_SpreadAngle{spreadAngle * utils::g_Deg2Rad}
	, m_SpreadPieces{spreadPieces}
	, m_BulletOffset{bulletOffset}
{
}

void Gun::Update(float elapsedSec)
{
	m_TimeToShoot -= elapsedSec * m_FireRate;
}

void Gun::Attack(const Vector2f& pos, float elapsedSec, const Vector2f& forward, Level& level)
{

	if (m_Clip <= 0) return;

	level.SetCurrentColor(Color4f{ 0.95f, 0.8f, 0.4f, 1}, 0.1f);
	PlaySoundEffect(0);
	m_Clip--;
	if (pos == level.GetPlayer()->GetPosition())
		Game::GetHud()->SetBottomRightText(DisplayAmmo());


	for (int i = 0; i < m_SpreadPieces; i++) 
	{
		float spreadAngleDeg = m_SpreadAngle * utils::g_Rad2Deg;
		float randomAngle = (-spreadAngleDeg + rand() % (int)(spreadAngleDeg * 2)) * utils::g_Deg2Rad;

		float angle = -forward.AngleWith(Vector2f(1, 0)) + randomAngle;
		Vector2f dir = Vector2f{ cosf(angle), sinf(angle) };

		Vector2f bulletPos = pos + (forward * m_BulletOffset);

		if (pos == level.GetPlayer()->GetPosition())
			level.GetEnemyManager()->Bullet(pos, dir, m_Range, level);
		else
			HitPlayer(pos, dir, m_Range, level);

		if (!level.Raycast(pos.ToPoint2f(), (pos + forward * m_BulletOffset).ToPoint2f()))
			level.AddProp(new Bullet(bulletPos,elapsedSec, dir * m_Range));
	}
	m_TimeToShoot = 1;
}

std::string Gun::DisplayAmmo()
{
	return std::to_string(m_Clip) + "/" + std::to_string(m_Ammo);
}

bool Gun::HitPlayer(const Vector2f& startpos, const Vector2f& velocity, float distance, const Level& level, Player::PlayerState switchState)
{
	Player* player = level.GetPlayer();

	if (player->GetState() != Player::PlayerState::Dead)
	{
		float enemydistance{ utils::GetDistance(player->GetPosition(), startpos) };
		if (enemydistance < distance)
		{
			if (!level.Raycast(startpos.ToPoint2f(), (startpos + (velocity * enemydistance)).ToPoint2f(), true))
			{
				utils::HitInfo hit;
				if (utils::Raycast(player->GetShape(), startpos.ToPoint2f(), (startpos + (velocity * enemydistance)).ToPoint2f(), hit))
				{
					player->SetState(switchState);
					return true;
				}
			}
		}
	}

	return false;
}

std::string Gun::ToString() const
{
	std::string s;

	s += "type=\"gun\" \n";
	s += "attackframes=\"" + std::to_string(GetAttackFrames()) + "\" \n";
	s += "walkframes=\"" + std::to_string(GetWalkFrames()) + "\" \n";
	s += "walkfps=\"" + std::to_string(GetWalkFramesPerSec()) + "\" \n";
	s += "attackfps=\"" + std::to_string(GetAttackFramesPerSec()) + "\" \n";
	s += "soundeffect=\"" + GetSoundEffectName() + "\" \n";
	s += "clip=\"" + std::to_string(m_Clip) + "\" \n";
	s += "ammo=\"" + std::to_string(m_Ammo) + "\" \n";
	s += "range=\"" + std::to_string(m_Range) + "\" \n";
	s += "firerate=\"" + std::to_string(m_FireRate) + "\" \n";
	s += "spreadangle=\"" + std::to_string(m_SpreadAngle * utils::g_Rad2Deg) + "\" \n";
	s += "spreadpieces=\"" + std::to_string(m_SpreadPieces) + "\" \n";
	s += "bulletoffset=\"" + std::to_string(m_BulletOffset) + "\" \n";

	return s;
}

