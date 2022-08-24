#include "Shapes.h"

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

void ShapeData::draw() const {}

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

void RectData::draw() const {
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

void CircleData::draw() const {
    Rect bounds = getBounds();
    if (!bounds.empty()) {
        // Circle
        int dx = -1;
        while (++dx < r2) {
            int dy1 = dx >= r1 ? 0 : (int)(std::sqrt(r1 * r1 - dx * dx) + .5);
            int dy2 = (int)(std::sqrt(r2 * r2 - dx * dx) + .5);
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

void ProgressBar::draw() const {
    Rect bounds = getBounds();
    if (!bounds.empty()) {
        RectData r;
        r.copy(*this);
        r.color = bkgrnd;
        r.set(rect).draw();

        Rect progR = rect;
        progR.setWidth(progR.w() * perc);
        if (!progR.empty()) {
            r.color = color;
            r.set(progR).draw();
        }
    }
}
