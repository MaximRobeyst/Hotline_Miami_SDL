#include "pch.h"
#include "Enemy.h"
#include "Animation.h"
#include "Texture.h"
#include "utils.h"
#include "Pickup.h"
#include <iostream>
#include "Level.h"
#include "Player.h"
#include "PickupManager.h"
#include "Bloodpool.h"
#include "Matrix2x3.h"
#include "Weapon.h"
#include "ResourceManager.h"
#include "Game.h"
#include "HUD.h"

Enemy::Enemy(const Vector2f& position, float angle, std::vector<Vector2f> path, EnemyState enemyState)
	: GameObject(position, angle)
	, m_PointIndex{}
	, m_Points{path}
	, m_EnemyState{enemyState}
	, m_pPlayer{nullptr}
	, m_IdleAngle{angle}
	, m_Score{600}
{
	if (m_Points[(m_PointIndex + 1) % (int)m_Points.size()] == m_Points[m_PointIndex] && enemyState != EnemyState::Dead) m_EnemyState = Enemy::EnemyState::Idle;
	if (enemyState == EnemyState::Dead) m_Died = true;
}

void Enemy::HandleCollision(const utils::HitInfo& hit)
{
	m_Position -= m_Velocity.Normalized() * hit.lambda;
	m_Velocity = m_Velocity.Normalized() * hit.lambda;
}

std::string Enemy::ToString() const
{
	std::string s;

	s += "<Enemy\n";
	s += "position=\"" + std::to_string(m_Position.x) + "," + std::to_string(m_Position.y) + "\" \n";
	s += "angle=\"" + std::to_string(m_Angle * utils::g_Rad2Deg) + "\" \n";

	s += "path=\"";
	for (Vector2f p : m_Points) s += ' ' + std::to_string(p.x) + ',' + std::to_string(p.y);
	s += "\" \n";
	s += "enemystate=\"" + std::to_string((int)m_EnemyState) + "\" \n";

	s += "/>";

	return s;
}

bool Enemy::IsOverlapping(std::vector<Point2f> p, bool stateImportant) const
{
	if (stateImportant) 
	{
		if (m_EnemyState == EnemyState::Dead || m_EnemyState == EnemyState::Unconsious || m_EnemyState == EnemyState::Takeout)
			return false;
	}

	std::vector<Point2f> shape = GetShape();
	for (Point2f point : p)
	{
		if (utils::IsPointInPolygon(point, shape))
		{
			return true;
		}
	}
	return false;

}

std::vector<Point2f> Enemy::GetShape() const
{
	std::vector<Point2f> points;
	return points;
}

void Enemy::SetEnemyState(EnemyState enemyState)
{
	if(enemyState == EnemyState::Unconsious && m_UnconsiousScore > 0)
		Game::GetHud()->AddHit(300, m_Position.ToPoint2f());

	m_EnemyState = enemyState;
}

Enemy::EnemyState Enemy::GetEnemyState() const
{
	return m_EnemyState;
}

bool Enemy::IsDead() const
{
	return m_Died;
}

Vector2f Enemy::GetForward() const
{
	return Vector2f(cosf(m_Angle), sinf(m_Angle));
}

Vector2f Enemy::GetRight() const
{
	return Vector2f(cosf(m_Angle + (float)(M_PI / 2)), sinf(m_Angle + (float)(M_PI / 2)));
}

bool Enemy::FoundPlayer(Player* p, const Level& level)
{
	if (p->GetState() == Player::PlayerState::Dead)
		return false;

	Vector2f playerPos = p->GetPosition();

	float angle = GetForward().AngleWith(playerPos - m_Position);

	if (angle < m_FieldOfView && angle > -m_FieldOfView)
	{
		m_pPlayer = level.GetPlayer();
		return !level.Raycast(m_Position.ToPoint2f(), p->GetPosition().ToPoint2f());
	}

	return false;
}
