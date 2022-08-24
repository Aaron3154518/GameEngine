#include "Shapes.h"

#include "TextureBuilder.h"

// ShapeData
void ShapeData::copy(const ShapeData &data) {
    color = data.color;
    blendMode = data.blendMode;
    boundary = data.boundary;
}

Rect ShapeData::getBounds() const {
    // Start with bounds as the target dimensions
    Rect bounds;
    SDL_Point size = Renderer::getTargetSize();
    bounds.setDim(size.x, size.y);
    if (boundary.empty()) {
        return bounds;
    }
    // Interesect screen and shape boundary
    SDL_Rect result;
    SDL_IntersectRect(boundary, bounds, &result);
    return result;
}

void ShapeData::draw(TextureBuilder &tex) const {}

// RectData
RectData &RectData::set() {
    r1 = r2 = Rect(0, 0, 0, 0);
    return *this;
}
RectData &RectData::set(const Rect &r) {
    r1 = Rect(0, 0, 0, 0);
    r2 = r;
    r2.normalize();
    return *this;
}
RectData &RectData::set(Rect r, int thickness, bool center) {
    r.normalize();
    r1 = r2 = r;
    if (center) {
        thickness = std::abs(thickness);
        r1.setDim(r1.w() - thickness, r1.h() - thickness, Rect::Align::CENTER);
        r2.setDim(r2.w() + thickness, r2.h() + thickness, Rect::Align::CENTER);
    } else {
        int dw = 2 * thickness;
        if (dw > 0) {
            r2.setDim(r2.w() + dw, r2.h() + dw, Rect::Align::CENTER);
        } else {
            r1.setDim(r1.w() + dw, r1.h() + dw, Rect::Align::CENTER);
        }
    }
    if (r1.invalid()) {
        r1 = Rect(0, 0, 0, 0);
    }
    return *this;
}

void RectData::draw(TextureBuilder &tex) const {
    Rect bounds = getBounds();
    if (!bounds.empty()) {
        SDL_Rect intersect;
        // Fill entire render target
        if (r2.empty()) {
            SDL_RenderFillRect(Renderer::get(), bounds);
            // Intersect r2 and bounds
        } else if (SDL_IntersectRect(r2, bounds, &intersect) == SDL_TRUE) {
            bounds = intersect;
            // Fill r2 if r is empty or not within bounds
            // Intersect r with bounds
            if (r1.empty() ||
                SDL_IntersectRect(bounds, r1, &intersect) == SDL_FALSE) {
                SDL_RenderFillRect(Renderer::get(), bounds);
                // Fill bounds except for r
            } else {
                // Start at r1
                Rect r = r1;
                // bounds is inclusive so draw once more when r = bounds
                float oldW, oldH;
                do {
                    oldW = r.w();
                    oldH = r.h();
                    // Expand rect
                    if (r.X() > bounds.X()) {
                        r.setWidth(r.w() + 1, Rect::Align::BOT_RIGHT);
                    }
                    if (r.Y() > bounds.Y()) {
                        r.setHeight(r.h() + 1, Rect::Align::BOT_RIGHT);
                    }
                    if (r.X2() < bounds.X2()) {
                        r.setWidth(r.w() + 1, Rect::Align::TOP_LEFT);
                    }
                    if (r.Y2() < bounds.Y2()) {
                        r.setHeight(r.h() + 1, Rect::Align::TOP_LEFT);
                    }
                    SDL_RenderDrawRect(Renderer::get(), r);
                } while (oldW != r.w() || oldH != r.h());
            }
        }
    }
}

// CircleData
CircleData &CircleData::set(SDL_Point _c, int r) {
    c = _c;
    r1 = 0;
    r2 = std::abs(r);
    return *this;
}
CircleData &CircleData::set(SDL_Point _c, int r, int thickness, bool center) {
    c = _c;
    r = std::abs(r);
    if (center) {
        r2 = r + std::abs(thickness) / 2;
    } else {
        r2 = thickness < 0 ? r : r + thickness;
    }
    r1 = std::max(r2 - std::abs(thickness), 0);
    return *this;
}
CircleData &CircleData::setFullCircle() {
    a1 = 0;
    a2 = TWO_PI;
    return *this;
}
CircleData &CircleData::setAngleRad(float _a1, float _a2) {
    if (_a1 < 0) {
        _a1 = fmodf(_a1, TWO_PI) + TWO_PI;
    }
    if (_a2 < 0) {
        _a2 = fmodf(_a2, TWO_PI) + TWO_PI;
    }
    a1 = fmodf(_a1, TWO_PI);
    a2 = fmodf(_a2, TWO_PI);
    return *this;
}
CircleData &CircleData::setAngleDeg(float _a1, float _a2) {
    return setAngleRad(_a1 * DEG_TO_RAD, _a2 * DEG_TO_RAD);
}
CircleData &CircleData::setDashed(unsigned int d) {
    dashes = d;
    return *this;
}

void CircleData::draw(TextureBuilder &tex) const {
    Rect bounds = getBounds();
    if (bounds.empty()) {
        return;
    }

    if (dashes > 0) {
        drawDashed(bounds);
        return;
    }

    float da = a2 - a1;
    if (da < 0) {
        da += TWO_PI;
    }

    // Draw full circle
    if (da == TWO_PI) {
        drawCircle(bounds);
    } else {
        drawSectors(bounds);
    }
}

