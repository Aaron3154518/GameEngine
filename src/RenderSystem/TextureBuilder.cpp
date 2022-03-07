#include "TextureBuilder.h"

// TextureBuilder
TextureBuilder::TextureBuilder(int w, int h, SDL_Color bkgrnd) {
	reset(w, h, bkgrnd);
}
TextureBuilder::TextureBuilder(SharedTexture src) {
	int w, h;
	if (getTextureSize(src.get(), &w, &h)) {
		reset(w, h);

		Renderer::setRenderTarget(mTex.get());
		RenderData data;
		data.dest = Rect(0, 0, w, h);
		data.texture = src;
		draw(data);
		Renderer::resetRenderTarget();
	} else {
#ifndef RENDER_DEBUG
		std::cerr << "TextureBuilder(): "
			<< "could not query source texture" << std::endl;
#endif
	}
}

// Get texture
SharedTexture TextureBuilder::getTexture() {
	return mTex;
}

// Start new texture
void TextureBuilder::reset(int w, int h, SDL_Color bkgrnd) {
	mTex = makeSharedTexture(SDL_CreateTexture(Renderer::get(),
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h));
	SDL_SetTextureBlendMode(mTex.get(), SDL_BLENDMODE_BLEND);
	RectData r;
	r.color = bkgrnd;
	r.blendMode = SDL_BLENDMODE_BLEND;
	drawRect(r.set());
}

