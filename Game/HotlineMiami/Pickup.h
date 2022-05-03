#pragma once
#include "GameObject.h"

class Texture;
class Player;
class Enemy;
class Weapon;
class Pickup final: public GameObject
{
public:
	Pickup(const Vector2f& position, const std::string& pickupName, float frameWidth, float frameHeight, float angle = 0);
	Pickup(const Pickup& other);
	Pickup& operator=(const Pickup& other) = delete;
	Pickup(Pickup&& other) = delete;
	Pickup& operator=(Pickup&& other) = delete;
	~Pickup() override;


	virtual void Draw() const override;
	virtual void Update(float elapsedSec, Level& level) override;
	virtual void HandleCollision(const utils::HitInfo& hit) override;

	virtual std::string ToString() const override;

	virtual std::vector<Point2f> GetShape() const override;

	bool IsOverlapping(std::vector<Point2f> r) const;

	void SetVelocity(const Vector2f& v);
	void SetPosition(const Vector2f& v);

	std::string GetName() const;
	Rectf GetAnimRect(const Point2f& p = Point2f{0,0}) const;
	int GetAttackFrames() const;
	int GetWalkFrames() const;
	int GetAttackFramesPerSec() const;
	int GetWalkFramesPerSec() const;
	Weapon* GetWeapon() const;

	void SetWeapon(Weapon* weapon);

private:
	std::string m_PickUpName;
	Texture* m_pPickupTexture;
	Weapon* m_pWeapon;

	float m_FrameWidth;
	float m_FrameHeight;

	//This makes the pickups bob like in the game
	float m_AnimationScaleOffset{};

	std::string GetFileName(const std::string& pickupName) const;
};

