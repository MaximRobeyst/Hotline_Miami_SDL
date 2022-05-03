#pragma once
#include <vector>
#include "Vector2f.h"
#include "utils.h"

class Level;
class GameObject
{
public:
	GameObject(const Vector2f& position, float angle = 0, float scale = 1, float friction = 0);
	GameObject(const GameObject& go) = delete;
	GameObject& operator=(const GameObject& go) = delete;
	GameObject(GameObject&& go) = delete;
	GameObject& operator=(GameObject&& go) = delete;
	virtual ~GameObject() = default;
	
	virtual void Draw() const = 0;
	virtual void Update(float elapsedSec, Level& level) = 0;
	virtual void HandleCollision(const utils::HitInfo& hit) = 0;

	Vector2f GetPosition() const;
	Vector2f GetVelocity() const;
	virtual std::vector<Point2f> GetShape() const = 0;
	virtual std::string ToString() const = 0;

protected:
	Vector2f m_Position;
	float m_Angle;
	float m_Scale;

	Vector2f m_Velocity;
	float m_Friction{ 0.5f };
};

