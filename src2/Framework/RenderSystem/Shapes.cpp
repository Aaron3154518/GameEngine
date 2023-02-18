#include "Shapes.h"

namespace Shapes {
// Shape Attributes
void ShapeColor::setColor(SDL_Color c) { color = c; }

void ShapeBlendmode::setBlendMode(SDL_BlendMode b) { blend = b; }

void ShapeBoundary::setBoundary(const Rect &b) { bounds = b; }
Rect ShapeBoundary::getBounds() {  // Start with bounds as the target dimensions
    Rect b;
    SDL_Point size = Renderer::getTargetSize();
    b.setDim(size.x, size.y);
    if (bounds.empty()) {
        return b;
    }
    // Intersect screen and shape boundary
    SDL_Rect result;
    SDL_IntersectRect(bounds, b, &result);
    return result;
}

// Rectangle
const Rectangle::Data &Rectangle::operator()() const { return data; }

void Rectangle::set(const Rect &r) {
    data.r1 = Rect(0, 0, 0, 0);
    data.r2 = r;
    data.r2.normalize();
}
void Rectangle::set(const Rect &r1, const Rect &r2) {
    SDL_Rect res;
    SDL_IntersectRect(r1, r2, &res);
    data.r1 = res;
    data.r2 = r2;
}
void Rectangle::set(Rect r, int thickness, bool center) {
    r.normalize();
    data.r1 = data.r2 = r;
    if (center) {
        thickness = std::abs(thickness);
        data.r1.setDim(data.r1.w() - thickness, data.r1.h() - thickness,
                       Rect::Align::CENTER);
        data.r2.setDim(data.r2.w() + thickness, data.r2.h() + thickness,
                       Rect::Align::CENTER);
    } else {
        int dw = 2 * thickness;
        if (dw > 0) {
            data.r2.setDim(data.r2.w() + dw, data.r2.h() + dw,
                           Rect::Align::CENTER);
        } else {
            data.r1.setDim(data.r1.w() + dw, data.r1.h() + dw,
                           Rect::Align::CENTER);
        }
    }
    if (data.r1.invalid()) {
        data.r1 = Rect(0, 0, 0, 0);
    }
}

void Rectangle::draw(TextureBuilder &tex) {
    Renderer::setDrawColor(color);
    Renderer::setBlendMode(blend);

    Rect bounds = getBounds();
    if (bounds.empty()) {
        return;
    }

    // Fill entire render target
    if (data.r2.empty()) {
        SDL_RenderFillRect(Renderer::get(), bounds);
        return;
    }

    SDL_Rect intersect;
    // Intersect r2 and bounds
    if (SDL_IntersectRect(data.r2, bounds, &intersect) == SDL_FALSE) {
        return;
    }

    bounds = intersect;
    // Intersect r1 with bounds
    if (data.r1.empty() ||
        SDL_IntersectRect(bounds, data.r1, &intersect) == SDL_FALSE) {
        // Fill intersection if r1 is empty or not within bounds
        SDL_RenderFillRect(Renderer::get(), bounds);
        return;
    }

    // Fill bounds except for r
    Rectangle rs = *this;
    // Left, right, top, bottom
    for (Rect side :
         {Rect(data.r2.x(), data.r2.y(), data.r1.x() - data.r2.x(),
               data.r2.y2() - data.r2.y()),
          Rect(data.r1.x2(), data.r2.y(), data.r2.x2() - data.r1.x2(),
               data.r2.y2() - data.r2.y()),
          Rect(data.r2.x(), data.r2.y(), data.r2.x2() - data.r2.x(),
               data.r1.y() - data.r2.y()),
          Rect(data.r2.x(), data.r1.y2(), data.r2.x2() - data.r2.x(),
               data.r2.y2() - data.r1.y2())}) {
        if (!side.empty()) {
            rs.set(side);
            rs.draw(tex);
        }
    }
}

// Circle
const Circle::Data &Circle::operator()() const { return data; }

void Circle::setCenter(SDL_Point c) { data.c = c; }
void Circle::setRadius(int r) {
    data.r1 = 0;
    data.r2 = std::abs(r);
}
void Circle::setRadius(int r, int thickness, bool center) {
    r = std::abs(r);
    if (center) {
        data.r2 = r + std::abs(thickness) / 2;
    } else {
        data.r2 = thickness < 0 ? r : r + thickness;
    }
    data.r1 = std::max(data.r2 - std::abs(thickness), 0);
}
void Circle::setFullCircle() {
    data.a1 = 0;
    data.a2 = TWO_PI;
}
void Circle::setAngleRad(float a1, float a2) {
    if (a1 < 0) {
        a1 = fmodf(a1, TWO_PI) + TWO_PI;
    }
    if (a2 < 0) {
        a2 = fmodf(a2, TWO_PI) + TWO_PI;
    }
    data.a1 = fmodf(a1, TWO_PI);
    data.a2 = fmodf(a2, TWO_PI);
}
void Circle::setAngleDeg(float a1, float a2) {
    return setAngleRad(a1 * DEG_TO_RAD, a2 * DEG_TO_RAD);
}
void Circle::setDashed(unsigned int d) { data.dashes = d; }

void Circle::draw(TextureBuilder &tex) {
    Renderer::setDrawColor(color);
    Renderer::setBlendMode(blend);

    Rect bounds = getBounds();
    if (bounds.empty()) {
        return;
    }

    if (data.dashes > 0) {
        drawDashed(bounds);
        return;
    }

    float da = data.a2 - data.a1;
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

void Circle::drawCircle(const Rect &bounds) const {
    // Circle
    int dx = -1;
    while (++dx < data.r2) {
        int dy1 =
            dx >= data.r1 ? 0 : (int)(sqrtf(data.r1 * data.r1 - dx * dx) + .5);
        int dy2 = (int)(sqrtf(data.r2 * data.r2 - dx * dx) + .5);
        // Iterate through dx, -dx
        do {
            int x = data.c.x + dx;
            // Make sure x is in bounds
            if (x >= bounds.X() && x <= bounds.X2()) {
                // Iterate through [dy1, dy2], [-dy2, -dy1]
                do {
                    int y1 = std::max(data.c.y + dy1, bounds.Y());
                    int y2 = std::min(data.c.y + dy2, bounds.Y2());
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

void Circle::drawSectors(const Rect &bounds) const {
    float da = data.a2 - data.a1;
    if (da < 0) {
        da += TWO_PI;
    }

    // Draw sectors
    float sA1 = 0, sA2 = data.a1;
    int sQuad = (int)(data.a1 / HALF_PI) + 1;
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

void Circle::drawSector(const Sector &s, const Rect &bounds) const {
    const bool flip = s.quad == 3 || s.quad == 4;

    const float sinA1 = sin(s.a1);
    const float cosA1 = cos(s.a1);
    const float sinA2 = sin(s.a2);
    const float cosA2 = cos(s.a2);

    const SDL_FPoint p11{data.r1 * cosA1, data.r1 * sinA1},
        p12{data.r1 * cosA2, data.r1 * sinA2},
        p21{data.r2 * cosA1, data.r2 * sinA1},
        p22{data.r2 * cosA2, data.r2 * sinA2};
    const SDL_FPoint v1{p21.x - p11.x, p21.y - p11.y},
        v2{p22.x - p12.x, p22.y - p12.y};

    const bool m1Inf = v1.x == 0, m2Inf = v2.x == 0;
    const float m1 = m1Inf ? 0 : v1.y / v1.x, m2 = m2Inf ? 0 : v2.y / v2.x;
    const float b1 = p11.y - m1 * p11.x, b2 = p22.y - m2 * p22.x;

    for (int offX = ceilf(p12.x); offX <= p21.x; offX++) {
        int dy1 =
            (int)(.5 + (offX < p11.x ? sqrtf(data.r1 * data.r1 - offX * offX)
                                     : (m1Inf ? data.r1 : m1 * offX + b1)));
        int dy2 =
            (int)(.5 + (offX < p22.x ? (m2Inf ? data.r2 : m2 * offX + b2)
                                     : sqrtf(data.r2 * data.r2 - offX * offX)));

        int dx = offX;
        if (flip) {
            dx *= -1;
            dy1 *= -1;
            dy2 *= -1;
        }

        switch (s.quad) {
            case 1:
            case 3:
                SDL_RenderDrawLine(Renderer::get(), data.c.x + dx,
                                   data.c.y - dy1, data.c.x + dx,
                                   data.c.y - dy2);
                break;
            case 2:
            case 4:
                SDL_RenderDrawLine(Renderer::get(), data.c.x - dy1,
                                   data.c.y - dx, data.c.x - dy2,
                                   data.c.y - dx);
                break;
        };
    }
}

void Circle::drawDashed(const Rect &bounds) const {
    const int numXs = (data.dashes - 1) / 2 + 2;
    std::vector<float> cosXs(numXs);
    const float da = M_PI / data.dashes;
    const float maxA = data.a1 <= data.a2 ? data.a2 : data.a2 + TWO_PI;
    Circle cd;
    cd.setCenter(data.c);
    cd.setRadius(data.r1, data.r2 - data.r1);
    for (float sA1 = data.a1; sA1 < maxA; sA1 += da * 2) {
        cd.setAngleRad(sA1, fminf(sA1 + da, maxA));
        cd.drawSectors(bounds);
    }
}

// ProgressBar
const ProgressBar::Data &ProgressBar::operator()() const { return data; }

void ProgressBar::set(const Rect &r) { data.rect = r; }
void ProgressBar::set(SDL_Color foreground, SDL_Color background) {
    data.fgrnd = foreground;
    data.bkgrnd = background;
}
void ProgressBar::set(float amnt, float cap, bool log) {
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
    set(cap == 0 ? 1. : amnt / cap);
}
void ProgressBar::set(float percent) {
    data.perc = fmax(0., fmin(1., percent));
}

void ProgressBar::draw(TextureBuilder &tex) {
    Renderer::setBlendMode(blend);

    Rect bounds = getBounds();
    if (!bounds.empty()) {
        Rectangle r;
        r.setBlendMode(blend);
        r.setBoundary(bounds);
        r.setColor(data.bkgrnd);
        r.set(data.rect);
        tex.draw(r);

        Rect progR = data.rect;
        progR.setWidth(progR.w() * data.perc);
        if (!progR.empty()) {
            r.setColor(data.fgrnd);
            r.set(progR);
            tex.draw(r);
        }
    }
}

Rect ProgressBar::getFullRect() const {
    return Rect(data.rect.x(), data.rect.y(), data.rect.w() * data.perc,
                data.rect.h());
}
Rect ProgressBar::getEmptyRect() const {
    float w = data.rect.w() * (1 - data.perc);
    return Rect(data.rect.x2() - w, data.rect.y(), w, data.rect.h());
}
}  // namespace Shapes