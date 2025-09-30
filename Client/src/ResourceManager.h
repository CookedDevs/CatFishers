#pragma once
#include <raylib.h>
#include <map>
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

	static inline std::map<std::string, loadTexture> loadedTextures;
};
