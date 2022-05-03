#pragma once
#include "GameObject.h"

class Animation;
class Blood final : public GameObject
{
public:
	// Used for blood pool
	Blood(const Vector2f& pos, float angle, float scale);
	// Used for blood splatter
	Blood(const Vector2f& pos, float angle, float scale, const Vector2f& veocity);
	Blood(const Vector2f& pos, float angle, float scale, const std::string& animation, float timer);
	//Rule of 5
	Blood(const Blood& other) = delete;
	Blood& operator=(const Blood& other) = delete;
	Blood(Blood&& other) = delete;
	Blood& operator=(Blood&& other) = delete;
	~Blood();

	// Inherited via GameObject
	virtual void Draw() const override;
	virtual void Update(float elapsedSec, Level& level) override;
	virtual void HandleCollision(const utils::HitInfo& hit) override;
	virtual std::string ToString() const override;

	virtual std::vector<Point2f> GetShape() const override;

private:
	std::string m_AnimationName;
	Animation* m_pAnimation;
	float m_Timer{};
};

