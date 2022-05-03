#include "pch.h"
#include "PickupManager.h"
#include "Player.h"
#include "Enemy.h"
#include "Game.h"
#include "HUD.h"
#include <iostream>

PickupManager::~PickupManager()
{
	for (Pickup* element : m_pItems) 
	{
		delete element;
	}

	m_pItems.clear();
}

Pickup* PickupManager::AddItem(Pickup* pickup)
{
	m_pItems.push_back(pickup);
	return m_pItems[m_pItems.size() - 1];
}

Pickup* PickupManager::AddItem(Pickup* pickup, const Vector2f& pos, const Vector2f& v)
{
	m_pItems.push_back(pickup);

	pickup->SetPosition(pos);
	pickup->SetVelocity(v);

	return m_pItems[m_pItems.size() - 1];
}

void PickupManager::Update(float elapsedSec, Level &level)
{
	for (Pickup* element : m_pItems) 
	{
		element->Update(elapsedSec, level);
	}
}

void PickupManager::Draw() const
{
	for (Pickup* element : m_pItems)
	{
		element->Draw();
	}
}

int PickupManager::Size() const
{
	return (int)m_pItems.size();
}

bool PickupManager::HitItem(std::vector<Point2f> shape)
{
	int i{};
	for (Pickup* element : m_pItems)
	{
		if (element->IsOverlapping(shape))
		{
			Swap(i, (int)Size() - 1);
	
			return true;
		}
	
		++i;
	}

	return false;
}

bool PickupManager::HitItem(Enemy& enemy)
{
	int i{};
	for (Pickup* element : m_pItems)
	{
		if (enemy.IsOverlapping(element->GetShape(), true) && element->GetVelocity().SquaredLength() >= 1)
		{
			enemy.SetEnemyState(Enemy::EnemyState::Unconsious);
			return true;
		}

		++i;
	}

	return false;
	
}

Pickup* PickupManager::GetCollidedItem()
{
	Pickup* pickup = m_pItems[m_pItems.size() - 1];
	m_pItems.pop_back();

	return pickup;
}

Pickup* PickupManager::ReturnClosest(const Vector2f& refPoint)
{
	Pickup* closestPickup{ m_pItems[0] };
	float closestDistance{ utils::GetSquaredDistance(closestPickup->GetPosition(), refPoint) };

	for (Pickup* element : m_pItems)
	{
		float newDistance = utils::GetSquaredDistance(element->GetPosition(), refPoint);

		if (newDistance < closestDistance) 
		{
			closestDistance = newDistance;
			closestPickup = element;
		}
	}

	return closestPickup;
}

std::string PickupManager::ToString() const
{
	std::string s;

	for (Pickup* p : m_pItems) 
	{
		s += '\n';
		s += p->ToString();
		s += '\n';
	}

	return s;
}

void PickupManager::Swap(int id1, int id2)
{
	Pickup* temp = m_pItems[id1];
	m_pItems[id1] = m_pItems[id2];
	m_pItems[id2] = temp;
}
