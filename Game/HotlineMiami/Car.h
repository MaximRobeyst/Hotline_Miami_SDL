#pragma once
#include "GameObject.h"

class Animation;

class Car final: public GameObject
{
public:
	Car(const Vector2f& pos, float angle);
	Car(const Car& other) = delete;
	Car& operator=(const Car& other) = delete;
	Car(Car&& other) = delete;
	Car& operator=(Car&& other) = delete;
	~Car();

	virtual void Draw() const override;
	virtual void Update(float elapsedSec, Level& level) override;
	virtual void HandleCollision(const utils::HitInfo& hit) override;
	virtual std::vector<Point2f> GetShape() const override;
	virtual std::string ToString() const override;

	Rectf GetRect() const;

private:
	Animation* m_pAnimation;

	float m_Timer{};
	bool m_Open{false};
};