void CircleData::drawSectors(const Rect &bounds) const {
    float da = a2 - a1;
    if (da < 0) {
        da += TWO_PI;
    }

    // Draw sectors
    float sA1 = 0, sA2 = a1;
    int sQuad = (int)(a1 / HALF_PI) + 1;
    std::vector<Sector> sectors;
    while (da > 0) {
        sA1 = fmodf(sA2, HALF_PI);
        float sDa = fminf(fminf(da, HALF_PI), HALF_PI - sA1);
        sA2 = sA1 + sDa;
        sectors.push_back(Sector{sA1, sA2, sQuad});
        sQuad++;
        if (sQuad == 5) {
            sQuad = 1;
        }
        da -= sDa;
    }

    for (Sector s : sectors) {
        drawSector(s, bounds);
    }
}

void CircleData::drawCircle(const Rect &bounds) const {
    // Circle
    int dx = -1;
    while (++dx < r2) {
        int dy1 = dx >= r1 ? 0 : (int)(sqrtf(r1 * r1 - dx * dx) + .5);
        int dy2 = (int)(sqrtf(r2 * r2 - dx * dx) + .5);
        // Iterate through dx, -dx
        do {
            int x = c.x + dx;
            // Make sure x is in bounds
            if (x >= bounds.X() && x <= bounds.X2()) {
                // Iterate through [dy1, dy2], [-dy2, -dy1]
                do {
                    int y1 = std::max(c.y + dy1, bounds.Y());
                    int y2 = std::min(c.y + dy2, bounds.Y2());
                    // Make sure at least one y is in bounds
                    if (y1 <= bounds.Y2() && y2 >= bounds.Y()) {
                        SDL_RenderDrawLine(Renderer::get(), x, y1, x, y2);
                    }
                    int tmp = -dy1;
                    dy1 = -dy2;
                    dy2 = tmp;
                } while (dy1 < 0);
            }
        } while ((dx *= -1) < 0);
    }
}

void CircleData::drawSector(const Sector &s, const Rect &bounds) const {
    const bool flip = s.quad == 3 || s.quad == 4;

    const float sinA1 = sin(s.a1);
    const float cosA1 = cos(s.a1);
    const float sinA2 = sin(s.a2);
    const float cosA2 = cos(s.a2);

    const SDL_FPoint p11{r1 * cosA1, r1 * sinA1}, p12{r1 * cosA2, r1 * sinA2},
        p21{r2 * cosA1, r2 * sinA1}, p22{r2 * cosA2, r2 * sinA2};
    const SDL_FPoint v1{p21.x - p11.x, p21.y - p11.y},
        v2{p22.x - p12.x, p22.y - p12.y};

    const bool m1Inf = v1.x == 0, m2Inf = v2.x == 0;
    const float m1 = m1Inf ? 0 : v1.y / v1.x, m2 = m2Inf ? 0 : v2.y / v2.x;
    const float b1 = p11.y - m1 * p11.x, b2 = p22.y - m2 * p22.x;

    for (int offX = ceilf(p12.x); offX <= p21.x; offX++) {
        int dy1 = (int)((offX < p11.x ? sqrtf(r1 * r1 - offX * offX) + .5
                                      : (m1Inf ? r1 : m1 * offX + b1)) +
                        .5);
        int dy2 = (int)((offX < p22.x ? (m2Inf ? r2 : m2 * offX + b2)
                                      : sqrtf(r2 * r2 - offX * offX) + .5) +
                        .5);

        int dx = offX;
        if (flip) {
            dx *= -1;
            dy1 *= -1;
            dy2 *= -1;
        }

        switch (s.quad) {
            case 1:
            case 3:
                SDL_RenderDrawLine(Renderer::get(), c.x + dx, c.y - dy1,
                                   c.x + dx, c.y - dy2);
                break;
            case 2:
            case 4:
                SDL_RenderDrawLine(Renderer::get(), c.x - dy1, c.y - dx,
                                   c.x - dy2, c.y - dx);
                break;
        };
    }
}

void CircleData::drawDashed(const Rect &bounds) const {
    const bool evenDashes = dashes % 2 == 0;
    const int numXs = (dashes - 1) / 2 + 2;
    std::vector<float> cosXs(numXs);
    const float da = M_PI / dashes;
    const float maxA = a1 < a2 ? a2 : a2 + TWO_PI;
    CircleData cd;
    cd.set(c, r1, r2 - r1);
    for (float sA1 = a1; sA1 < maxA; sA1 += da * 2) {
        cd.setAngleRad(sA1, fminf(sA1 + da, maxA)).drawSectors(bounds);
    }
}

// ProgressBar
ProgressBar &ProgressBar::set(const Rect &r) {
    rect = r;
    return *this;
}
ProgressBar &ProgressBar::set(SDL_Color foreground, SDL_Color background) {
    color = foreground;
    bkgrnd = background;
    return *this;
}
ProgressBar &ProgressBar::set(float amnt, float cap, bool log) {
    if (amnt < 0) {
        amnt = 0.;
    }
    if (cap < 0) {
        cap = 0.;
    }
    if (log) {
        if (amnt > 1) {
            amnt = log10(amnt);
        } else {
            amnt = 0;
        }
        if (cap > 1) {
            cap = log10(cap);
        } else {
            cap = 0;
        }
    }
    return set(cap == 0 ? 1. : amnt / cap);
}
ProgressBar &ProgressBar::set(float percent) {
    perc = fmax(0., fmin(1., percent));
    return *this;
}

void ProgressBar::draw(TextureBuilder &tex) const {
    Rect bounds = getBounds();
    if (!bounds.empty()) {
        RectData r;
        r.copy(*this);
        r.color = bkgrnd;
        tex.draw(r.set(rect));

        Rect progR = rect;
        progR.setWidth(progR.w() * perc);
        if (!progR.empty()) {
            r.color = color;
            tex.draw(r.set(progR));
        }
    }
}
