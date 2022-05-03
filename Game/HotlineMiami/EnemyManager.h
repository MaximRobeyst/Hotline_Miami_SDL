#pragma once
#include "Vector2f.h"
#include <vector>
#include "Enemy.h"

class Player;
class Level;
class Pickup;
class EnemyManager final
{
public:
	EnemyManager() = default;
	EnemyManager(const EnemyManager& ) = delete;
	EnemyManager& operator=(const EnemyManager&) = delete;
	EnemyManager(EnemyManager&& ) = delete;
	EnemyManager& operator=(EnemyManager&&) = delete;
	~EnemyManager();

	Enemy* AddEnemy(Enemy* enemy);
	//Enemy* AddEnemy(Enemy* enemy, const Vector2f& pos, const Vector2f& v);
	void Update(float elapsedSec, Level& level);
	void Draw() const;

	bool EnemyHit(std::vector<Point2f> shape, bool stateImportant = true);
	bool EnemyHit(const Vector2f& startpos, float minangle, float maxAngle, float distance, const Level& level, Enemy::EnemyState switchState = Enemy::EnemyState::Dead);

	bool Bullet(const Vector2f& startpos, const Vector2f& velocity, float distance, const Level& level, Enemy::EnemyState switchState = Enemy::EnemyState::Dead);

	Enemy* GetHitItem() const;

	bool GetStageClear() const;
	std::string ToString() const;

private:
	std::vector<Enemy*> m_pEnemies;

	void Swap(int id1, int id2);
	bool m_StageClear{ true };
};

