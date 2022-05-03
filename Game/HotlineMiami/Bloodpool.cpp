#include "pch.h"
#include "Bloodpool.h"
#include "Animation.h"
#include "Texture.h"

#include <iostream>

Blood::Blood(const Vector2f& pos, float angle, float scale)
	: GameObject(pos, angle, scale)
	, m_AnimationName{ "Resources/Anim/Bloodpool.txt" }
	, m_pAnimation(new Animation(m_AnimationName))
{
}

Blood::Blood(const Vector2f& pos, float angle, float scale, const Vector2f& velocity)
	: GameObject(pos, angle, scale)
	, m_AnimationName{ "Resources/Anim/Bloodsplatter.txt" }
	, m_pAnimation(new Animation(m_AnimationName))
{
	m_Velocity = velocity;
}

Blood::Blood(const Vector2f& pos, float angle, float scale, const std::string& animation, float timer)
	: GameObject(pos, angle, scale)
	, m_AnimationName{ animation }
	, m_pAnimation(new Animation(m_AnimationName))
	, m_Timer{timer}
{
	m_pAnimation->SetFrameFromTimer(m_Timer);
}

Blood::~Blood()
{
	delete m_pAnimation;
}

void Blood::Draw() const
{
	glPushMatrix();
		glTranslatef(m_Position.x, m_Position.y, 0);
		glRotatef(m_Angle * utils::g_Rad2Deg, 0, 0, 1);

		glTranslatef(-m_pAnimation->GetFrameWidth() / 2, -m_pAnimation->GetFrameHeight() / 2, 0);
		m_pAnimation->Draw();
		glTranslatef(m_pAnimation->GetFrameWidth() / 2, m_pAnimation->GetFrameHeight() / 2, 0);
	glPopMatrix();
}

void Blood::Update(float elapsedSec, Level& level)
{
	if (m_Timer >= m_pAnimation->GetFrameTime() * (m_pAnimation->GetNrOfFrames() - 1))
		return;
		
	m_Timer += elapsedSec;
	m_Position += m_Velocity * elapsedSec;
	m_pAnimation->Update(elapsedSec);
}

void Blood::HandleCollision(const utils::HitInfo& hit)
{
}

std::string Blood::ToString() const
{
	std::string s;

	s += "<Blood \n";
	s += "position=\"" + std::to_string(m_Position.x) + "," + std::to_string(m_Position.y) + "\" \n";
	s += "angle=\"" + std::to_string(m_Angle * utils::g_Rad2Deg) + "\" \n";
	s += "scale=\"" + std::to_string(m_Scale) + "\" \n";
	s += "animation=\"" + m_AnimationName + "\" \n";
	s += "timer=\"" + std::to_string(m_Timer) + "\" \n";
	s += "/>";

	return s;

	return std::string();
}

std::vector<Point2f> Blood::GetShape() const
{
	return std::vector<Point2f>();
}

