#include "pch.h"
#include "Door.h"
#include "utils.h"
#include "Player.h"
#include "Level.h"
#include "Texture.h"
#include "EnemyManager.h"
#include "Game.h"
#include "SoundEffect.h"

#include <iostream>
#include "ResourceManager.h"

Door::Door(const Rectf& doorRect, float angle)
	: GameObject{ Vector2f{doorRect.left, doorRect.bottom}, angle}
	, m_pDoorTexture{ ResourceManager::GetTexture("Resources/Images/Door.png")}
	, m_DoorRect{ 0, 0, doorRect.width, doorRect.height}
{
}

void Door::Draw() const
{
	glPushMatrix();
		glTranslatef(m_Position.x , m_Position.y, 0);
		glRotatef(m_Angle * utils::g_Rad2Deg, 0, 0, 1);
		m_pDoorTexture->Draw();
	glPopMatrix();
}

void Door::Update(float elapsedSec, Level& level)
{
	if (utils::IsOverlapping(m_DoorPoints, Circlef{ level.GetPlayer()->GetPosition().ToPoint2f(), 5.f })) 
	{
		m_Angle = atan2f(level.GetPlayer()->GetPosition().y - m_Position.y, level.GetPlayer()->GetPosition().x - m_Position.x);
		if (level.GetPlayer()->GetVelocity().SquaredLength() > 0) 
		{
			if (level.GetEnemyManager()->EnemyHit(m_DoorPoints))
			{
				level.GetEnemyManager()->GetHitItem()->SetEnemyState(Enemy::EnemyState::Unconsious);
				ResourceManager::GetSoundEffect("Resources/Audio/Punch.wav")->Play(0);
				level.SetCurrentColor(Color4f{ 0.95f, 0.8f, 0.4f, 1 }, 0.1f);
			}
		}
	}

	//TRS matrix
	m_DoorPoints = GetShape();
}

void Door::HandleCollision(const utils::HitInfo& hit)
{
}

std::string Door::ToString() const
{
	std::string s;

	s += "<Door \n";
	s += "rect=\"" + std::to_string(m_Position.x) + "," + std::to_string(m_Position.y) + "," + std::to_string(m_DoorRect.width) + "," + std::to_string(m_DoorRect.height) + "\" \n";
	s += "angle=\"" + std::to_string(m_Angle * utils::g_Rad2Deg) + "\" \n";
	s += "/>";

	return s;
}

void Door::SetDoorAngle(const utils::HitInfo& hit)
{
	m_Angle = atan2f((hit.intersectPoint.y + hit.normal.y) - m_Position.y, (hit.intersectPoint.x + hit.normal.x) - m_Position.x);
}

bool Door::IsOverlapping(std::vector<Point2f> points)
{
	std::vector<Point2f> shape = GetShape();
	for (Point2f point : points)
	{
		if (utils::IsPointInPolygon(point, shape))
		{
			m_Angle = atan2f(m_Position.y + point.y, m_Position.x + point.x);
			return true;
		}
	}
	return false;
}

std::vector<Point2f> Door::GetShape() const
{
	Matrix2x3 matLocation, matScale, matRotation, matWorld;

	matLocation.SetAsTranslate(m_Position);
	matRotation.SetAsRotate(m_Angle * utils::g_Rad2Deg);
	matScale.SetAsScale(m_Scale);

	matWorld = matLocation * matRotation * matScale;
	return matWorld.Transform(m_DoorRect);
}
