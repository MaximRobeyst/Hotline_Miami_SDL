#include "pch.h"
#include "Pickup.h"
#include "utils.h"
#include "Texture.h"
#include "Player.h"
#include "Level.h"
#include "Weapon.h"
#include "Mellee.h"
#include "Gun.h"
#include "Matrix2x3.h"
#include "ResourceManager.h"
#include "Game.h"

#include <string>
#include <iostream>

Pickup::Pickup(const Vector2f& position, const std::string& pickupName, float frameWidth, float frameHeight, float angle)
	: GameObject(position, angle, 1, 0.95f)
	, m_pWeapon{ nullptr }
	, m_PickUpName{ pickupName }
	, m_pPickupTexture{ ResourceManager::GetTexture(GetFileName(pickupName)) }
	, m_FrameHeight{ frameHeight }
	, m_FrameWidth{frameWidth}
{
}

Pickup::Pickup(const Pickup& other)
	: GameObject(other.m_Position, other.m_Angle, other.m_Scale, other.m_Friction)
	, m_pWeapon{ nullptr }
	, m_PickUpName{ other.m_PickUpName }
	, m_pPickupTexture{ ResourceManager::GetTexture(GetFileName(m_PickUpName)) }
	, m_FrameHeight{ other.m_FrameHeight }
	, m_FrameWidth{ other.m_FrameWidth }
{
	if (other.m_pWeapon->IsGun())
		m_pWeapon = new Gun(*static_cast<Gun*>(other.m_pWeapon));
	else
		m_pWeapon = new Mellee(*static_cast<Mellee*>(other.m_pWeapon));
}


Pickup::~Pickup()
{
	delete m_pWeapon;
}

void Pickup::Draw() const
{
	glPushMatrix();
		// Set translation, scale and rotation of the texture
		glTranslatef(m_Position.x, m_Position.y, 0);
		glRotatef(m_Angle * utils::g_Rad2Deg, 0, 0, 1);
		glScalef(m_Scale + m_AnimationScaleOffset, m_Scale + m_AnimationScaleOffset, 0);
		glTranslatef(-m_pPickupTexture->GetWidth() / 2, -m_pPickupTexture->GetHeight() / 2, 0);
		m_pPickupTexture->Draw();
	glPopMatrix();
}

void Pickup::Update(float elapsedSec, Level& level)
{
	Vector2f tmp = m_Velocity;
	m_Velocity = m_Velocity * m_Friction * elapsedSec;

	level.HandleCollision(*this);

	m_Velocity = m_Velocity.Normalized() * tmp.Length() * m_Friction;

	m_Position += m_Velocity * elapsedSec;
	m_pWeapon->Update(elapsedSec);

	//Rotation Animation
	if (m_Velocity.SquaredLength() >= 0.1f)
		m_Angle += m_Velocity.Length() * utils::g_Deg2Rad * elapsedSec;

	//Scale Animation
	float maxScaleOffset{ 0.1f };
	float frequency{ 4 };

	m_AnimationScaleOffset = sinf(frequency * Game::GetElapsedTime()) * maxScaleOffset;
}

void Pickup::HandleCollision(const utils::HitInfo& hit)
{
	m_Position -= m_Velocity.Normalized() * hit.lambda;
	//m_Velocity = -hit.normal * m_Velocity.Length();

	m_Velocity = 2 * ((-hit.normal).DotProduct(m_Velocity.Normalized())) * -hit.normal - m_Velocity;
}

std::string Pickup::ToString() const
{
	std::string s;

	s += "<Pickup\n";
	s += "position=\"" + std::to_string(m_Position.x) + "," + std::to_string(m_Position.y) + "\" \n";
	s += "name=\"" + m_PickUpName + "\" \n";
	s += "width=\"" + std::to_string(m_FrameWidth) + "\" \n";
	s += "height=\"" + std::to_string(m_FrameHeight) + "\" \n";
	s += m_pWeapon->ToString();
	s += "angle=\"" + std::to_string(m_Angle) + "\" \n";

	s += "/>";

	return s;
}

std::vector<Point2f> Pickup::GetShape() const
{
	Matrix2x3 matTrans, matAngle, matScale, matCenter, matWorld;
	matTrans.SetAsTranslate(m_Position);
	matAngle.SetAsRotate(m_Angle * utils::g_Rad2Deg);
	matScale.SetAsScale(m_Scale);
	matCenter.SetAsTranslate(Vector2f{ -m_pPickupTexture->GetWidth() / 2, -m_pPickupTexture->GetHeight() / 2 });

	matWorld = matTrans * matAngle * matScale * matCenter;

	std::vector<Point2f> points;
	points = matWorld.Transform(Rectf{ 0, 0, m_pPickupTexture->GetWidth(), m_pPickupTexture->GetHeight() });

	return points;
}

bool Pickup::IsOverlapping(std::vector<Point2f> r) const
{
	for (Point2f p : r)
	{
		if (utils::IsPointInPolygon(p, GetShape()))
		{
			return true;
		}
	}
	return false;
}

void Pickup::SetVelocity(const Vector2f& v)
{
	m_Velocity = v;
}

void Pickup::SetPosition(const Vector2f& v)
{
	m_Position = v;
}

std::string Pickup::GetName() const
{
	return m_PickUpName;
}

Rectf Pickup::GetAnimRect(const Point2f& p) const
{
	return Rectf(p.x, p.y, m_FrameWidth, m_FrameHeight);
}

int Pickup::GetAttackFrames() const
{
	return m_pWeapon->GetAttackFrames();
}

int Pickup::GetWalkFrames() const
{
	return m_pWeapon->GetWalkFrames();
}

int Pickup::GetAttackFramesPerSec() const
{
	return m_pWeapon->GetAttackFramesPerSec();
}

int Pickup::GetWalkFramesPerSec() const
{
	return m_pWeapon->GetWalkFramesPerSec();
}

Weapon* Pickup::GetWeapon() const
{
	return m_pWeapon;
}

void Pickup::SetWeapon(Weapon* weapon)
{
	m_pWeapon = weapon;
}

std::string Pickup::GetFileName(const std::string& pickupName) const
{
	return "Resources/Images/pickup" + pickupName + ".png";
}
