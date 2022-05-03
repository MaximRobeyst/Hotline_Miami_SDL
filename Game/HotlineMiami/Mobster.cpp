#include "pch.h"
#include "Mobster.h"
#include "Animation.h"
#include "Level.h"
#include "Pickup.h"
#include "ResourceManager.h"
#include "PickupManager.h"
#include "Weapon.h"
#include "Matrix2x3.h"
#include "Player.h"
#include "Game.h"
#include "HUD.h"
#include "Bloodpool.h"

Mobster::Mobster(const Vector2f& position, float angle, std::vector<Vector2f> path, EnemyState enemyState)
	: Enemy(position, angle* utils::g_Deg2Rad,path, enemyState)
	, m_pLegAnimation{ new Animation("Resources/Anim/EnemyLegs.txt") }
	, m_pTorsoAnimation{ new Animation("Resources/Anim/EnemyUnarmed.txt") }
	, m_pCurrentPickup{ nullptr }
{
}

Mobster::~Mobster()
{
	delete m_pLegAnimation;
	delete m_pTorsoAnimation;

	if (m_pCurrentPickup != nullptr)
		delete m_pCurrentPickup;
}

void Mobster::Draw() const
{
	if (m_EnemyState == Enemy::EnemyState::Takeout)
		return;

	glPushMatrix();
	// Set translation, scale and rotation of the texture
	glTranslatef(m_Position.x, m_Position.y, 0);
	glRotatef(m_Angle * utils::g_Rad2Deg, 0, 0, 1);
	glScalef(m_Scale, m_Scale, 0);
	//draw the enemy legs
	if (m_EnemyState != Enemy::EnemyState::Dead && m_EnemyState != Enemy::EnemyState::Unconsious)
	{
		glTranslatef(-m_pLegAnimation->GetFrameWidth() / 2, -m_pLegAnimation->GetFrameHeight() / 2, 0);
		m_pLegAnimation->Draw();
		glTranslatef(m_pLegAnimation->GetFrameWidth() / 2, m_pLegAnimation->GetFrameHeight() / 2, 0);
	}

	//draw the enemy Torso
	glTranslatef(-m_pTorsoAnimation->GetFrameWidth() / 2, -m_pTorsoAnimation->GetFrameHeight() / 2, 0);
	m_pTorsoAnimation->Draw();
	glTranslatef(m_pTorsoAnimation->GetFrameWidth() / 2, m_pTorsoAnimation->GetFrameHeight() / 2, 0);
	glPopMatrix();
}

void Mobster::Update(float elapsedSec, Level& level)
{
	if (m_Died)
	{
		if (level.Raycast(m_Position.ToPoint2f(), (m_Position + -GetForward() * (m_pTorsoAnimation->GetFrameWidth() / 2)).ToPoint2f(), false))
			m_pTorsoAnimation->SetStartPos(Point2f(0, m_pTorsoAnimation->GetFrameHeight() * 2));
		else
			m_pTorsoAnimation->SetStartPos(Point2f(0, m_pTorsoAnimation->GetFrameHeight()));
		m_pTorsoAnimation->ResetAnimation();
		m_EnemyState = Enemy::EnemyState::Dead;
		return;
	}

	if (m_pCurrentPickup != nullptr)
		m_pCurrentPickup->GetWeapon()->Update(elapsedSec);

	switch (m_EnemyState)
	{
	case Enemy::EnemyState::Idle:
		UpdateIdle(elapsedSec, level);
		break;
	case Enemy::EnemyState::Moving:
		UpdateMoving(elapsedSec, level);
		break;
	case Enemy::EnemyState::Unconsious:
		UpdateUnconsious(elapsedSec, level);
		break;
	case Enemy::EnemyState::Getup:
		UpdateGetUp(elapsedSec, level);
		break;
	case Enemy::EnemyState::Attack:
		UpdateAttack(elapsedSec, level);
		break;
	case Enemy::EnemyState::Dead:
		UpdateDead(elapsedSec, level);
		break;
	case Enemy::EnemyState::Takeout:
		m_Score = 1000;
		break;
	}

	level.HandleCollision(*this);
	m_Position += m_Velocity;
}
std::string Mobster::ToString() const
{
	std::string s;

	s += "<Enemy\n";
	s += "position=\"" + std::to_string(m_Position.x) + "," + std::to_string(m_Position.y) + "\" \n";
	s += "angle=\"" + std::to_string(m_Angle * utils::g_Rad2Deg) + "\" \n";

	s += "path=\"";
	for (Vector2f p : m_Points) s += ' ' + std::to_string(p.x) + ',' + std::to_string(p.y);
	s += "\" \n";
	s += "enemystate=\"" + std::to_string((int)m_EnemyState) + "\" \n";

	if (m_pCurrentPickup != nullptr)
		s += "pickup=*" + m_pCurrentPickup->ToString() + "* \n";

	s += "/>";

	return s;
}

