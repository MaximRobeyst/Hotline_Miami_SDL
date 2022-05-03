#include "pch.h"
#include "Dog.h"
#include "Animation.h"
#include "Matrix2x3.h"
#include "Level.h"
#include "HUD.h"
#include "Game.h"
#include "Player.h"
#include <iostream>

Dog::Dog(const Vector2f& position, float m_Angle, std::vector<Vector2f> path, EnemyState enemyState)
	: Enemy(position, m_Angle, path, enemyState)
	, m_pDogAnimation{new Animation("Resources/Anim/Dog_Animation.txt")}
{
	if (m_Points[(m_PointIndex + 1) % (int)m_Points.size()] == m_Points[m_PointIndex] && enemyState != EnemyState::Dead) m_EnemyState = Enemy::EnemyState::Idle;
	if (enemyState == EnemyState::Dead) m_Died = true;

	m_UnconsiousScore = 0;
}

Dog::~Dog()
{
	delete m_pDogAnimation;
}

void Dog::Draw() const
{
	glPushMatrix();
	// Set translation, scale and rotation of the texture
	glTranslatef(m_Position.x, m_Position.y, 0);
	glRotatef(m_Angle * utils::g_Rad2Deg, 0, 0, 1);
	glScalef(m_Scale, m_Scale, 0);
	//draw the enemy legs
	glTranslatef(-m_pDogAnimation->GetFrameWidth() / 2, -m_pDogAnimation->GetFrameHeight() / 2, 0);
	m_pDogAnimation->Draw();
	glTranslatef(m_pDogAnimation->GetFrameWidth() / 2, m_pDogAnimation->GetFrameHeight() / 2, 0);
	glPopMatrix();
}

void Dog::Update(float elapsedSec, Level& level)
{
	if (m_Died)
	{
		m_pDogAnimation->ResetAnimation();
		m_pDogAnimation->SetStartPos(Point2f(0, m_pDogAnimation->GetFrameHeight() * 2));
		m_EnemyState = EnemyState::Dead;
		return;
	}

	switch (m_EnemyState)
	{
	case Enemy::EnemyState::Idle:
		UpdateIdle(elapsedSec, level);
		break;
	case Enemy::EnemyState::Moving:
		UpdateMoving(elapsedSec, level);
		break;
	case Enemy::EnemyState::Unconsious:
		UpdateUnconsious(elapsedSec, level);
		break;
	case Enemy::EnemyState::Getup:
		UpdateGetUp(elapsedSec, level);
		break;
	case Enemy::EnemyState::Attack:
		UpdateAttack(elapsedSec, level);
		break;
	case Enemy::EnemyState::Dead:
		UpdateDead(elapsedSec, level);
		break;
	case Enemy::EnemyState::Takeout:
		m_Score = 1000;
		break;
	}

	level.HandleCollision(*this);
	m_Position += m_Velocity;
}

std::vector<Point2f> Dog::GetShape() const
{
	Matrix2x3 matTrans, matAngle, matScale, matCenter, matWorld;
	matTrans.SetAsTranslate(m_Position);
	matAngle.SetAsRotate(m_Angle * (180.0f / (float)M_PI));
	matScale.SetAsScale(m_Scale);
	matCenter.SetAsTranslate(Vector2f{ -m_pDogAnimation->GetFrameWidth() / 2, -m_pDogAnimation->GetFrameHeight() / 2 });

	matWorld = matTrans * matAngle * matScale * matCenter;

	std::vector<Point2f> points;
	points = matWorld.Transform(Rectf{ 0, 0, m_pDogAnimation->GetFrameWidth(), m_pDogAnimation->GetFrameHeight() });

	return points;
}

std::string Dog::ToString() const
{
	std::string s;

	s += "<Dog\n";
	s += "position=\"" + std::to_string(m_Position.x) + "," + std::to_string(m_Position.y) + "\" \n";
	s += "angle=\"" + std::to_string(m_Angle * utils::g_Rad2Deg) + "\" \n";

	s += "path=\"";
	for (Vector2f p : m_Points) s += ' ' + std::to_string(p.x) + ',' + std::to_string(p.y);
	s += "\" \n";
	s += "enemystate=\"" + std::to_string((int)GetEnemyState()) + "\" \n";

	s += "/>";

	return s;
}

void Dog::UpdateIdle(float elapsedSec, const Level& level)
{
	m_Angle = m_IdleAngle;
	m_Velocity = Vector2f{ 0,0 };

	if (FoundPlayer(level.GetPlayer(), level))
		m_EnemyState = EnemyState::Attack;
	else
		m_EnemyState = EnemyState::Moving;
}

void Dog::UpdateMoving(float elapsedSec, const Level& level)
{
	float minDistance{ 5 };

	Vector2f target{};
	if (m_Points[(m_PointIndex + 1) % (int)m_Points.size()] == m_Points[m_PointIndex]) m_EnemyState = Enemy::EnemyState::Idle;
	else target = m_Points[m_PointIndex];

	m_Velocity = (target - m_Position).Normalized() * m_Speed * elapsedSec;
	m_Angle = atan2f(m_Velocity.y, m_Velocity.x);

	if (utils::GetDistance(m_Position, m_Points[m_PointIndex]) < minDistance)
	{
		if (m_Points[(m_PointIndex + 1) % (int)m_Points.size()] == m_Points[m_PointIndex]) m_EnemyState = Enemy::EnemyState::Idle;
		m_PointIndex = (m_PointIndex + 1) % (int)m_Points.size();
	}

	if (m_Velocity.SquaredLength() > 0)
	{
		m_pDogAnimation->Update(elapsedSec);
	}

	if (FoundPlayer(level.GetPlayer(), level)) m_EnemyState = Enemy::EnemyState::Attack;
}

void Dog::UpdateUnconsious(float elapsedSec, const Level& level)
{
	m_EnemyState = EnemyState::Moving;
}

void Dog::UpdateGetUp(float elapsedSec, const Level& level)
{
}

void Dog::UpdateAttack(float elapsedSec, Level& level)
{
	m_AttackTimer += elapsedSec;
	m_Speed = 200.f;
	m_pDogAnimation->SetStartPos(Point2f(0, m_pDogAnimation->GetFrameHeight()));
	m_pDogAnimation->Update(elapsedSec, 6);

	m_Velocity = (m_pPlayer->GetPosition() - m_Position).Normalized() * m_Speed * elapsedSec;
	m_Angle = atan2f(m_Velocity.y, m_Velocity.x);

	if (m_AttackTimer < m_AttackWait)
		return;

	if (m_pPlayer->IsOverlapping(GetShape()))
		m_pPlayer->SetState(Player::PlayerState::Dead);
	// TODO: chase after player if in min distance attack

	float distanceToPlayer = utils::GetDistance(m_Position, m_pPlayer->GetPosition());

	if (m_pPlayer->GetState() == Player::PlayerState::Dead)
	{
		m_pDogAnimation->SetStartPos(Point2f(0, 0));
		m_Speed = 100;

		m_Timer = 0;
		m_EnemyState = EnemyState::Moving;
	}
}

void Dog::UpdateDead(float elapsedSec, Level& level)
{
	if (m_Died)
		return;

	Game::GetHud()->AddHit(m_Score, m_Position.ToPoint2f());

	m_Died = true;
}
