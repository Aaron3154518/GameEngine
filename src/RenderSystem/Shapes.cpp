#include "Shapes.h"

// ShapeData
void ShapeData::copy(const ShapeData &data) {
    color = data.color;
    blendMode = data.blendMode;
    boundary = data.boundary;
}

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
        r1.resize(r1.w - thickness, r1.h - thickness, true);
        r2.resize(r2.w + thickness, r2.h + thickness, true);
    } else {
        int dw = 2 * thickness;
        if (dw > 0) {
            r2.resize(r2.w + dw, r2.h + dw, true);
        } else {
            r1.resize(r1.w + dw, r1.h + dw, true);
        }
    }
    if (r1.invalid()) {
        r1 = Rect(0, 0, 0, 0);
    }
    return *this;
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
ProgressBar &ProgressBar::set(double amnt, double cap, bool log) {
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
ProgressBar &ProgressBar::set(double percent) {
    perc = fmax(0., fmin(1., percent));
    return *this;
}
