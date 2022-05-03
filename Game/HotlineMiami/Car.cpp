#include "pch.h"
#include "Car.h"
#include "Animation.h"
#include "Level.h"
#include "Player.h"
#include "EnemyManager.h"
#include "Matrix2x3.h"

Car::Car(const Vector2f& pos, float angle)
	: GameObject(pos, angle)
	, m_pAnimation{new Animation("Resources/Anim/Car_Animation.txt")}
{
}

Car::~Car()
{
	delete m_pAnimation;
}

void Car::Draw() const
{
	glPushMatrix();
		glTranslatef(m_Position.x, m_Position.y, 0);
		glRotatef(m_Angle * utils::g_Rad2Deg, 0, 0, 1);
		glTranslatef(-m_pAnimation->GetFrameWidth() / 2, -m_pAnimation->GetFrameHeight() / 2, 0);
		m_pAnimation->Draw();
		glTranslatef(m_pAnimation->GetFrameWidth() / 2, m_pAnimation->GetFrameHeight() / 2, 0);
	glPopMatrix();
}

void Car::Update(float elapsedSec, Level& level)
{
	if (!level.GetEnemyManager()->GetStageClear()) return;

	if (utils::GetDistance(level.GetPlayer()->GetPosition(), m_Position) < m_pAnimation->GetFrameWidth())
	{
		if (m_Open) return;
	
		m_pAnimation->SetStartPos(Point2f{ 0,0 });
	
		m_Timer += elapsedSec;
		if (m_Timer < m_pAnimation->GetNrOfFrames() * m_pAnimation->GetFrameTime())
		{
			m_pAnimation->Update(elapsedSec);
		}
		else {
			m_Open = true;
			m_Timer = 0;
		}
	}
	else
	{
		if (m_Open) 
		{
			m_pAnimation->SetStartPos(Point2f{ 0,m_pAnimation->GetFrameHeight() });
	
			m_Timer += elapsedSec;
			if (m_Timer < m_pAnimation->GetNrOfFrames() * m_pAnimation->GetFrameTime())
			{
				m_pAnimation->Update(elapsedSec);
			}
			else
			{
				m_Open = false;
				m_Timer = 0;
			}
		}
	}
}

void Car::HandleCollision(const utils::HitInfo& hit)
{
}

std::vector<Point2f> Car::GetShape() const
{
	Matrix2x3 matTrans, matAngle, matScale, matCenter, matWorld;
	matTrans.SetAsTranslate(m_Position);
	matAngle.SetAsRotate(m_Angle * utils::g_Rad2Deg);
	matScale.SetAsScale(m_Scale);
	matCenter.SetAsTranslate(Vector2f{ -m_pAnimation->GetFrameWidth() / 2, -m_pAnimation->GetFrameHeight() / 2 });

	matWorld = matTrans * matAngle * matScale * matCenter;

	std::vector<Point2f> points;
	points = matWorld.Transform(Rectf{ 0, 0, m_pAnimation->GetFrameWidth(), m_pAnimation->GetFrameHeight() });

	return points;
}

std::string Car::ToString() const
{
	std::string s;

	s += "<Car\n";
	s += "position=\"" + std::to_string(m_Position.x) + "," + std::to_string(m_Position.y) + "\" \n";
	s += "angle=\"" + std::to_string(m_Angle * utils::g_Rad2Deg) + "\" \n";

	s += "/>";

	return s;
}

Rectf Car::GetRect() const
{
	//calculate bounds
	std::vector<Point2f> points = GetShape();
	float minX = points[0].x;
	float minY = points[0].y;
	float maxX = points[0].x;
	float maxY = points[0].y;

	for (Point2f p : points) 
	{
		if (p.x < minX)
			minX = p.x;
		else if (p.x > maxX)
			maxX = p.x;

		if (p.y < minY)
			minY = p.y;
		else if (p.y > maxY)
			maxY = p.y;
	}

	return Rectf{minX, minY, maxX - minX, maxY - minY};
}
