#pragma once
class Texture;
class Animation final
{
public:
	// Constructor, Destructor, etc...
	Animation(const std::string& animation);
	Animation(Texture* texture, Rectf frameRect, int nrOfFrames, int nrOfFramesPerSec);
	Animation(const Animation& other) = delete;
	Animation& operator=(const Animation& other) = delete;
	Animation(Animation&& other) = delete;
	Animation& operator=(Animation&& other) = delete;
	~Animation();

	void Draw() const;
	void Update(float elapsedsec);
	void Update(float elapsedsec, int frames);

	//Get & Set Functions
	void SetFrameFromTimer(float timer);
	void SetStartPos(const Point2f& p);
	void SetFramesPerSec(int framesPerSec);
	void SetAnimFrame(int frame);
	void ResetAnimation();

	Texture* GetTexture() const;
	float GetFrameWidth() const;
	float GetFrameHeight() const;
	float GetFrameTime() const;
	int GetNrOfFrames() const;

private:
	Texture* m_pTexture;
	Point2f m_StartPoint;

	float m_FrameWidth;
	float m_FrameHeight;

	int m_NrOfFrames;
	int m_NrOfFramesPerSec;

	float m_AnimTime{0};
	int m_AnimFrame{0};

	void CreateElement(const std::string& element);

	Point2f ToPoint2f(const std::string& pointString);
	std::string GetAttributeValue(const std::string& attrName, const std::string& element, char seperator = '\"');
};

