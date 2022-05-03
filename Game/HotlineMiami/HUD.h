#pragma once
#include <vector>

class Animation;
class Texture;
class Player;
class Level;
class Camera;
struct Vector2f;
class Game;

struct ScoreLabel {
	int m_Score;
	float m_LifeTime;
	Point2f m_Position;

	ScoreLabel(int score, Point2f position);
};

struct ComboLabel {
	int m_Combo;
	int m_Score;
	float m_LifeTime;
	Point2f m_Position;

	ComboLabel(int combo, int score,Point2f position);
};

class HUD final
{
public:
	HUD();
	HUD(const HUD& other) = delete;
	HUD& operator=(const HUD& other) = delete;
	HUD(HUD&& other) = delete;
	HUD& operator=(HUD&& other) = delete;
	~HUD();

	void DrawOffset(const Level& level) const;
	void DrawWindow(const Point2f& mousePos, const Level& level, const Window& window) const;
	void Update(float elapsedsec, const Level& level, const Window& window);

	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e, Game& game);

	int GetMaskIndex() const;
	int GetScore() const;
	std::string GetBottomRightText() const;

	void AddHit(int score, const Point2f& hit);
	void SetBottomRightText(const std::string& string);
	void Reset(int score = 0);
	void SetStageClear();

private:
	Animation* m_pCrossair;
	Texture* m_pFont[CHAR_MAX - ' '];
	Texture* m_pArrowTexture;
	Texture* m_pPalmTreeTexture;

	std::vector<ScoreLabel*> m_pScoreLabels;
	std::vector<ComboLabel*> m_pComboLabels;

	//Masks
	Texture* m_pUIMasks;
	// make const
	int m_MaskColumns{ 5 };
	int m_MaskRows{ 2 };

	int m_NrOfMasks{ 10 };
	int m_MaskIndex{ 0 };

	int m_DisplayScore{};
	float m_DisplaySpeed{ 600 };
	int m_Score{};
	bool m_ComboStart{ false };

	//Stage clear
	std::string m_StageClearText{ "Stage Clear" };
	int m_CurrChar{};
	float m_StageClearTime{ 2 };
	float m_Timer{ m_StageClearTime };
	float m_AnimTime{};

	float m_MaxComboTimer{ 2.5f };
	float m_ComboTimer{};
	int m_CurrentCombo{};

	float m_PosX{};
	float m_ScrollSpeed{ 100 };

	std::string m_BottomText{""};

	const Color4f m_Color1{ 0.97f, 0.4f ,0.92f, 1.f };
	const Color4f m_Color2{ 0.15f, 1.f, 0.96f, 1.f };

	//Game world
	void DrawLevelExit(const Level& level) const;
	void DrawHits() const;

	//Game window
	void DrawScore(const Player& player, const Window& window) const;
	void DrawBottomRight(const Player& player) const;
	void DrawDied(const Player& player, const Window& window) const;
	void DrawMaskSelection(const Player& player, const Window& window) const;
	void DrawEndScreen(const Window& window) const;

	void PrintString(const std::string& string, const Point2f& centerPos, float scale = 1.0f, float angle = 0.0f) const;
	Rectf GetStringRect(const std::string& string, const Point2f& centerPos, float scale = 1.0f) const;

	void DeleteScoreLabel(int index);
	void DeleteComboLabel(int index);
	float ScaleAnimation(float n) const;
	float AngleAnimation(float n, float maxAngle) const;
};

