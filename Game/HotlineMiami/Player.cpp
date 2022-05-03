#include "pch.h"
#include "Player.h"
#include "Texture.h"
#include "Level.h"
#include "Animation.h"
#include "utils.h"
#include "Pickup.h"
#include "Matrix2x3.h"
#include "Game.h"
#include "PickupManager.h"
#include "EnemyManager.h"
#include "Weapon.h"
#include "Mellee.h"
#include "Enemy.h"
#include "SoundEffect.h"
#include "HUD.h"

#include <iostream>
#include "ResourceManager.h"
#include "Gun.h"

Player::Player(const Vector2f& position)
	: GameObject(position)
	, m_pMaskTexture{ ResourceManager::GetTexture("Resources/Images/Player/MaskSpriteSheet.png")}
	, m_pLegAnimation{ new Animation("Resources/Anim/PlayerLegs.txt") }
	, m_pTakeDownAnim{ new Animation("Resources/Anim/TakeoutAnim.txt") }
	//, m_pTorsoAnimation{ new Animation(new Texture("Resources/Images/Player/PlayerUnarmed.png"), Rectf{0, 0, 54, 26}, 8,(int)(8 * (5.0f / 3.0f))) }
	, m_pTorsoAnimation{new Animation("Resources/Anim/PlayerUnarmed.txt")}
	, m_pThrowSound{ ResourceManager::GetSoundEffect("Resources/Audio/Throw.wav")}
	, m_pSwingSound{ ResourceManager::GetSoundEffect("Resources/Audio/Swing1.wav")}
	, m_pPunchSound{ ResourceManager::GetSoundEffect("Resources/Audio/Punch.wav")}
{
	SetMaskRect(0);
}

Player::~Player()
{
	//delete m_pMaskTexture;
	delete m_pLegAnimation;
	delete m_pTorsoAnimation;
	delete m_pTakeDownAnim;

	if(m_pCurrentPickup != nullptr)
		delete m_pCurrentPickup;
}

void Player::Draw() const
{
	float legAngle = atan2f(m_Velocity.Normalized().y, m_Velocity.Normalized().x) - m_Angle;

	glPushMatrix();

		// Set translation, scale and rotation of the texture
		glTranslatef(m_Position.x, m_Position.y, 0);
		glRotatef(m_Angle * utils::g_Rad2Deg, 0, 0, 1);
		glScalef(m_Scale, m_Scale, 0);

		if (m_Playerstate == PlayerState::TakeDown) 
		{
			glTranslatef(-m_pTakeDownAnim->GetFrameWidth() / 2, -m_pTakeDownAnim->GetFrameHeight() / 2, 0);
			m_pTakeDownAnim->Draw();
			glTranslatef(m_pTakeDownAnim->GetFrameWidth() / 2, m_pTakeDownAnim->GetFrameHeight() / 2, 0);

			glTranslatef(-(m_pMaskTexture->GetWidth() / 5) / 2, -(m_pMaskTexture->GetHeight() / 2) / 2, 0);
			m_pMaskTexture->Draw(Point2f{}, m_MaskSrcRect);
		}
		else 
		{
			//draw the player legs
			if (m_Playerstate != PlayerState::Dead)
			{
				glRotatef(legAngle * utils::g_Rad2Deg, 0, 0, 1);
				glTranslatef(-m_pLegAnimation->GetFrameWidth() / 2, -m_pLegAnimation->GetFrameHeight() / 2, 0);
				m_pLegAnimation->Draw();
				glTranslatef(m_pLegAnimation->GetFrameWidth() / 2, m_pLegAnimation->GetFrameHeight() / 2, 0);
				glRotatef(-legAngle * utils::g_Rad2Deg, 0, 0, 1);
			}

			//draw the player Torso
			glTranslatef(-m_pTorsoAnimation->GetFrameWidth() / 2, -m_pTorsoAnimation->GetFrameHeight() / 2, 0);
			m_pTorsoAnimation->Draw();
			glTranslatef(m_pTorsoAnimation->GetFrameWidth() / 2, m_pTorsoAnimation->GetFrameHeight() / 2, 0);

			//Draw the mask 

			if (m_Playerstate != PlayerState::Dead)
			{
				glTranslatef(-(m_pMaskTexture->GetWidth() / 5) / 2, -(m_pMaskTexture->GetHeight() / 2) / 2, 0);
				m_pMaskTexture->Draw(Point2f{}, m_MaskSrcRect);
			}
		}
	glPopMatrix();
}