std::vector<Point2f> Mobster::GetShape() const
{
	Matrix2x3 matTrans, matAngle, matScale, matCenter, matWorld;
	matTrans.SetAsTranslate(m_Position);
	matAngle.SetAsRotate(m_Angle * utils::g_Rad2Deg);
	matScale.SetAsScale(m_Scale);
	matCenter.SetAsTranslate(Vector2f{ -m_pTorsoAnimation->GetFrameWidth() / 2, -m_pTorsoAnimation->GetFrameHeight() / 2 });

	matWorld = matTrans * matAngle * matScale * matCenter;

	std::vector<Point2f> points;
	points = matWorld.Transform(Rectf{ 0, 0, m_pTorsoAnimation->GetFrameWidth(), m_pTorsoAnimation->GetFrameHeight() });

	return points;
}


void Mobster::SetCurrentPickup(Pickup* pickup, PickupManager& pickupManger)
{
	m_pCurrentPickup = pickup;

	delete m_pTorsoAnimation;
	m_pTorsoAnimation = new Animation(ResourceManager::GetTexture("Resources/Images/Enemy/Enemy" + m_pCurrentPickup->GetName() + ".png"), pickup->GetAnimRect(), m_pCurrentPickup->GetWalkFrames(), m_pCurrentPickup->GetAttackFramesPerSec());
}

void Mobster::RemovePickup(PickupManager& pickupManager)
{
	pickupManager.AddItem(m_pCurrentPickup, m_Position, Vector2f{ 0,0 });
	m_pCurrentPickup = nullptr;

	delete m_pTorsoAnimation;
	m_pTorsoAnimation = { new Animation("Resources/Anim/EnemyUnarmed.txt") };
}

void Mobster::UpdateIdle(float elapsedSec, const Level& level)
{
	m_Angle = m_IdleAngle;
	m_Velocity = Vector2f{ 0,0 };

	if (m_pCurrentPickup == nullptr)
		m_EnemyState = EnemyState::Moving;

	if (FoundPlayer(level.GetPlayer(), level) && m_pCurrentPickup != nullptr)
		m_EnemyState = EnemyState::Attack;
}

void Mobster::UpdateMoving(float elapsedSec, const Level& level)
{
	float minDistance{ 5 };

	Vector2f target{};
	if (m_pCurrentPickup != nullptr)
	{
		if (m_Points[(m_PointIndex + 1) % (int)m_Points.size()] == m_Points[m_PointIndex]) m_EnemyState = Enemy::EnemyState::Idle;
		else target = m_Points[m_PointIndex];
	}
	else
	{
		target = level.GetPickupManager()->ReturnClosest(m_Position)->GetPosition();

		if (utils::GetDistance(m_Position, target) < minDistance)
		{
			level.GetPickupManager()->HitItem(GetShape());
			SetCurrentPickup(level.GetPickupManager()->GetCollidedItem(), *level.GetPickupManager());
		}
	}

	m_Velocity = (target - m_Position).Normalized() * m_Speed * elapsedSec;
	m_Angle = atan2f(m_Velocity.y, m_Velocity.x);

	if (utils::GetDistance(m_Position, m_Points[m_PointIndex]) < minDistance)
	{
		if (m_Points[(m_PointIndex + 1) % (int)m_Points.size()] == m_Points[m_PointIndex]) m_EnemyState = Enemy::EnemyState::Idle;
		m_PointIndex = (m_PointIndex + 1) % (int)m_Points.size();

	}

	if (m_Velocity.SquaredLength() > 0)
	{
		m_pTorsoAnimation->Update(elapsedSec);
		m_pLegAnimation->Update(elapsedSec);
	}

	if (FoundPlayer(level.GetPlayer(), level) && m_pCurrentPickup != nullptr) m_EnemyState = Enemy::EnemyState::Attack;
}

