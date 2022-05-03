#include "pch.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "SoundEffect.h"

#include <iostream>

std::map<std::string, Texture*> ResourceManager::m_pTextures;
std::map<std::string, SoundEffect*> ResourceManager::m_pSoundEffects;

Texture* ResourceManager::GetTexture(const std::string& filename)
{
	if (m_pTextures[filename] == nullptr)
		m_pTextures[filename] = new Texture(filename);

	return m_pTextures[filename];
}

SoundEffect* ResourceManager::GetSoundEffect(const std::string& filename)
{
	if (m_pSoundEffects[filename] == nullptr)
		m_pSoundEffects[filename] = new SoundEffect(filename);

	return m_pSoundEffects[filename];
}

void ResourceManager::ClearTextures()
{
	for (std::map<std::string, Texture*>::iterator itr = m_pTextures.begin(); itr != m_pTextures.end(); itr++)
	{
		delete (itr->second);
	}

	m_pTextures.clear();
}

void ResourceManager::ClearSoundEffects()
{
	for (std::map<std::string, SoundEffect*>::iterator itr = m_pSoundEffects.begin(); itr != m_pSoundEffects.end(); itr++)
	{
		delete (itr->second);
	}

	m_pSoundEffects.clear();
}

void ResourceManager::SetVolume(int volume)
{
	for (std::pair<std::string, SoundEffect*> p : m_pSoundEffects) 
	{
		p.second->SetVolume(volume);
	}
}
