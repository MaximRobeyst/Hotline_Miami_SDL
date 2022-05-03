#include "pch.h"
#include "HUD.h"
#include "Texture.h"
#include "Animation.h"
#include "utils.h"
#include "Player.h"
#include "Level.h"
#include "EnemyManager.h"
#include "Camera.h"
#include "Vector2f.h"
#include "ResourceManager.h"
#include <iostream>
#include "Game.h"


ScoreLabel::ScoreLabel(int score, Point2f position)
	: m_Score{ score }
	, m_LifeTime{}
	, m_Position{ position }
{
}

ComboLabel::ComboLabel(int combo, int score, Point2f position)
	:m_Combo{combo}
	,m_Score{score}
	,m_LifeTime{}
	,m_Position{position}
{
}

HUD::HUD()
	: m_pCrossair{new Animation("Resources/Anim/Crossair.txt")}
	, m_pArrowTexture(ResourceManager::GetTexture("Resources/Images/Arrow.png"))
	, m_pUIMasks(ResourceManager::GetTexture("Resources/Images/UIMasks.png"))
	, m_pPalmTreeTexture(ResourceManager::GetTexture("Resources/Images/Palmtree.png"))
	, m_pFont{nullptr}
{
	for (int i = 0; i < CHAR_MAX - ' '; i++) 
	{
		std::string s{ (char)(i + ' ') };

		m_pFont[i] = new Texture(s, "Resources/Images/Font.ttf", 26, Color4f{ 1,1,1,1 });
	}
}

HUD::~HUD()
{
	delete m_pCrossair;
	for (ScoreLabel* l : m_pScoreLabels)
		delete l;
	for (ComboLabel* l : m_pComboLabels)
		delete l;

	for (int i = 0; i < CHAR_MAX - ' '; i++)
	{
		std::string s{ (char)(i + ' ') };
		delete m_pFont[i];
	}
}

void HUD::DrawOffset(const Level& level) const
{
	if(level.GetEnemyManager()->GetStageClear())
	{
		DrawLevelExit(level);
	}
	DrawHits();
}

void HUD::DrawWindow(const Point2f& mousePos, const Level& level, const Window& window) const
{
	glPushMatrix();
	glTranslatef(mousePos.x, mousePos.y, 0);
	glTranslatef(-m_pCrossair->GetFrameWidth() / 2, -m_pCrossair->GetFrameWidth() / 2, 0);
	m_pCrossair->Draw();
	glPopMatrix();

	DrawScore(*level.GetPlayer(), window);

	if(!m_BottomText.empty())
		DrawBottomRight(*level.GetPlayer());

	if (level.GetPlayer()->GetState() == Player::PlayerState::Dead)
		DrawDied(*level.GetPlayer(), window);

	if (Game::GetState() == Game::GameState::MaskSelection)
		DrawMaskSelection(*level.GetPlayer(), window);
	if (Game::GetState() == Game::GameState::EndScreen)
		DrawEndScreen(window);

	if(m_CurrChar > 0)
		PrintString(m_StageClearText.substr(0, m_CurrChar), Point2f{ window.width / 2, window.height / 2 });
}

void HUD::Update(float elapsedsec, const Level& level, const Window& window)
{
	if (level.GetEnemyManager()->GetStageClear() && level.GetNextLevel().find("-") != std::string::npos)
	{
		m_BottomText = "Go to Car!";
	}

	m_PosX -= m_ScrollSpeed * elapsedsec;
	if (m_PosX < -window.width)
		m_PosX = 0;

	m_pCrossair->Update(elapsedsec);

	if (m_ComboStart) 
	{
		m_ComboTimer -= elapsedsec;
		if (m_ComboTimer <= 0)
		{
			if (m_CurrentCombo > 1)
			{
				m_pComboLabels.push_back(new ComboLabel(m_CurrentCombo, 350 * m_CurrentCombo, level.GetPlayer()->GetPosition().ToPoint2f()));
				m_Score += 350 * m_CurrentCombo;
			}
			m_ComboTimer = m_MaxComboTimer;
			m_CurrentCombo = 0;
			m_ComboStart = false;
		}
	}

	float labelLifeTime{ 1 };

	for (int i = 0; i < (int)m_pScoreLabels.size(); i++)
	{
		m_pScoreLabels[i]->m_LifeTime += elapsedsec;
		if (m_pScoreLabels[i]->m_LifeTime > labelLifeTime)
		{
			DeleteScoreLabel(i);
		}
	}

	for (int i = 0; i < (int)m_pComboLabels.size(); i++)
	{
		m_pComboLabels[i]->m_LifeTime += elapsedsec;
		if (m_pComboLabels[i]->m_LifeTime > labelLifeTime)
		{
			DeleteComboLabel(i);
		}
	}

	m_DisplayScore = (m_DisplayScore < m_Score) ? m_DisplayScore + (int)(m_DisplaySpeed * elapsedsec) : m_Score;

	if (m_Timer <= m_StageClearTime)
	{
		m_Timer += elapsedsec;
		if (m_CurrChar == m_StageClearText.size())
			return;

		m_AnimTime += elapsedsec;

		if (m_AnimTime >= (float)(m_StageClearText.size() / m_StageClearTime)) 
		{
			++m_CurrChar %= (int)m_StageClearText.size() + 1;
		}
	}

	if (m_Timer > m_StageClearTime&& m_CurrChar >= 0) 
	{
		m_AnimTime += elapsedsec;

		if (m_AnimTime >= (float)(m_StageClearText.size() / m_StageClearTime))
		{
			--m_CurrChar;
		}
	}

}

