#include "pch.h"
#include "Camera.h"
#include "Matrix2x3.h"
#include "Texture.h"
#include "utils.h"

Camera::Camera(float width, float height, float scale)
	: m_Width{width}
	, m_Height{height}
	, m_Scale{scale}
{
}

void Camera::Draw(const Point2f& target) const
{
	glScalef(m_Scale, m_Scale, 0);
	glTranslatef(-m_CurrentPos.x, -m_CurrentPos.y, 0);
}

void Camera::Update(float elapsedSec, const Point2f& target)
{
	Point2f cameraPos{ Track(target) };
	Clamp(cameraPos);

	m_CurrentPos.x += (cameraPos.x - m_CurrentPos.x) * m_Speed * elapsedSec;
	m_CurrentPos.y += (cameraPos.y - m_CurrentPos.y) * m_Speed * elapsedSec;
}

void Camera::SetLevelBoundries(const Rectf& levelBoundries)
{
	m_LevelBoundaries = levelBoundries;
}

Point2f Camera::WindowToGameWorld(const Point2f& p, const Point2f& target) const
{
	Point2f cameraPos{ Track(target) };
	Clamp(cameraPos);

	return Point2f{ (p.x / m_Scale) + cameraPos.x, (p.y / m_Scale) + cameraPos.y };
}

Point2f Camera::Track(const Point2f& target) const
{
	return Point2f{ (target.x) - ((m_Width / m_Scale) / 2), (target.y) - ((m_Height / m_Scale) / 2) };
}

void Camera::Clamp(Point2f& cameraCenter) const
{
	if (cameraCenter.x / m_Scale < m_LevelBoundaries.left)
		cameraCenter.x = m_LevelBoundaries.left;
	else if (cameraCenter.x + (m_Width / m_Scale) > (m_LevelBoundaries.left + m_LevelBoundaries.width))
		cameraCenter.x = ((m_LevelBoundaries.left + m_LevelBoundaries.width) - (m_Width / m_Scale));

	if (cameraCenter.y < m_LevelBoundaries.bottom)
		cameraCenter.y = m_LevelBoundaries.bottom;
	else if (cameraCenter.y + (m_Height / m_Scale) > (m_LevelBoundaries.bottom + m_LevelBoundaries.height))
		cameraCenter.y = ((m_LevelBoundaries.bottom + m_LevelBoundaries.height) - ((m_Height / m_Scale)));
}
