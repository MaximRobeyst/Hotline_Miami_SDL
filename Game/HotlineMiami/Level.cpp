#include "pch.h"
#include "Level.h"
#include "Texture.h"
#include "SVGParser.h"
#include "utils.h"
#include "GameObject.h"
#include "Door.h"
#include "Pickup.h"
#include "PickupManager.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "Player.h"
#include "Gun.h"
#include "Mellee.h"
#include "HUD.h"
#include "Bloodpool.h"
#include "Dog.h"
#include "Mobster.h"
#include "Car.h"

#include <iostream>
#include <fstream>
#include "ResourceManager.h"
#include "Game.h"

Level::Level(const std::string& filename)
	: m_pLevelLayout{nullptr}
	, m_pPlayer{nullptr}
	, m_pPickupManager{new PickupManager() }
	, m_pEnemyManager{new EnemyManager() }
{
	std::ifstream inputFile(filename);
	std::string element;
	if (!inputFile) std::cerr << "Problem with opening " + filename << std::endl;

	while (std::getline(inputFile, element, '>'))
	{
		CreateElement(element);
	}
}

Level& Level::operator=(Level&& other) noexcept
{
	if (&other != this) 
	{
		m_LevelName = other.m_LevelName;
		m_pLevelLayout = other.m_pLevelLayout;
		m_Shapes = other.m_Shapes;
		m_EndLevelRect = other.m_EndLevelRect;
		m_NextLevel = other.m_NextLevel;

		m_Vertices.clear();
		m_Vertices = other.m_Vertices;

		m_pPlayer->SetPosition(other.m_pPlayer->GetPosition());

		delete m_pPickupManager;
		m_pPickupManager = other.m_pPickupManager;

		delete m_pEnemyManager;
		m_pEnemyManager = other.m_pEnemyManager;


		for (GameObject* element : m_pProps)
		{
			delete element;
		}
		m_pProps.clear();
		m_pProps = other.m_pProps;

		other.m_pLevelLayout = nullptr;
		other.m_pPickupManager = nullptr;
		other.m_pEnemyManager = nullptr;
		other.m_pProps.clear();
	}

	return *this;
}

Level::~Level()
{
	if (m_pProps.size() > 0) 
	{
		for (GameObject* element : m_pProps)
		{
			delete element;
		}
		m_pProps.clear();
	}

	delete m_pPickupManager;
	delete m_pEnemyManager;
	delete m_pPlayer;
}

void Level::DrawBackground() const
{
	float sinX = sinf(Game::GetElapsedTime() + (utils::g_Pi / 2));

	//utils::SetColor(m_CurrentColor)
	if (m_Timer >= m_Cooldown)
	{
		utils::SetColor(
			Color4f{
				(m_Color1.r / 2) + sinX * ((m_Color1.r - m_Color2.r) / 2),
				(m_Color1.g / 2) + sinX * ((m_Color1.g - m_Color2.g) / 2),
				(m_Color1.b / 2) + sinX * ((m_Color1.b - m_Color2.b) / 2),
				1
			}
		);
	}
	else 
	{
		utils::SetColor(m_CurrentColor);
	}
	utils::FillRect(Rectf{0,0, m_pLevelLayout->GetWidth(), m_pLevelLayout->GetHeight()});
}

void Level::DrawLayout() const
{
	m_pLevelLayout->Draw(Rectf{0, 0, m_pLevelLayout->GetWidth(), m_pLevelLayout->GetHeight()});
}

void Level::DrawProps() const
{
	for (GameObject* element : m_pProps)
	{
		element->Draw();
	}

	m_pPickupManager->Draw();
	m_pEnemyManager->Draw();
	m_pPlayer->Draw();
}

void Level::Update(float elapsedSec, HUD& hud)
{
	if (m_Timer <= m_Cooldown) 
	{
		m_Timer += elapsedSec;
	}

	for (int i = 0; i < (int)m_pProps.size(); i++)
	{
		m_pProps[i]->Update(elapsedSec, *this);
	}

	m_pPlayer->Update(elapsedSec, *this);
	m_pPickupManager->Update(elapsedSec, *this);
	m_pEnemyManager->Update(elapsedSec, *this);
}

