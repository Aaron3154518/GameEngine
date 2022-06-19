#include "AssetManager.h"

// FontData
bool FontData::operator<(const FontData &rhs) const
{
	return w < rhs.w || (w == rhs.w && (h < rhs.h || (h == rhs.h &&
													  (sample.compare(rhs.sample) < 0 || (sample.compare(rhs.sample) == 0 &&
																						  file.compare(rhs.file) < 0)))));
}

// AssetManager
SharedTexture AssetManager::getTexture(std::string file)
{
	auto it = mTextures.find(file);
	if (it != mTextures.end())
	{
		return it->second;
	}
	else
	{
		struct stat buffer;
		if (stat(file.c_str(), &buffer) != 0)
		{
			std::cerr << "Could not find image file: " << file << std::endl;
			if (file != "src/default.png")
			{
				return getTexture("src/default.png");
			}
			else
			{
				return makeSharedTexture();
			}
		}
		SharedTexture tex = makeSharedTexture(IMG_LoadTexture(Renderer::get(), file.c_str()));
		mTextures[file] = tex;
		return tex;
	}
}

SharedFont AssetManager::getFont(const FontData &data)
{
	auto it = mFonts.find(data);
	if (it != mFonts.end())
	{
		return it->second;
	}
	else
	{
		// Min is always too small or just right, max is too big
		int minSize = 1, maxSize = 10;
		// If both dimensions are <= 0, use smallest font
		if (data.w > 0 || data.h > 0)
		{
			int w, h;
			getTextSize(data.file, minSize, data.sample, &w, &h);
			// While too small
			while ((data.w <= 0 || w <= data.w) && (data.h <= 0 || h <= data.h))
			{
				minSize = maxSize;
				maxSize *= 2;
				getTextSize(data.file, maxSize, data.sample, &w, &h);
			}
			// Terminate when maxSize (too big) is right after minSize (too small)
			while (maxSize - minSize > 1)
			{
				int size = (maxSize + minSize) / 2;
				getTextSize(data.file, size, data.sample, &w, &h);
				// Too big
				if ((data.w > 0 && w > data.w) || (data.h > 0 && h > data.h))
				{
					maxSize = size;
					// Too small or just right
				}
				else
				{
					minSize = size;
				}
			}
		}
		SharedFont font = makeSharedFont(TTF_OpenFont(data.file.c_str(), minSize));
		mFonts[data] = font;
		return font;
	}
}

bool AssetManager::getTextureSize(SDL_Texture *tex, int *w, int *h)
{
	return SDL_QueryTexture(tex, NULL, NULL, w, h) == 0;
}
void AssetManager::getFontSize(std::string fileName, int size, int *w, int *h)
{
	Font font = makeFont(TTF_OpenFont(fileName.c_str(), size));
	TTF_SizeText(font.get(), "_", w, NULL);
	if (h)
	{
		*h = TTF_FontHeight(font.get());
	}
	font.reset();
}
void AssetManager::getTextSize(std::string fileName, int size,
							   std::string sampleText, int *w, int *h)
{
	if (sampleText.empty())
	{
		getFontSize(fileName, size, w, h);
	}
	else
	{
		Font font = makeFont(TTF_OpenFont(fileName.c_str(), size));
		TTF_SizeText(font.get(), sampleText.c_str(), w, h);
	}
}