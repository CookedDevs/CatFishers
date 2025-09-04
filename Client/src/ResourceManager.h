#pragma once
#include <raylib.h>
#include <unordered_map>
#include <iostream>

class LoadedTextures
{
public:
	static Texture2D* LoadTex(std::string texture);
	static void UnLoadTex(std::string texture);
	static Texture2D* GetTex(std::string texture);
	static void UnLoadAllTex();
private:
	struct loadTexture
	{
		Texture2D texture;
		unsigned int uses;
	};

	static inline std::unordered_map <std::string, loadTexture> loadedTextures;
};