void Level::HandleCollision(GameObject& gameObject) const
{
	utils::HitInfo hit;

	Point2f rayEndPoint{ gameObject.GetPosition().x + gameObject.GetVelocity().x, gameObject.GetPosition().y + gameObject.GetVelocity().y };

	for (int i = 0; i < m_Shapes; i++) 
	{
		if (utils::Raycast(m_Vertices[i], gameObject.GetPosition().ToPoint2f(), rayEndPoint, hit))
		{
			gameObject.HandleCollision(hit);
		}
	}
}

void Level::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	m_pPlayer->ProcessKeyDownEvent(e, *this);
}

void Level::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
	m_pPlayer->ProcessKeyUpEvent(e);
}

void Level::SetMousePosition(const Point2f& mousePosition)
{
	m_pPlayer->SetMousePosition(mousePosition);
}

void Level::MouseDownEvent(const SDL_MouseButtonEvent& e)
{
	m_pPlayer->MouseDownEvent(e, *this);
}

void Level::MouseUpEvent(const SDL_MouseButtonEvent& e)
{
	m_pPlayer->MouseUpEvent(e);
}

bool Level::Raycast(Point2f p1, Point2f p2, bool props) const
{
	utils::HitInfo hit;
	for (int i = 0; i < m_Shapes; i++)
	{
		if (utils::Raycast(m_Vertices[i], p1, p2, hit))
			return true;
	}

	if (props) 
	{
		for (GameObject* element : m_pProps)
		{
			if (utils::Raycast(element->GetShape(), p1, p2, hit))
				return true;
		}
	}

	return false;
}

bool Level::LevelComplete() const
{
	bool b = m_pEnemyManager->GetStageClear() && utils::IsPointInRect(m_pPlayer->GetPosition().ToPoint2f(), m_EndLevelRect);

	if (b)
		SaveToFile("Resources/" + m_LevelName + "-save");

	return b;
}

void Level::AddProp(GameObject* gameobject)
{
	m_pProps.push_back(gameobject);
}

void Level::DeleteProp(GameObject* gameobject)
{
	GameObject* tmp = m_pProps[m_pProps.size() - 1];

	for (int i = 0; i < (int)m_pProps.size(); i++) 
	{
		if (m_pProps[i] == gameobject) 
		{
			m_pProps[m_pProps.size() - 1] = m_pProps[i];
			m_pProps[i] = tmp;

			delete m_pProps[m_pProps.size() - 1];
			m_pProps.pop_back();

			break;
		}
	}
}

void Level::DeleteProp(int index)
{
	GameObject* tmp = m_pProps[m_pProps.size() - 1];
	m_pProps[m_pProps.size() - 1] = m_pProps[index];
	m_pProps[index] = tmp;

	delete m_pProps[m_pProps.size() - 1];
	m_pProps.pop_back();
}

Rectf Level::GetLevelBoundries() const
{
	return Rectf{ 0,0,m_pLevelLayout->GetWidth(), m_pLevelLayout->GetHeight() };
}

Rectf Level::GetLevelEnd() const
{
	return m_EndLevelRect;
}

std::string Level::GetNextLevel() const
{
	return m_NextLevel;
}

PickupManager* Level::GetPickupManager() const
{
	return m_pPickupManager;
}

EnemyManager* Level::GetEnemyManager() const
{
	return m_pEnemyManager;
}

Player* Level::GetPlayer() const
{
	return m_pPlayer;
}

void Level::SetCurrentColor(Color4f c, float cooldown)
{
	m_Timer = 0;
	m_CurrentColor = c;
	m_Cooldown = cooldown;
}

void Level::CreateElement(const std::string& element)
{
	if (element.find("Level") != std::string::npos)
	{
		CreateLevel(element);
	}
	else if (element.find("Pickup") != std::string::npos)
	{
		m_pPickupManager->AddItem(CreatePickup(element));
	}
	else if (element.find("Enemy") != std::string::npos)
	{
		m_pEnemyManager->AddEnemy(CreateEnemy(element));
	}
	else if (element.find("Door") != std::string::npos)
	{
		m_pProps.push_back(CreateDoor(element));
	}
	else if (element.find("Blood") != std::string::npos)
	{
		m_pProps.push_back(CreateBlood(element));
	}
	else if (element.find("Dog") != std::string::npos) 
	{
		m_pEnemyManager->AddEnemy(CreateDog(element));
	}
	else if (element.find("Car") != std::string::npos)
	{
		m_pProps.push_back(CreateCar(element));
	}
}