void Mobster::UpdateUnconsious(float elapsedSec, const Level& level)
{
	if (level.Raycast(m_Position.ToPoint2f(), (m_Position + -GetForward() * (m_pTorsoAnimation->GetFrameWidth() / 2)).ToPoint2f(), false))
		m_pTorsoAnimation->SetStartPos(Point2f(0, m_pTorsoAnimation->GetFrameHeight() * 2));
	else
		m_pTorsoAnimation->SetStartPos(Point2f(0, m_pTorsoAnimation->GetFrameHeight()));

	m_pTorsoAnimation->ResetAnimation();

	if (m_pCurrentPickup != nullptr) RemovePickup(*level.GetPickupManager());

	m_Velocity = Vector2f{ 0,0 };

	m_Score = 600;

	m_Timer += elapsedSec;
	if (m_Timer > m_UnconsiousTime)
	{
		m_Timer = 0;
		m_EnemyState = EnemyState::Getup;
	}
}

void Mobster::UpdateGetUp(float elapsedSec, const Level& level)
{
	m_Timer += elapsedSec;
	int getupFrames{ 7 };

	if (level.Raycast(m_Position.ToPoint2f(), (m_Position + -GetForward() * (m_pTorsoAnimation->GetFrameWidth() / 2)).ToPoint2f(), false))
	{
		m_pTorsoAnimation->SetStartPos(Point2f(0, m_pTorsoAnimation->GetFrameHeight() * 2));
		getupFrames = 7;
	}
	else
	{
		m_pTorsoAnimation->SetStartPos(Point2f(0, m_pTorsoAnimation->GetFrameHeight()));
		getupFrames = 3;
	}

	m_pTorsoAnimation->Update(elapsedSec, getupFrames);

	if (m_Timer >= (m_pTorsoAnimation->GetFrameTime() * getupFrames))
	{
		m_pTorsoAnimation->SetStartPos(Point2f(0, m_pTorsoAnimation->GetFrameHeight() * 0));

		m_Timer = 0;
		m_EnemyState = EnemyState::Moving;
	}
}

void Mobster::UpdateAttack(float elapsedSec, Level& level)
{
	m_AttackTimer += elapsedSec;

	m_Velocity = (m_pPlayer->GetPosition() - m_Position).Normalized() * m_Speed * elapsedSec;
	m_Angle = atan2f(m_Velocity.y, m_Velocity.x);

	if (m_AttackTimer < m_AttackWait)
		return;

	m_Timer += elapsedSec;
	if (m_Timer <= elapsedSec)
		m_pCurrentPickup->GetWeapon()->Attack(m_Position, elapsedSec, GetForward(), level);

	m_pTorsoAnimation->SetStartPos(Point2f(0, m_pTorsoAnimation->GetFrameHeight()));
	m_pTorsoAnimation->Update(elapsedSec, m_pCurrentPickup->GetAttackFrames());

	float distanceToPlayer = utils::GetDistance(m_Position, m_pPlayer->GetPosition());

	if (distanceToPlayer <= m_pTorsoAnimation->GetFrameWidth() / 2)
		m_Velocity *= 0;

	if (m_Velocity.SquaredLength() > 0)
		m_pLegAnimation->Update(elapsedSec);
	else
		m_pLegAnimation->ResetAnimation();

	if (m_Timer >= (m_pTorsoAnimation->GetFrameTime() * m_pCurrentPickup->GetAttackFrames()))
	{
		m_Timer = 0;

		if (m_pPlayer->GetState() == Player::PlayerState::Dead)
		{
			m_pTorsoAnimation->SetStartPos(Point2f(0, m_pCurrentPickup->GetAnimRect().height * 2));
			m_pTorsoAnimation->SetFramesPerSec(m_pCurrentPickup->GetWalkFramesPerSec());

			m_Timer = 0;
			m_EnemyState = EnemyState::Moving;
		}
	}
}

void Mobster::UpdateDead(float elapsedSec, Level& level)
{
	if (m_Died)
		return;

	if (m_pCurrentPickup != nullptr)
	{
		RemovePickup(*level.GetPickupManager());
	}
	m_Velocity = Vector2f{ 0,0 };

	//Instatiate blood
	level.AddProp(new Blood(m_Position, m_Angle, m_Scale));

	for (int i = 0; i < 3 + rand() % 5; i++)
	{
		float randomAngle = (rand() % 360) * utils::g_Deg2Rad;
		level.AddProp(new Blood(m_Position, randomAngle, m_Scale, Vector2f(sinf(randomAngle), cosf(randomAngle)) * 50.f));
	}

	Game::GetHud()->AddHit(m_Score, m_Position.ToPoint2f());

	m_Died = true;
}
