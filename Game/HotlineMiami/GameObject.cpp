#include "pch.h"
#include "GameObject.h"

GameObject::GameObject(const Vector2f& position, float angle, float scale, float friction)
	: m_Position{ position }
	, m_Angle{angle}
	, m_Scale{scale}
	, m_Friction{friction}
	, m_Velocity{ Vector2f{0,0} }
{
}

Vector2f GameObject::GetPosition() const
{
	return m_Position;
}

Vector2f GameObject::GetVelocity() const
{
	return m_Velocity;
}