void Level::CreateLevel(const std::string& levelText)
{
	m_LevelName = GetAttributeValue("levelname", levelText);
	if (m_LevelName == "level-end") 
	{
		Game::SetState(Game::GameState::EndScreen);
	}

	m_pLevelLayout = ResourceManager::GetTexture(GetAttributeValue("background", levelText));
	m_pPlayer = new Player(ToVector2f(GetAttributeValue("player_position", levelText)));

	SVGParser::GetVerticesFromSvgFile(GetAttributeValue("vertices", levelText), m_Vertices);
	m_Shapes = std::stoi(GetAttributeValue("shapes", levelText));
	m_EndLevelRect = ToRectf(GetAttributeValue("endlevelrect", levelText));
	m_NextLevel = GetAttributeValue("nextlevel", levelText);
}

Pickup* Level::CreatePickup(const std::string& pickupText)
{
	Pickup* pickup = new Pickup(
		ToVector2f(GetAttributeValue("position", pickupText)),
		GetAttributeValue("name", pickupText),
		std::stof(GetAttributeValue("width", pickupText)),
		std::stof(GetAttributeValue("height", pickupText)),
		(pickupText.find("\nangle=\"") != std::string::npos) ? std::stof(GetAttributeValue("\nangle", pickupText)) : 0.f
	);

	if (GetAttributeValue("type", pickupText) == "gun") {
		pickup->SetWeapon(new Gun(
			std::stoi(GetAttributeValue("attackframes", pickupText)),
			std::stoi(GetAttributeValue("walkframes", pickupText)),
			std::stoi(GetAttributeValue("walkfps", pickupText)),
			std::stoi(GetAttributeValue("attackfps", pickupText)),
			GetAttributeValue("soundeffect", pickupText),
			std::stoi(GetAttributeValue("clip", pickupText)),
			std::stoi(GetAttributeValue("ammo", pickupText)),
			std::stof(GetAttributeValue("range", pickupText)),
			std::stof(GetAttributeValue("firerate", pickupText)),
			std::stof(GetAttributeValue("spreadangle", pickupText)),
			std::stoi(GetAttributeValue("spreadpieces", pickupText)),
			std::stof(GetAttributeValue("bulletoffset", pickupText))
		));
	}
	else 
	{
		pickup->SetWeapon(new Mellee(
			std::stoi(GetAttributeValue("attackframes", pickupText)),
			std::stoi(GetAttributeValue("walkframes", pickupText)),
			std::stoi(GetAttributeValue("walkfps", pickupText)),
			std::stoi(GetAttributeValue("attackfps", pickupText)),
			GetAttributeValue("soundeffect", pickupText),
			std::stof(GetAttributeValue("range", pickupText)),
			std::stof(GetAttributeValue("attackangle", pickupText))
		));
	}

	return pickup;
	//m_pPickupManager->AddItem(new Pickup(Vector2f(100, 500), "Axe", 64, 64, 7, 10));
}

Enemy* Level::CreateEnemy(const std::string& enemyText)
{
	Mobster* enemy = new Mobster(
		ToVector2f(GetAttributeValue("position", enemyText)),
		std::stof(GetAttributeValue("angle", enemyText)),
		ToVector2fList(GetAttributeValue("path", enemyText)),
		(enemyText.find("\nenemystate=\"") != std::string::npos) ? (Enemy::EnemyState)std::stoi(GetAttributeValue("\nenemystate", enemyText)) : Enemy::EnemyState::Moving
	);

	if(enemyText.find("pickup=") != std::string::npos)
		enemy->SetCurrentPickup(CreatePickup((GetAttributeValue("pickup", enemyText, '*'))), *m_pPickupManager);

	return enemy;
}

Dog* Level::CreateDog(const std::string& enemyText)
{
	Dog* dog = new Dog(
		ToVector2f(GetAttributeValue("position", enemyText)),
		std::stof(GetAttributeValue("angle", enemyText)),
		ToVector2fList(GetAttributeValue("path", enemyText)),
		(enemyText.find("\nenemystate=\"") != std::string::npos) ? (Enemy::EnemyState)std::stoi(GetAttributeValue("\nenemystate", enemyText)) : Enemy::EnemyState::Moving
	);

	return dog;
}