void Player::Update(float elapsedSec, Level& level)
{
	switch (m_Playerstate)
	{
	case PlayerState::Idle:
		UpdateIdle(elapsedSec, level);
		break;
	case PlayerState::Moving:
		UpdateMoving(elapsedSec, level);
		break;
	case PlayerState::Attack:
		UpdateAttack(elapsedSec, level);
		break;
	case PlayerState::Dead:
		UpdateDead(elapsedSec, level);
		return;
	case PlayerState::TakeDown:
		UpdateTakeDown(elapsedSec, level);
		return;
	}

	m_Angle = atan2f(m_MousePosition.y - m_Position.y, m_MousePosition.x - m_Position.x);
	level.HandleCollision(*this);
	m_Position += m_Velocity;
}

void Player::HandleCollision(const utils::HitInfo& hit)
{
	m_Position -= m_Velocity.Normalized() * hit.lambda;
	m_Velocity = m_Velocity.Normalized() * hit.lambda;
}

std::string Player::ToString() const
{
	return std::string();
}

void Player::ProcessKeyDownEvent(const SDL_KeyboardEvent& e, Level& level)
{
	switch (e.keysym.sym)
	{
	case SDLK_SPACE:
	
		if (m_Playerstate == PlayerState::TakeDown)
		{
			m_Playerstate = PlayerState::Idle;
			m_pTakedownTarget->SetEnemyState(Enemy::EnemyState::Unconsious);
			m_pTakedownTarget = nullptr;
			break;
		}
	
		if (level.GetEnemyManager()->EnemyHit(GetShape(), false))
		{
			m_pTakedownTarget = level.GetEnemyManager()->GetHitItem();
			if (level.GetEnemyManager()->GetHitItem()->GetEnemyState() == Enemy::EnemyState::Unconsious && !level.Raycast(m_Position.ToPoint2f(), level.GetEnemyManager()->GetHitItem()->GetPosition().ToPoint2f()))
			{
				m_Playerstate = PlayerState::TakeDown;
				m_pTakedownTarget->SetEnemyState(Enemy::EnemyState::Takeout);
				m_Position = m_pTakedownTarget->GetPosition();
				m_Angle = atan2f(m_pTakedownTarget->GetForward().y, m_pTakedownTarget->GetForward().x) + utils::g_Pi;
				if (level.Raycast(m_pTakedownTarget->GetPosition().ToPoint2f(), (m_pTakedownTarget->GetPosition() + GetForward() * (m_pTakeDownAnim->GetFrameWidth() / 2)).ToPoint2f(), false)) 
				{
					m_pTakeDownAnim->SetStartPos(Point2f{ 0, m_pTakeDownAnim->GetFrameHeight() * 1 });
					m_pTakeDownAnim->ResetAnimation();
					m_MaxBashes = 1;
				}
				else
				{
					m_pTakeDownAnim->SetStartPos(Point2f{ 0, 0 });
					m_pTakeDownAnim->ResetAnimation();
					m_MaxBashes = 3;
				}
				
			}
			else
			{
				m_pTakedownTarget = nullptr;
			}
		}
		break;
	}
}

void Player::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
}

void Player::SetMousePosition(const Point2f& mousePosition)
{
	m_MousePosition = mousePosition;
}

void Player::MouseDownEvent(const SDL_MouseButtonEvent& e, Level& level)
{
	if (m_Playerstate == PlayerState::Dead)
		return;

	switch ( e.button )
	{
	case SDL_BUTTON_LEFT:
		if (m_Playerstate == PlayerState::TakeDown) 
		{
			m_InAnimation = true;
			break;
		}

		m_Playerstate = PlayerState::Attack;
		if(m_pCurrentPickup != nullptr)
			m_pTorsoAnimation->SetFramesPerSec(m_pCurrentPickup->GetAttackFramesPerSec());
		m_HoldingLeftMouse = true;
		break;
	case SDL_BUTTON_RIGHT:
		// Pickup
		if (m_Playerstate == PlayerState::TakeDown)
			break;

		if (m_pCurrentPickup != nullptr)
			ThrowPickup(*level.GetPickupManager(), m_ThrowForce);
		else if (level.GetPickupManager()->HitItem(GetShape()))
			SetCurrentPickup(level.GetPickupManager()->GetCollidedItem(), *level.GetPickupManager());
		break;
	case SDL_BUTTON_MIDDLE:
		break;
	}
}

void Player::MouseUpEvent(const SDL_MouseButtonEvent& e)
{
	switch ( e.button )
	{
	case SDL_BUTTON_LEFT:
		m_HoldingLeftMouse = false;
		break;
	case SDL_BUTTON_RIGHT:
		break;
	case SDL_BUTTON_MIDDLE:
		break;
	}
}

bool Player::IsOverlapping(std::vector<Point2f> p)
{
	std::vector<Point2f> shape = GetShape();
	for (Point2f point : p)
	{
		if (utils::IsPointInPolygon(point, shape))
		{
			return true;
		}
	}
	return false;
}

