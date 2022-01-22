#pragma once

/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <map>
#include <string>
#include <sdl2/SDL.h>

namespace engine
{
	class AssetManager {
	private:
		std::map<std::string, SDL_Texture*> textures;
		//TODO: Create a map for fonts.
		//TODO: Create a map for audio.
	public:
		AssetManager();
		~AssetManager();

		void ClearAssets();
		void AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath);
		SDL_Texture* GetTexture(const std::string& assetId);
	};
}