#pragma once
#include <map>

class Texture;
class SoundEffect;

// Gives memory leak detection but that's because it is in static memory and gets cleared after you quit the project
class ResourceManager final
{
public:
	static Texture* GetTexture(const std::string& filename);
	static SoundEffect* GetSoundEffect(const std::string& filename);
	static void ClearTextures();
	static void ClearSoundEffects();

	static void SetVolume(int volume);
private:
	static std::map<std::string, Texture*> m_pTextures;
	static std::map<std::string, SoundEffect*> m_pSoundEffects;
};

