#include "pch.h"
#include "Bullet.h"
#include "Animation.h"
#include "Texture.h"
#include "Level.h"
#include "EnemyManager.h"
#include "Player.h"
#include "Game.h"

#include <iostream>
#include "Matrix2x3.h"
#include "ResourceManager.h"

Bullet::Bullet(const Vector2f& pos, float elapsedsec, const Vector2f& v, float lifeTime)
	: GameObject(pos)
	, m_pAnimation{ new Animation(ResourceManager::GetTexture("Resources/Images/Bullet.png"), Rectf{0,0,16,3}, 4, 4)}
	, m_Lifetime{2}
{
	m_Velocity = v * elapsedsec;
	m_Angle = atan2f(m_Velocity.Normalized().y, m_Velocity.Normalized().x);
}

Bullet::~Bullet()
{
	delete m_pAnimation;
}

void Bullet::Draw() const
{
	glPushMatrix();
		glTranslatef(m_Position.x , m_Position.y, 0);
		glRotatef(m_Angle * utils::g_Rad2Deg, 0, 0, 1);
		m_pAnimation->Draw();
	glPopMatrix();
}

void Bullet::Update(float elapsedSec, Level& level)
{
	m_Timer += elapsedSec;

	//std::vector<Point2f> shape = GetShape();
	//if (level.GetPlayer()->IsOverlapping(shape)) 
	//{
	//	level.GetPlayer()->SetState(Player::PlayerState::Dead);
	//	level.DeleteProp(this);
	//	return;
	//}
	//
	//if (m_Remove || m_Timer >= m_Lifetime || level.GetEnemyManager()->EnemyHit(shape))
	//{
	//	if (level.GetEnemyManager()->EnemyHit(shape)) 
	//	{
	//		if(level.Raycast(m_Position.ToPoint2f(), level.GetEnemyManager()->GetHitItem()->GetPosition().ToPoint2f()))
	//			level.GetEnemyManager()->GetHitItem()->SetEnemyState(Enemy::EnemyState::Dead);
	//	}
	//
	//	level.DeleteProp(this);
	//	return;
	//}

	if (level.Raycast(m_Position.ToPoint2f(), (m_Position + m_Velocity).ToPoint2f()))
	{
		level.DeleteProp(this);
		return;
	}

	if (m_Remove) 
	{
		level.DeleteProp(this);
		return;
	}

	level.HandleCollision(*this);

	m_Position += m_Velocity;
}

void Bullet::HandleCollision(const utils::HitInfo& hit)
{
	m_Remove = true;
	//Instatiate bullet puff
}

std::string Bullet::ToString() const
{
	return std::string();
}

std::vector<Point2f> Bullet::GetShape() const
{
	Matrix2x3 matTrans, matAngle, matScale, matCenter, matWorld;
	matTrans.SetAsTranslate(m_Position);
	matAngle.SetAsRotate(m_Angle * (180.0f / (float)M_PI));
	matScale.SetAsScale(m_Scale);
	matCenter.SetAsTranslate(Vector2f{ -m_pAnimation->GetFrameWidth() / 2, -m_pAnimation->GetFrameHeight() / 2 });

	matWorld = matTrans * matAngle * matScale * matCenter;

	std::vector<Point2f> points;
	points = matWorld.Transform(Rectf{ 0, 0, m_pAnimation->GetFrameWidth(), m_pAnimation->GetFrameHeight() });

	return points;
}
