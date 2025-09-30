#include "ResourceManager.h"
#include "CatLog.h"
#include <string>

Texture2D* LoadedTextures::LoadTex(std::string texture)
{
	if (loadedTextures.find(texture) == loadedTextures.end())
	{
		Texture2D texture2d = LoadTexture(texture.c_str());
		if (texture2d.id <= 0)
		{
			CatCore::LogError(std::string("Could not load texture : ") + texture + "!!\n");
			return nullptr;
		}
		loadedTextures[texture] = { texture2d, 1 };
	}
	else
	{
		loadedTextures[texture].uses += 1;
		CatCore::LogError(std::string("Loaded: ") + texture + " : " + std::to_string(loadedTextures[texture].uses) + "\n");
	}

	return &loadedTextures[texture].texture;
}

void LoadedTextures::UnLoadTex(std::string texture)
{
	if (loadedTextures.find(texture) != loadedTextures.end())
	{
		if (loadedTextures[texture].uses <= 1)
		{
			UnloadTexture(loadedTextures[texture].texture);
			loadedTextures.erase(texture);
		}
		else
		{
			loadedTextures[texture].uses -= 1;
			CatCore::LogError(std::string("Unloaded: ") + texture + " : " + std::to_string(loadedTextures[texture].uses) + "\n");
		}
	}
	else
		CatCore::LogError(std::string("Could not find loaded texture : ") + texture + "!!\n");
}

Texture2D* LoadedTextures::GetTex(std::string texture)
{
	if (loadedTextures.find(texture) != loadedTextures.end())
		return &loadedTextures[texture].texture;
	else
		return nullptr;
}

void LoadedTextures::UnLoadAllTex()
{
	if (loadedTextures.size() <= 0) return;

	for (auto texture : loadedTextures)
	{
		if (!texture.first.empty()) UnloadTexture(texture.second.texture);
		loadedTextures.erase(texture.first);
	}
}