#ifndef RENDER_DATA_H
#define RENDER_DATA_H

#include <Framework/RenderSystem/Shapes.h>
#include <Framework/RenderSystem/TextureBuilder.h>
#include <Utils/Rect.h>

struct RenderData : public Drawable, public Shapes::ShapeBoundary {
    RenderData();
    RenderData(SharedTexture tex);

    SharedTexture mTex;
    Rect mRect, mArea = Rect(0, 0, 1, 1);

    void draw(TextureBuilder& tex);
};

#endif