std::vector<Point2f> Player::GetShape() const
{
	Matrix2x3 matTrans, matAngle, matScale,matCenter, matWorld;
	matTrans.SetAsTranslate(m_Position );
	matAngle.SetAsRotate(m_Angle * utils::g_Rad2Deg);
	matScale.SetAsScale(m_Scale);
	matCenter.SetAsTranslate(Vector2f{ -m_pTorsoAnimation->GetFrameWidth() / 2, -m_pTorsoAnimation->GetFrameHeight() / 2 });

	matWorld = matTrans * matAngle * matScale * matCenter;

	std::vector<Point2f> points;
	points = matWorld.Transform(Rectf{ 0, 0, m_pTorsoAnimation->GetFrameWidth(), m_pTorsoAnimation->GetFrameHeight() });

	return points;
}

Player::PlayerState Player::GetState() const
{
	return m_Playerstate;
}

Pickup* Player::GetCurrentPickup() const
{
	return m_pCurrentPickup;
}

void Player::SetState(PlayerState playerState)
{
	m_Playerstate = playerState;
}

void Player::SetCurrentPickup(Pickup* pickup, PickupManager& pickupManger)
{
	if (pickup == nullptr) 
	{
		delete m_pTorsoAnimation;
		m_pTorsoAnimation = new Animation("Resources/Anim/PlayerUnarmed.txt");
		Game::GetHud()->SetBottomRightText("");
		return;
	}

	if (m_pCurrentPickup != nullptr)
		ThrowPickup(pickupManger, m_ThrowForce);

	m_pCurrentPickup = pickup;

	delete m_pTorsoAnimation;
	m_pTorsoAnimation = new Animation(ResourceManager::GetTexture("Resources/Images/Player/Player" + m_pCurrentPickup->GetName() + ".png"), pickup->GetAnimRect(), pickup->GetWalkFrames(), pickup->GetWalkFramesPerSec());

	if (pickup->GetWeapon()->IsGun())
		Game::GetHud()->SetBottomRightText(static_cast<Gun*>(pickup->GetWeapon())->DisplayAmmo());
}

void Player::SetMaskRect(int maskIndex)
{
	m_MaskIndex = maskIndex;

	m_MaskSrcRect = 
		Rectf{
				(m_MaskIndex % 5) * m_pMaskTexture->GetWidth() / 5,
				((m_MaskIndex / 5) * (m_pMaskTexture->GetHeight() / 2)) + (m_pMaskTexture->GetHeight() / 2),
				m_pMaskTexture->GetWidth() / 5,
				m_pMaskTexture->GetHeight() / 2
		};
}

void Player::SetPosition(const Vector2f& pos)
{
	m_Position = pos;
}

Vector2f Player::GetForward() const
{
	return Vector2f(cosf(m_Angle) , sinf(m_Angle));
}

Vector2f Player::GetRight() const
{
	return Vector2f(cosf(m_Angle + (float)(M_PI / 2)), sinf(m_Angle + (float)(M_PI / 2)));
}

bool Player::GetMovementInput(float elapsedSec)
{
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);
	Vector2f playerInput;

	if (pStates[SDL_SCANCODE_W]) playerInput.y = 1;
	else if (pStates[SDL_SCANCODE_S]) playerInput.y = -1;

	if (pStates[SDL_SCANCODE_A]) playerInput.x = 1;
	else if (pStates[SDL_SCANCODE_D]) playerInput.x = -1;

	if (pStates[SDL_SCANCODE_TAB]) m_LocalMovement = !m_LocalMovement;

	if (m_LocalMovement)
		m_Velocity = ((GetForward() * playerInput.y) + (GetRight() * playerInput.x)) * m_Speed * elapsedSec;
	else
		m_Velocity = ((Vector2f{ 0,1 } *playerInput.y) + (Vector2f{ -1,0 } *playerInput.x)) * m_Speed * elapsedSec;
	//m_Velocity = playerInput * m_Speed * elapsedSec;

	return (playerInput.SquaredLength() != 0);
}

void Player::UpdateIdle(float elapsedSec, const Level& level)
{
	//std::cout << "Idle\n";

	m_pLegAnimation->ResetAnimation();
	m_pTorsoAnimation->ResetAnimation();

	if (GetMovementInput(elapsedSec)) m_Playerstate = PlayerState::Moving;
}

void Player::UpdateMoving(float elapsedSec, const Level& level)
{
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);
	if (!GetMovementInput(elapsedSec)) m_Playerstate = PlayerState::Idle;

	m_pLegAnimation->Update(elapsedSec);
	m_pTorsoAnimation->Update(elapsedSec);
}

void Player::UpdateAttack(float elapsedSec, Level& level)
{
	m_AttackSec += elapsedSec;

	if (m_pCurrentPickup == nullptr) 
	{
		AttackUnarmed(elapsedSec, level);
	}
	else 
	{
		m_pCurrentPickup->GetWeapon()->Update(elapsedSec);
		AttackWeapon(elapsedSec, level);
	}
}

