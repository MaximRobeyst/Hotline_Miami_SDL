#include "pch.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "Level.h"
#include "PickupManager.h"
#include "Player.h"
#include "ResourceManager.h"
#include "SoundEffect.h"
#include "Game.h"
#include "HUD.h"

#include <iostream>

EnemyManager::~EnemyManager()
{
	for (Enemy* element : m_pEnemies) 
	{
		delete element;
	}
	m_pEnemies.clear();
}

Enemy* EnemyManager::AddEnemy(Enemy* enemy)
{
	m_pEnemies.push_back(enemy);
	if (!enemy->IsDead()) m_StageClear = false;

	return m_pEnemies[m_pEnemies.size() - 1];
}

void EnemyManager::Update(float elapsedSec, Level& level)
{
	if (m_pEnemies.size() <= 0)
		return;

	int dead{ 0 };
	
	for (Enemy* element : m_pEnemies)
	{
		element->Update(elapsedSec, level);
		level.GetPickupManager()->HitItem(*element);
	
		if (element->IsDead()) dead++;
	}

	if (m_StageClear)
		return;
	if (dead >= (int)m_pEnemies.size()) 
	{
		m_StageClear = true;
		ResourceManager::GetSoundEffect("Resources/Audio/Complete.wav")->Play(0);
		Game::GetHud()->SetStageClear();
		if (level.GetNextLevel().find("-save") != std::string::npos) 
		{
			Game::GetHud()->SetBottomRightText("Go to car!");
		}
	}
}

void EnemyManager::Draw() const
{
	for (Enemy* element : m_pEnemies)
	{
		element->Draw();
	}
}

bool EnemyManager::EnemyHit(std::vector<Point2f> shape, bool stateImportant)
{
	int i = 0;
	for (Enemy* element : m_pEnemies)
	{
		if (element->IsOverlapping(shape, stateImportant))
		{
			//element->SetEnemyState(switchState);
			Swap(i, (int)m_pEnemies.size() - 1);
			return true;
		}
		i++;
	}
	return false;

}

bool EnemyManager::EnemyHit(const Vector2f& startpos, float minangle, float maxAngle, float distance, const Level& level, Enemy::EnemyState switchState)
{
	for (Enemy* element : m_pEnemies)
	{
		if (element->GetEnemyState() != Enemy::EnemyState::Dead && element->GetEnemyState() != Enemy::EnemyState::Unconsious) 
		{
			Vector2f dir{ (element->GetPosition() - startpos) };
			if ((element->GetPosition() - startpos).Length() < distance)
			{
				float angle = -dir.Normalized().AngleWith(Vector2f{ 1,0 });
				if (angle > minangle&& angle < maxAngle)
				{
					if (!level.Raycast(element->GetPosition().ToPoint2f(), startpos.ToPoint2f())) 
					{
						element->SetEnemyState(switchState);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool EnemyManager::Bullet(const Vector2f& startpos, const Vector2f& velocity, float distance, const Level& level, Enemy::EnemyState switchState)
{
	for (Enemy* element : m_pEnemies)
	{
		if (element->GetEnemyState() != Enemy::EnemyState::Dead && element->GetEnemyState() != Enemy::EnemyState::Unconsious)
		{
			float enemydistance{ utils::GetDistance(element->GetPosition(), startpos) };
			if (enemydistance < distance) 
			{
				if (!level.Raycast(startpos.ToPoint2f(), (startpos + (velocity * enemydistance)).ToPoint2f(), true))
				{
					utils::HitInfo hit;
					if (utils::Raycast(element->GetShape(), startpos.ToPoint2f(), (startpos + (velocity * enemydistance)).ToPoint2f(), hit))
					{
						element->SetEnemyState(switchState);
						return true;
					}
				}
			}
		}

	}

	return false;
}

Enemy* EnemyManager::GetHitItem() const
{
	return m_pEnemies[m_pEnemies.size() - 1];
}

bool EnemyManager::GetStageClear() const
{
	return m_StageClear;
}

std::string EnemyManager::ToString() const
{
	std::string s;

	for (Enemy* e : m_pEnemies)
	{
		s += '\n';
		s += e->ToString();
		s += '\n';
	}

	return s;
}

void EnemyManager::Swap(int id1, int id2)
{
	Enemy* tmp = m_pEnemies[id1];
	m_pEnemies[id1] = m_pEnemies[id2];
	m_pEnemies[id2] = tmp;
}
