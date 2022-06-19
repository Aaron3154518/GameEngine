#include "Renderer.h"

Renderer::RendererPtr Renderer::mRenderer =
	Renderer::RendererPtr(NULL, SDL_DestroyRenderer);
std::stack<SDL_Texture *> Renderer::targetStack;
std::stack<SDL_Color> Renderer::colorStack;
std::stack<SDL_BlendMode> Renderer::blendStack;

// Renderer functions
void Renderer::init(SDL_Window *window)
{
	if (!window)
	{
		std::cerr << "Invalid SDL_Window for initialization" << std::endl;
		return;
	}
	mRenderer.reset(SDL_CreateRenderer(window, -1, 0));
	setDrawColor(BLACK);
	setRenderTarget(NULL);
	setBlendMode(SDL_BLENDMODE_BLEND);
}
SDL_Renderer *Renderer::get()
{
	return mRenderer.get();
}
SDL_Point Renderer::getTargetSize()
{
	SDL_Point size;
	getTargetSize(&size.x, &size.y);
	return size;
}
void Renderer::getTargetSize(int *w, int *h)
{
	SDL_GetRendererOutputSize(mRenderer.get(), w, h);
}

// Render target, draw color, and blend mode stacks
void Renderer::setRenderTarget(SDL_Texture *tex)
{
	if (SDL_SetRenderTarget(mRenderer.get(), tex) != 0)
	{
		std::cerr << "Unable to set render target" << std::endl;
	}
	else
	{
		targetStack.push(tex);
	}
}
void Renderer::changeRenderTarget(SDL_Texture *tex)
{
	if (targetStack.size() > 1)
	{
		targetStack.pop();
	}
	setRenderTarget(tex);
}
void Renderer::resetRenderTarget()
{
	if (targetStack.size() > 1)
	{
		targetStack.pop();
		setRenderTarget(targetStack.top());
		targetStack.pop();
	}
}
void Renderer::setDrawColor(const SDL_Color &c)
{
	SDL_SetRenderDrawColor(mRenderer.get(), c.r, c.g, c.b, c.a);
	colorStack.push(c);
}
void Renderer::changeDrawColor(const SDL_Color &c)
{
	if (colorStack.size() > 1)
	{
		colorStack.pop();
	}
	setDrawColor(c);
}
void Renderer::resetDrawColor()
{
	if (colorStack.size() > 1)
	{
		colorStack.pop();
		setDrawColor(colorStack.top());
		colorStack.pop();
	}
}
void Renderer::setBlendMode(SDL_BlendMode mode)
{
	SDL_SetRenderDrawBlendMode(mRenderer.get(), mode);
	blendStack.push(mode);
}
void Renderer::changeBlendMode(SDL_BlendMode mode)
{
	if (blendStack.size() > 1)
	{
		blendStack.pop();
	}
	setBlendMode(mode);
}
void Renderer::resetBlendMode()
{
	if (blendStack.size() > 1)
	{
		blendStack.pop();
		setBlendMode(blendStack.top());
		blendStack.pop();
	}
}