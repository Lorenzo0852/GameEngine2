/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include <AssetManager/AssetManager.h>
#include <sdl2/SDL_image.h>

namespace engine
{
	AssetManager::AssetManager()
	{

	}

	AssetManager::~AssetManager()
	{
		ClearAssets();
	}

	void AssetManager::ClearAssets()
	{
		for (auto texture : textures)
		{
			SDL_DestroyTexture(texture.second);
		}
		textures.clear();
	}

	void AssetManager::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath)
	{
		SDL_Surface* surface = IMG_Load(filePath.c_str());
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

		//Add the texture to the map
		textures.emplace(assetId, texture);
	}

	SDL_Texture* AssetManager::GetTexture(const std::string& assetId)
	{
		return textures[assetId];
	}


}