void HUD::ProcessKeyDownEvent(const SDL_KeyboardEvent& e, Game& game)
{
	if (game.GetState() != Game::GameState::MaskSelection)
		return;

	switch (e.keysym.sym)
	{
	case SDLK_SPACE:
		game.SetState(Game::GameState::Playing);
		game.GetLevel()->GetPlayer()->SetMaskRect(m_MaskIndex);
		break;
	case SDLK_UP:
		m_MaskIndex = (m_MaskIndex - 1) % m_NrOfMasks;
		break;
	case SDLK_DOWN:
		m_MaskIndex = (m_MaskIndex + 1) % m_NrOfMasks;
	}

	if (m_MaskIndex < 0)
		m_MaskIndex = m_NrOfMasks - 1;
}

void HUD::AddHit(int score, const Point2f& hit)
{
	m_CurrentCombo++;
	m_ComboTimer = m_MaxComboTimer;

	m_pScoreLabels.push_back(new ScoreLabel(score, hit));
	m_Score += score;
	m_ComboStart = true;
}

int HUD::GetMaskIndex() const
{
	return m_MaskIndex;
}

int HUD::GetScore() const
{
	return m_Score;
}

std::string HUD::GetBottomRightText() const
{
	return m_BottomText;
}

void HUD::SetBottomRightText(const std::string& string)
{
	m_BottomText = string;
}

void HUD::Reset(int score)
{
	m_Score = score;
	m_DisplayScore = score;

	m_BottomText = "";

	m_ComboTimer = m_MaxComboTimer;
	m_CurrentCombo = 0;
	m_ComboStart = false;
}

void HUD::SetStageClear()
{
	m_Timer = 0;
}

void HUD::DrawScore(const Player& player, const Window& window) const
{
	float padding = 5;
	float borderOffset = 30;

	std::string stringToPrint;
	if (player.GetState() == Player::PlayerState::Dead)
		stringToPrint = "You're Dead";
	else
		stringToPrint = std::to_string(m_DisplayScore) + " pts";

	Rectf textRect{ GetStringRect(stringToPrint, Point2f{}) };
	float width{ padding + textRect.width + padding };
	float height{ padding + textRect.height + padding };

	Point2f anchoredPos{ window.width - width - borderOffset,window.height - 100 };


	Rectf backgroundRect{ anchoredPos.x, anchoredPos.y, width + borderOffset, height };

	Point2f scorePos{ (backgroundRect.left + padding) + (textRect.width / 2), (backgroundRect.bottom + padding) + (textRect.height / 2) };

	utils::SetColor(Color4f{ 0,0,0,1 });
	utils::FillRect(backgroundRect);

	PrintString(stringToPrint, scorePos, 1.f, AngleAnimation(Game::GetElapsedTime(), 5.f * utils::g_Deg2Rad));
}

void HUD::DrawBottomRight(const Player& player) const
{
	float padding = 5;
	float borderOffset = 30;
	
	Rectf textRect{ GetStringRect(m_BottomText, Point2f{}) };
	Rectf backgroundRect{ 0, 100, (padding + textRect.width + padding) + borderOffset, padding + textRect.height + padding };
	
	Point2f scorePos{ (backgroundRect.left + padding + borderOffset) + (textRect.width / 2), (backgroundRect.bottom + padding) + (textRect.height / 2) };
	
	utils::SetColor(Color4f{ 0,0,0,1 });
	utils::FillRect(backgroundRect);
	
	PrintString(m_BottomText, scorePos, 1.f, AngleAnimation(Game::GetElapsedTime(), 5.f * utils::g_Deg2Rad));

}