// Draw textures/text
void TextureBuilder::draw(const RenderData& data) {
	Renderer::setRenderTarget(mTex.get());

	Rect renderBounds;
	Renderer::getTargetSize(&renderBounds.w, &renderBounds.h);
	// Make sure we are actually drawing something
	if (data.dest.empty()) {
#ifdef RENDER_DEBUG
		std::cerr << "draw(): Empty destination rect" << std::endl;
#endif
		return;
	}
	// Check the texture to draw
	if (!data.texture) {
#ifdef RENDER_DEBUG
		std::cerr << "draw(): Invalid Texture" << std::endl;
#endif
		return;
	}
	// Get the boundary rect
	Rect boundary = data.boundary;
	if (boundary.empty()) {
		boundary = renderBounds;
	} else if (SDL_IntersectRect(&boundary, &renderBounds, &boundary)
		== SDL_FALSE) {
#ifdef RENDER_DEBUG
		std::cerr << "draw(): Boundary rect " << boundary
			<< " was outside the screen: " << renderBounds << std::endl;
#endif
		return;
	}

	Rect destRect = data.dest;
	// Get fraction of item to draw
	double leftFrac = fmax(boundary.x - destRect.x, 0) / destRect.w;
	double topFrac = fmax(boundary.y - destRect.y, 0) / destRect.h;
	double rightFrac = fmax(destRect.x2() - boundary.x2(), 0) / destRect.w;
	double botFrac = fmax(destRect.y2() - boundary.y2(), 0) / destRect.h;
	// Make sure the rect is actually in the boundary
	if (leftFrac + rightFrac >= 1 || topFrac + botFrac >= 1) {
#ifdef RENDER_DEBUG
		std::cerr << "draw(): Rect " << destRect << " was out side the boundary " <<
			boundary << std::endl;
#endif
		return;
	}

	destRect = Rect(destRect.x + destRect.w * leftFrac,
		destRect.y + destRect.h * topFrac,
		destRect.w * (1. - leftFrac - rightFrac),
		destRect.h * (1. - topFrac - botFrac));
	int w, h;
	if (!getTextureSize(data.texture.get(), &w, &h)) {
#ifdef RENDER_DEBUG
		std::cerr << "draw(): Unable to query texture size: "
			<< SDL_GetError() << std::endl;
#endif
		return;
	}
	Rect areaRect = data.area.invalid() ? Rect(0, 0, w, h) : data.area;
	Rect texRect = Rect(areaRect.x + areaRect.w * leftFrac,
		areaRect.y + areaRect.h * topFrac,
		areaRect.w * (1. - leftFrac - rightFrac),
		areaRect.h * (1. - topFrac - botFrac));
	// Make sure at least one pixel will be drawn
	if (texRect.empty()) {
#ifdef RENDER_DEBUG
		std::cerr << "draw(): Can't draw from " << texRect << " to " << destRect
			<< std::endl;
#endif
		return;
	}

	SDL_RenderCopy(Renderer::get(), data.texture.get(), &texRect, &destRect);

	Renderer::resetRenderTarget();
}
// Draw shapes
void TextureBuilder::startDrawShape(const ShapeData& data) {
	Renderer::setRenderTarget(mTex.get());
	Renderer::setBlendMode(data.blendMode);
	Renderer::setDrawColor(data.color);
}
Rect TextureBuilder::getShapeBounds(const ShapeData& data) {
	// Start with bounds as the target dimensions
	Rect bounds; bounds.resize(Renderer::getTargetSize(), false);
	if (data.boundary.empty()) {
		return bounds;
	}
	// Interesect screen and shape boundary
	SDL_IntersectRect(&data.boundary, &bounds, &bounds);
	return bounds;
}
void TextureBuilder::endDrawShape() {
	Renderer::resetDrawColor();
	Renderer::resetBlendMode();
	Renderer::resetRenderTarget();
}
void TextureBuilder::drawRect(const RectData& data) {
	startDrawShape(data);
	Rect bounds = getShapeBounds(data);
	if (!bounds.empty()) {
		// Fill entire render target
		if (data.r2.empty()) {
			SDL_RenderFillRect(Renderer::get(), &bounds);
		// Intersect r2 and bounds
		} else if (SDL_IntersectRect(&data.r2, &bounds, &bounds) == SDL_TRUE) {
			// Start at r1
			Rect r = data.r1;
			// Fill r2 if r is empty or not within bounds
			// Intersect r with bounds
			if (r.empty() || SDL_IntersectRect(&bounds, &r, &r) == SDL_FALSE) {
				SDL_RenderFillRect(Renderer::get(), &bounds);
			// Fill bounds except for r
			} else {
				// bounds is inclusive so draw once more when r = bounds
				do {
					// Expand rect
					if (r.x > bounds.x) { r.x--; r.w++; }
					if (r.y > bounds.y) { r.y--; r.h++; }
					if (r.x2() < bounds.x2()) { r.w++; }
					if (r.y2() < bounds.y2()) { r.h++; }
					SDL_RenderDrawRect(Renderer::get(), &r);
				} while (r != bounds);
			}
		}
	}
	endDrawShape();
}
void TextureBuilder::drawCircle(const CircleData& data) {
	startDrawShape(data);
	Rect bounds = getShapeBounds(data);
	if (!bounds.empty()) {
		// Circle
		int dx = -1;
		while (++dx < data.r2) {
			int dy1 = dx >= data.r1 ? 0 : (int)(std::sqrt(data.r1 * data.r1 - dx * dx) + .5);
			int dy2 = (int)(std::sqrt(data.r2 * data.r2 - dx * dx) + .5);
			// Iterate through dx, -dx
			do {
				int x = data.c.x + dx;
				// Make sure x is in bounds
				if (x >= bounds.x && x <= bounds.x2()) {
					// Iterate through [dy1, dy2], [-dy2, -dy1]
					do {
						int y1 = std::max(data.c.y + dy1, bounds.y);
						int y2 = std::min(data.c.y + dy2, bounds.y2());
						// Make sure at least one y is in bounds
						if (y1 <= bounds.y2() && y2 >= bounds.y) {
							SDL_RenderDrawLine(Renderer::get(), x, y1, x, y2);
						}
						int tmp = -dy1; dy1 = -dy2; dy2 = tmp;
					} while (dy1 < 0);
				}
			} while ((dx *= -1) < 0);
		}
	}
	endDrawShape();
}
void TextureBuilder::drawProgressBar(const ProgressBar& data) {
	startDrawShape(data);
	Rect bounds = getShapeBounds(data);
	if (!bounds.empty()) {
		RectData r;
		r.copy(data);
		r.color = data.bkgrnd;
		drawRect(r.set(data.r));
		Rect progR(data.r.x, data.r.y, data.r.w * data.perc, data.r.h);
		if (!progR.empty()) {
			r.color = data.color;
			drawRect(r.set(progR));
		}
	}
	endDrawShape();
}

// Brighten texture
void TextureBuilder::brighten(Uint8 strength) {
	int w, h;
	if (getTextureSize(mTex.get(), &w, &h)) {
		RectData r;
		r.color = SDL_Color{ strength,strength,strength,255 };
		r.blendMode = SDL_BLENDMODE_ADD;
		drawRect(r.set());
	} else {
#ifndef RENDER_DEBUG
		std::cerr << "brighten: "
			<< "could not query source texture" << std::endl;
#endif
	}
}

bool TextureBuilder::getTextureSize(SDL_Texture* tex, int* w, int* h) {
	return SDL_QueryTexture(tex, NULL, NULL, w, h) == 0;
}