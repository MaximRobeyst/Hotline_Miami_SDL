#include "pch.h"
#include "Animation.h"
#include "Texture.h"
#include "ResourceManager.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "Game.h"


Animation::Animation(const std::string& animation)
	: m_pTexture{ nullptr }
	, m_StartPoint{ }
	, m_FrameWidth{  }
	, m_FrameHeight{  }
	, m_NrOfFrames{  }
	, m_NrOfFramesPerSec{  }
{
	std::ifstream inputFile(animation);
	std::string element;
	if (!inputFile) std::cerr << "Problem with opening " + animation << std::endl;

	while (std::getline(inputFile, element, '>'))
	{
		if (element.find("Animation") != std::string::npos) 
		{
			m_pTexture = ResourceManager::GetTexture(GetAttributeValue("Imagepath", element));
			//m_pTexture = new Texture(
			//	GetAttributeValue("Imagepath", element)
			//);
			m_StartPoint = ToPoint2f(GetAttributeValue("startpoint", element));
			m_FrameWidth = std::stof(GetAttributeValue("framewidth", element));
			m_FrameHeight = std::stof(GetAttributeValue("frameheight", element));
			m_NrOfFrames = std::stoi(GetAttributeValue("frames", element));
			m_NrOfFramesPerSec = std::stoi(GetAttributeValue("framespersec", element));
		}
	}
}

Animation::Animation(Texture* texture, Rectf frameRect, int nrOfFrames, int nrOfFramesPerSec)
	: m_pTexture{texture}
	, m_StartPoint{frameRect.left, frameRect.bottom }
	, m_FrameWidth{frameRect.width}
	, m_FrameHeight{frameRect.height}
	, m_NrOfFrames{nrOfFrames}
	, m_NrOfFramesPerSec{nrOfFramesPerSec}
{
}

Animation::~Animation()
{
	//delete m_pTexture;
}

void Animation::Draw() const
{
	m_pTexture->Draw(
		Point2f{},
		Rectf{ m_StartPoint.x + (m_FrameWidth * m_AnimFrame),m_StartPoint.y + m_FrameHeight, m_FrameWidth, m_FrameHeight }
	);
}

void Animation::Update(float elapsedsec)
{
	m_AnimTime += elapsedsec;

	if (m_AnimTime >= (1.0f / m_NrOfFramesPerSec))
	{
		++m_AnimFrame %= int(m_NrOfFrames);
		m_AnimTime -= (1.0f / m_NrOfFramesPerSec);
	}
}

void Animation::Update(float elapsedsec, int frames)
{
	m_AnimTime += elapsedsec;

	if (m_AnimTime >= (1.0f / m_NrOfFramesPerSec))
	{
		++m_AnimFrame %= int(frames);
		m_AnimTime -= (1.0f / m_NrOfFramesPerSec);
	}
}

void Animation::SetFrameFromTimer(float timer)
{
	m_AnimFrame = (int)(timer / (1.0f / m_NrOfFramesPerSec));
}

void Animation::SetStartPos(const Point2f& p)
{
	m_StartPoint = p;
}

void Animation::SetFramesPerSec(int framesPerSec)
{
	m_NrOfFramesPerSec = framesPerSec;
}

void Animation::SetAnimFrame(int frame)
{
	m_AnimFrame = frame;
}

void Animation::ResetAnimation()
{
	m_AnimTime = 0 ;
	m_AnimFrame = 0 ;
}

Texture* Animation::GetTexture() const
{
	return m_pTexture;
}

float Animation::GetFrameWidth() const
{
	return m_FrameWidth;
}

float Animation::GetFrameHeight() const
{
	return m_FrameHeight;
}

float Animation::GetFrameTime() const
{
	return 1.0f / m_NrOfFramesPerSec;
}

int Animation::GetNrOfFrames() const
{
	return m_NrOfFrames;
}

std::string Animation::GetAttributeValue(const std::string& attrName, const std::string& element, char seperator)
{
	std::string searchName{ attrName + "=" };

	size_t attributePosition = element.find(searchName);
	size_t openingQoute = element.find(seperator, attributePosition);
	size_t closingQoute = element.find(seperator, openingQoute + 1);

	std::string attribute;

	attribute = element.substr(openingQoute + 1, closingQoute - openingQoute - 1);

	return attribute;
}

Point2f Animation::ToPoint2f(const std::string& pointString)
{
	Point2f point;
	std::stringstream str{ pointString };

	str >> point.x;
	str.ignore();
	str >> point.y;
	return point;
}