void HUD::DrawDied(const Player& player, const Window& window) const
{
	Point2f scorePos{ 200, 100 };
	float padding = 5;
}

void HUD::DrawMaskSelection(const Player& player, const Window& window) const
{
	utils::SetColor(Color4f{ 0,0,0,1 });
	utils::FillRect(0, 0, window.width, window.height / 5.f);
	utils::FillRect(0, (4 * (window.height / 5.f)), window.width, window.height / 5.f);

	utils::SetColor(Color4f{ 1,1,1,1 });

	float maskTextureWidth{ m_pUIMasks->GetWidth() / m_MaskColumns };
	float maskTextureHeight{ m_pUIMasks->GetHeight() / m_MaskRows };
	float maxAngle{ 10.f };

	for (int i = 0; i < m_NrOfMasks; i++) 
	{
		glPushMatrix();
		float scale = m_MaskIndex == i ? 1.5f : 1.f;
		glTranslatef(window.width / m_MaskColumns, (window.height / m_MaskRows) - (maskTextureHeight * i) + (maskTextureHeight * m_MaskIndex), 0);
		glScalef(scale, scale, 0);
		if (m_MaskIndex == i) 
		{
			glRotatef(AngleAnimation(Game::GetElapsedTime(), maxAngle), 0, 0, 1);
		}

		glTranslatef(-maskTextureWidth / 2, -maskTextureHeight / 2, 0);
		m_pUIMasks->Draw(
			Point2f{},
			Rectf{
				(i % m_MaskColumns) * maskTextureWidth,
				((i / m_MaskColumns) * maskTextureHeight) + maskTextureHeight,
				maskTextureWidth,
				maskTextureHeight
			}
		);
		glTranslatef(maskTextureWidth / 2, -maskTextureHeight / 2, 0);
		glPopMatrix();
	}
}

void HUD::DrawEndScreen(const Window& window) const
{
	//background
	float sinX = sinf(Game::GetElapsedTime() + (utils::g_Pi / 2));

	utils::SetColor(
		Color4f{
			(m_Color1.r / 2) + sinX * ((m_Color1.r - m_Color2.r) / 2),
			(m_Color1.g / 2) + sinX * ((m_Color1.g - m_Color2.g) / 2),
			(m_Color1.b / 2) + sinX * ((m_Color1.b - m_Color2.b) / 2),
			1
		}
	);

	utils::FillRect(Rectf{ 0,0, window.width, window.height });

	//Drawings
	m_pPalmTreeTexture->Draw(Rectf{m_PosX,0,window.width,  m_pPalmTreeTexture->GetHeight() * (window.width / m_pPalmTreeTexture->GetWidth())});
	m_pPalmTreeTexture->Draw(Rectf{ window.width + m_PosX,0,window.width,  m_pPalmTreeTexture->GetHeight() * (window.width / m_pPalmTreeTexture->GetWidth()) });

	utils::SetColor(Color4f{ 0,0,0,1 });
	utils::FillRect(0, 0, window.width, window.height / 5.f);
	utils::FillRect(0, (4 * (window.height / 5.f)), window.width, window.height / 5.f);

	float scoreWidth{ (GetStringRect("Your score: " + std::to_string(m_Score), Point2f{0,0}).width / 2) };
	float messageWidth{ (GetStringRect("Thanks for playing!", Point2f{0,0}).width / 2) };

	PrintString("Thanks for playing!", Point2f{ window.width / 2, (window.height / 2) + 20 }, 1, AngleAnimation(Game::GetElapsedTime(), 2.5f * utils::g_Deg2Rad));
	PrintString("Your score:" + std::to_string(m_Score), Point2f{ window.width / 2, (window.height / 2) - 20 }, 1, AngleAnimation(Game::GetElapsedTime(), 2.5f * utils::g_Deg2Rad));
	PrintString("Press R to replay", Point2f{ window.width / 2, (window.height / 2) - 60 }, .75f, AngleAnimation(Game::GetElapsedTime(), 2.5f * utils::g_Deg2Rad));
}

