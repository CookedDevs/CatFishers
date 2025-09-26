#include "ResourceManager.h"

Texture2D* LoadedTextures::LoadTex(std::string texture)
{
	if (loadedTextures.find(texture) == loadedTextures.end())
	{
		Texture2D texture2d = LoadTexture(texture.c_str());
		if (texture2d.id <= 0)
		{
			std::cout << "Could not load texture : " << texture << " !!\n";
			return nullptr;
		}
		loadedTextures[texture] = { texture2d, 1 };
	}
	else
		loadedTextures[texture].uses += 1;

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
			loadedTextures[texture].uses -= 1;
	}
	else
		std::cout << "Could not find loaded texture : " << texture << " !!\n";
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
		UnloadTexture(texture.second.texture);
		loadedTextures.erase(texture.first);
	}
}