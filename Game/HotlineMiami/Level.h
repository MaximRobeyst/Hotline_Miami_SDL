#pragma once
#include "Vector2f.h"

#include <vector>

class Texture;
class GameObject;
class PickupManager;
class EnemyManager;
class Player;
class Pickup;
class Enemy;
class HUD;
class Dog;
class Level final
{
public:
	Level(const std::string& filename);
	Level(const Level& other) = delete;
	Level& operator=(const Level& other) = delete;
	Level(Level&& other) = delete;
	Level& operator=(Level&& other) noexcept;
	~Level();

	void DrawBackground() const;
	void DrawLayout() const;
	void DrawProps() const;

	void Update(float elapsedSec, HUD& hud);
	void HandleCollision(GameObject& gameObject) const;

	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e);
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e);
	void SetMousePosition(const Point2f& mousePosition);
	void MouseDownEvent(const SDL_MouseButtonEvent& e);
	void MouseUpEvent(const SDL_MouseButtonEvent& e);

	bool Raycast(Point2f p1, Point2f p2, bool props = true) const;
	bool LevelComplete() const;

	void AddProp(GameObject* gameobject);
	void DeleteProp(GameObject* gameobject);
	void DeleteProp(int index);

	Rectf GetLevelBoundries() const;
	Rectf GetLevelEnd() const;
	std::string GetNextLevel() const;

	PickupManager* GetPickupManager() const;
	EnemyManager* GetEnemyManager() const;
	Player* GetPlayer() const;

	void SetCurrentColor(Color4f c, float cooldown);

private:
	//Lists
	std::vector<std::vector<Point2f>> m_Vertices;
	std::vector<GameObject*> m_pProps;

	std::string m_LevelName;

	//Managers
	PickupManager* m_pPickupManager;
	EnemyManager* m_pEnemyManager;
	Player* m_pPlayer;

	//Level Data
	Texture* m_pLevelLayout;
	int m_Shapes{ 1 };
	Rectf m_EndLevelRect{};
	Rectf m_SaveEndLevelRect{};
	std::string m_NextLevel{};
	std::string m_SavedNextLevel{};

	//Background
	const Color4f m_Color1{ 0.97f, 0.4f ,0.92f, 1.f };
	const Color4f m_Color2{ 0.15f, 1.f, 0.96f, 1.f };
	Color4f m_CurrentColor{};

	float m_Cooldown{5};
	float m_Timer{m_Cooldown};

	// Read form file
	void CreateElement(const std::string& element);
	void CreateLevel(const std::string& levelText);
	Pickup* CreatePickup(const std::string& pickupText);
	Enemy* CreateEnemy(const std::string& enemyText);
	Dog* CreateDog(const std::string& enemyText);

	GameObject* CreateDoor(const std::string& doorText);
	GameObject* CreateBlood(const std::string& bloodText);
	GameObject* CreateCar(const std::string& carText);

	std::string GetAttributeValue(const std::string& attrName, const std::string& element, char seperator='\"');

	Point2f ToPoint2f(const std::string& pointString);
	Vector2f ToVector2f(const std::string& vectorString);
	Rectf ToRectf(const std::string& rectString);

	std::vector<Vector2f> ToVector2fList(const std::string& vectorString);

	// Save level
	void SaveToFile(const std::string& filename) const;
};