void HUD::DrawLevelExit( const Level& level) const
{
	Rectf LevelEndRect{ level.GetLevelEnd() };
	Point2f center{ (LevelEndRect.left - (m_pArrowTexture->GetWidth() / 2)) + (LevelEndRect.width / 2), (LevelEndRect.bottom - (m_pArrowTexture->GetHeight() / 2)) + (LevelEndRect.height / 2) };

	utils::SetColor(Color4f{ 1,0,0,1 });
	m_pArrowTexture->Draw(center);
}

void HUD::DrawHits() const
{
	for (ScoreLabel* p : m_pScoreLabels)
	{
		Point2f labelPosition{ p->m_Position.x, p->m_Position.y + ((1 - cos((p->m_LifeTime * utils::g_Pi) / 2)) * 50) };

		PrintString(std::to_string(p->m_Score) + "pts", labelPosition, ScaleAnimation(p->m_LifeTime));
	}

	for (ComboLabel* p : m_pComboLabels)
	{
		Point2f labelPosition{ p->m_Position.x, p->m_Position.y + ((1 - cos((p->m_LifeTime * utils::g_Pi) / 2)) * 50) };

		PrintString(std::to_string(p->m_Combo) + "x Combo", Point2f(labelPosition.x, labelPosition.y + 10), ScaleAnimation(p->m_LifeTime));
		PrintString('+' + std::to_string(p->m_Score), labelPosition, ScaleAnimation(p->m_LifeTime));
	}
}

void HUD::PrintString(const std::string& string, const Point2f& centerPos, float scale, float angle) const
{
	//Word size
	Rectf wordRect{ GetStringRect(string, centerPos, scale) };

	Point2f previousPoint{ wordRect.left, wordRect.bottom };

	glPushMatrix();
	glTranslatef(centerPos.x, centerPos.y, 0);
	glRotatef(angle * utils::g_Rad2Deg, 0, 0, 1);
	for (int i = 0; i < (int)string.size(); i++) 
	{
		Texture* textureToPrint = m_pFont[(int)string.at(i) - ' '];

		Rectf dstRect{ previousPoint.x - centerPos.x, previousPoint.y - centerPos.y, textureToPrint->GetWidth() * scale, textureToPrint->GetHeight() * scale };
		Rectf srcRect{ 0, textureToPrint->GetHeight(), textureToPrint->GetWidth(), textureToPrint->GetHeight() };

		textureToPrint->Draw(dstRect, srcRect);
		previousPoint.x += textureToPrint->GetWidth() * scale;
	}
	glPopMatrix();
}

Rectf HUD::GetStringRect(const std::string& string, const Point2f& centerPos, float scale) const
{
	Rectf wordRect{};
	for (int i = 0; i < (int)string.size(); i++)
	{
		Texture* textureToPrint = m_pFont[(int)string.at(i) - ' '];
		wordRect.width += textureToPrint->GetWidth() * scale;
		wordRect.height = textureToPrint->GetHeight() * scale;
	}

	wordRect.left = centerPos.x - wordRect.width / 2;
	wordRect.bottom = centerPos.y - wordRect.height / 2;

	return wordRect;
}

void HUD::DeleteScoreLabel(int index)
{
	ScoreLabel* tmp = m_pScoreLabels[m_pScoreLabels.size() - 1];
	m_pScoreLabels[m_pScoreLabels.size() - 1] = m_pScoreLabels[index];
	m_pScoreLabels[index] = tmp;

	delete m_pScoreLabels[m_pScoreLabels.size() - 1];
	m_pScoreLabels.pop_back();
}

void HUD::DeleteComboLabel(int index)
{
	ComboLabel* tmp = m_pComboLabels[m_pComboLabels.size() - 1];
	m_pComboLabels[m_pComboLabels.size() - 1] = m_pComboLabels[index];
	m_pComboLabels[index] = tmp;

	delete m_pComboLabels[m_pComboLabels.size() - 1];
	m_pComboLabels.pop_back();
}

float HUD::ScaleAnimation(float n) const
{
	float halfMaxSize{ 0.25f };
	float changeFunction{ 0.9f };
	float frequency{ 15.f };

	return (n < changeFunction) ? (halfMaxSize + ((1 - powf(1 - n, 3)) * halfMaxSize)) : cosf(frequency*n - changeFunction)*(halfMaxSize * 2);
}

float HUD::AngleAnimation(float n, float maxAngle) const
{
	return sinf(n) * maxAngle;
}
