#pragma once
#include "Vector2f.h"
#include "GameObject.h"
#include <vector>
#include "Matrix2x3.h"

class Texture;
class Door final : public GameObject
{
public:
	Door(const Rectf& doorRect, float angle = 0.0f);

	virtual void Draw() const override;
	virtual void Update(float elapsedSec, Level& level) override;
	virtual void HandleCollision(const utils::HitInfo& hit) override;
	virtual std::string ToString() const override;

	virtual std::vector<Point2f> GetShape() const override;

	void SetDoorAngle(const utils::HitInfo& hit);
	bool IsOverlapping(std::vector<Point2f> points);

private:
	Texture* m_pDoorTexture;
	std::vector<Point2f> m_DoorPoints;
	Rectf m_DoorRect;
	float m_RotVelocity{0};
	
	Matrix2x3 m_MatWorld;

	Point2f m_StartPoint;
	Point2f m_EndPoint;

};