void Player::UpdateDead(float elapsedSec, Level& level)
{
	if(m_pCurrentPickup != nullptr)
		ThrowPickup(*level.GetPickupManager(), 0);

	m_pTorsoAnimation->SetStartPos(Point2f(0, m_pTorsoAnimation->GetFrameHeight() * 2));
	m_pTorsoAnimation->ResetAnimation();

	Game::GetHud()->SetBottomRightText("Press r to restart");
}

void Player::UpdateTakeDown(float elapsedSec, Level& level)
{
	if (!m_InAnimation) 
	{
		m_Velocity = Vector2f{ 0,0 };
		m_pTakedownTarget->SetEnemyState(Enemy::EnemyState::Takeout);
		return;
	}

	m_AttackSec += elapsedSec;
	m_pTakeDownAnim->Update(elapsedSec, m_MaxBashes == 3 ? m_pTakeDownAnim->GetNrOfFrames() : m_pTakeDownAnim->GetNrOfFrames() * 2);

	if (m_AttackSec >= (m_pTakeDownAnim->GetFrameTime() * m_pTakeDownAnim->GetNrOfFrames())) 
	{
		m_Bashes++;
		m_AttackSec = 0;
		m_pPunchSound->Play(0);

		m_InAnimation = false;

		if (m_Bashes >= m_MaxBashes)
		{
			m_Bashes = 0;
			m_pTakedownTarget->SetEnemyState(Enemy::EnemyState::Dead);
			m_Playerstate = PlayerState::Idle;
		}
	}
}

void Player::AttackWeapon(float elapsedSec, Level& level)
{
	if (m_AttackSec <= elapsedSec)
	{
		m_pCurrentPickup->GetWeapon()->Attack(m_Position,elapsedSec, GetForward(), level);
	}

	m_pTorsoAnimation->SetStartPos(Point2f(0, m_pCurrentPickup->GetAnimRect().height));
	m_pTorsoAnimation->Update(elapsedSec, m_pCurrentPickup->GetAttackFrames());

	if (m_Velocity.SquaredLength() > 0)
		m_pLegAnimation->Update(elapsedSec);
	else
		m_pLegAnimation->ResetAnimation();

	GetMovementInput(elapsedSec);

	if (m_AttackSec >= (m_pTorsoAnimation->GetFrameTime() * m_pCurrentPickup->GetAttackFrames()))
	{
		m_AttackSec = 0;

		if (!m_HoldingLeftMouse)
		{
			m_pTorsoAnimation->SetStartPos(Point2f(0, m_pCurrentPickup->GetAnimRect().height * 2));
			m_pTorsoAnimation->SetFramesPerSec(m_pCurrentPickup->GetWalkFramesPerSec());

			if (m_Velocity.SquaredLength() == 0)
				m_Playerstate = PlayerState::Idle;
			else
				m_Playerstate = PlayerState::Moving;
		}
	}
}

void Player::AttackUnarmed(float elapsedSec, Level& level)
{
	if (m_AttackSec <= elapsedSec)
	{
		m_pSwingSound->Play(0);
		if(level.GetEnemyManager()->EnemyHit(m_Position, m_Angle - m_PunchAngle, m_Angle + m_PunchAngle, m_PunchRange, level, Enemy::EnemyState::Unconsious))
			m_pPunchSound->Play(0);
	}

	int attackFramesUnarmed{ 7 };

	m_pTorsoAnimation->SetStartPos(Point2f(0, m_pTorsoAnimation->GetFrameHeight()));
	m_pTorsoAnimation->Update(elapsedSec, attackFramesUnarmed);

	if (m_Velocity.SquaredLength() > 0)
		m_pLegAnimation->Update(elapsedSec);
	else
		m_pLegAnimation->ResetAnimation();

	GetMovementInput(elapsedSec);

	if (m_AttackSec >= (attackFramesUnarmed * m_pTorsoAnimation->GetFrameTime()))
	{
		//Attack in level (raycast to enemy and attack)

		if (!m_HoldingLeftMouse)
		{
			m_pTorsoAnimation->SetStartPos(Point2f(0, 0));

			m_AttackSec = 0;
			if (m_Velocity.SquaredLength() == 0)
				m_Playerstate = PlayerState::Idle;
			else
				m_Playerstate = PlayerState::Moving;
		}
	}
}

void Player::ThrowPickup(PickupManager& pickupManager, float throwForce)
{
	pickupManager.AddItem(m_pCurrentPickup, m_Position, GetForward() * throwForce);
	m_pCurrentPickup = nullptr;

	m_pThrowSound->Play(0);

	delete m_pTorsoAnimation;
	m_pTorsoAnimation = new Animation("Resources/Anim/PlayerUnarmed.txt");
	Game::GetHud()->SetBottomRightText("");
}
