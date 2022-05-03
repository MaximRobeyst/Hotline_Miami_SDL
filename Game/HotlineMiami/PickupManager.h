#pragma once
#include "Pickup.h"
#include <vector>

class Player;
class Enemy;
class PickupManager final
{
public:
	PickupManager() = default;
	PickupManager(const PickupManager& p) = delete;
	PickupManager& operator=(const PickupManager&) = delete;
	PickupManager(PickupManager&& p) = delete;
	PickupManager& operator=(PickupManager&&) = delete;
	~PickupManager();

	Pickup* AddItem(Pickup* pickup);
	Pickup* AddItem(Pickup* pickup, const Vector2f& pos, const Vector2f& v);
	void Update(float elapsedSec, Level& level);
	void Draw() const;

	int Size() const;
	bool HitItem(std::vector<Point2f> shape);
	bool HitItem(Enemy& enemy);
	Pickup* GetCollidedItem();
	Pickup* ReturnClosest(const Vector2f& refPoint);

	std::string ToString() const;

private:
	std::vector<Pickup*> m_pItems;

	void Swap(int id1, int id2);
};

