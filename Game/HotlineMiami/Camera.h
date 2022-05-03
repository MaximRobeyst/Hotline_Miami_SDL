#pragma once
#include "Vector2f.h"

class Texture;
class Camera final
{
public:
	Camera(float width, float height, float scale);
	void Draw(const Point2f& target) const;
	void Update(float elapsedSec, const Point2f& target);

	void SetLevelBoundries(const Rectf& levelBoundries);
	Point2f WindowToGameWorld(const Point2f& p, const Point2f& target) const;

private:
	float m_Width;
	float m_Height;
	float m_Scale;
	Rectf m_LevelBoundaries;

	float m_ScrollSpeed{-0.5f};
	float m_Speed{ 7.5f };
	Point2f m_CurrentPos{};


	Point2f Track(const Point2f& target) const;
	void Clamp(Point2f& bottomLeftPos) const;
};