GameObject* Level::CreateDoor(const std::string& doorText)
{
	return new Door(
		ToRectf(GetAttributeValue("rect", doorText)),
		std::stof(GetAttributeValue("angle", doorText)) * utils::g_Deg2Rad
	);
}

GameObject* Level::CreateBlood(const std::string& bloodText)
{
	return new Blood{
		ToVector2f(GetAttributeValue("position", bloodText)),
		std::stof(GetAttributeValue("angle", bloodText)) * utils::g_Deg2Rad,
		std::stof(GetAttributeValue("scale", bloodText)),
		GetAttributeValue("animation", bloodText),
		std::stof(GetAttributeValue("timer", bloodText))
	};
}

GameObject* Level::CreateCar(const std::string& carText)
{
	Car* c = new Car(
		ToVector2f(GetAttributeValue("position", carText)),
		std::stof(GetAttributeValue("angle", carText)) * utils::g_Deg2Rad
	);
	m_SavedNextLevel = GetAttributeValue("nextlevel", carText);

	m_SaveEndLevelRect = c->GetRect();

	return c;
}

std::string Level::GetAttributeValue(const std::string& attrName, const std::string& element, char seperator)
{
	std::string searchName{ attrName + "=" };

	size_t attributePosition = element.find(searchName);
	size_t openingQoute = element.find(seperator, attributePosition);
	size_t closingQoute = element.find(seperator, openingQoute + 1);

	std::string attribute;

	attribute = element.substr(openingQoute + 1, closingQoute - openingQoute - 1);

	return attribute;
}

Point2f Level::ToPoint2f(const std::string& pointString)
{
	Point2f point;
	std::stringstream str{ pointString };

	str >> point.x;
	str.ignore();
	str >> point.y;
	return point;
}

Vector2f Level::ToVector2f(const std::string& vectorString)
{
	Vector2f vector;
	std::stringstream str{ vectorString };

	str >> vector.x;
	str.ignore();
	str >> vector.y;
	return vector;
}

Rectf Level::ToRectf(const std::string& rectString)
{
	Rectf rectf;
	std::stringstream str{ rectString };

	str >> rectf.left;
	str.ignore();
	str >> rectf.bottom;
	str.ignore();
	str >> rectf.width;
	str.ignore();
	str >> rectf.height;
	return rectf;
}

std::vector<Vector2f> Level::ToVector2fList(const std::string& vectorString)
{
	std::vector<Vector2f> list;

	std::stringstream path{ vectorString };
	std::string point;

	size_t position = 0;

	size_t start = position;
	size_t end = position;

	do
	{
		start = path.str().find(" ", position);
		end = path.str().find(" ", start + 1);

		position = end;
		point = path.str().substr(start, end - start + 1);

		list.push_back(ToVector2f(point));

	} while (end < path.str().size() && start < path.str().size());

	return list;
}

void Level::SaveToFile(const std::string& filename) const
{
	std::ofstream outputFile(filename);
	if (!outputFile) std::cout << "Problem with opening" + filename << std::endl;

	outputFile << "<Level\n";

	outputFile << "levelname=\"" << m_LevelName << "\" \n";
	outputFile << "background=\"Resources/Images/" << m_LevelName << ".png\"\n";
	outputFile << "player_position=\"" << m_pPlayer->GetPosition().x << ',' << m_pPlayer->GetPosition().y << "\" \n";
	outputFile << "vertices=\"Resources/Images/" << m_LevelName << ".svg\"\n";
	outputFile << "shapes=\"" << m_Shapes << "\" \n";
	outputFile << "endlevelrect=\"" << m_SaveEndLevelRect.left << ',' << m_SaveEndLevelRect.bottom << ',' << m_SaveEndLevelRect.width << ',' << m_SaveEndLevelRect.height << "\" \n";
	outputFile << "nextlevel=\"Resources/" << m_SavedNextLevel << "\" \n";

	outputFile << "/>\n";

	//save pickups
	outputFile << m_pPickupManager->ToString();

	//save enemies
	outputFile << m_pEnemyManager->ToString();

	//save props
	for (GameObject* go : m_pProps)
		outputFile << std::endl << go->ToString() << std::endl;
}
