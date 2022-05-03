#pragma once
#include "GameObject.h"

class Animation;
class Bullet final : public GameObject
{
public:
	Bullet(const Vector2f& pos,float elapsedSec, const Vector2f& v, float lifeTime = 2);
	Bullet(const Bullet& go) = delete;
	Bullet& operator=(const Bullet& go) = delete;
	Bullet(Bullet&& go) = delete;
	Bullet& operator=(Bullet&& go) = delete;
	~Bullet();

	// Inherited via GameObject
	virtual void Draw() const override;
	virtual void Update(float elapsedSec, Level& level) override;
	virtual void HandleCollision(const utils::HitInfo& hit) override;
	virtual std::string ToString() const override;

	virtual std::vector<Point2f> GetShape() const override;
private:
	Animation* m_pAnimation;
	float m_Lifetime;

	bool m_Remove{ false };
	float m_Timer{};

};

