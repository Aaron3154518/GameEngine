#include "TestComponents.h"

// TestBase
TestBase::TestBase(Rect r, int e) : mPos(std::make_shared<UIComponent>(r, e)) {}

const Rect &TestBase::getRect() const {
    return mPos->rect;
}

int TestBase::getElevation() const {
    return mPos->elevation;
}

SDL_Color TestBase::getColor() const {
    return BLACK;
}

void TestBase::init(GameStruct &gs) {}

void TestBase::onRender(SDL_Renderer *renderer) {
    SDL_Color c = getColor();
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(renderer, &mPos->rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &mPos->rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

// ClickRenderTest
ClickRenderTest::ClickRenderTest(Rect r, int e) : TestBase(r, e) {
    Game::registerComponent(this);
}

SDL_Color ClickRenderTest::getColor() const {
    return color ? GREEN : RED;
}

void ClickRenderTest::init(GameStruct &gs) {
    mMouseSub = ServiceHandler::Get<MouseService>()->mouse$.subscribe(
        std::bind(&ClickRenderTest::onClick, this, std::placeholders::_1, std::placeholders::_2),
        mPos);
    mRenderSub = ServiceHandler::Get<RenderService>()->render$.subscribe(
        std::bind(&ClickRenderTest::onRender, this, std::placeholders::_1), mPos);
}

void ClickRenderTest::onClick(Event::MouseButton b, bool clicked) {
    color = clicked;
}

// ChangeSubTest
ChangeSubTest::ChangeSubTest(Rect r, int e) : TestBase(r, e) {
    Game::registerComponent(this);
}

SDL_Color ChangeSubTest::getColor() const {
    return color ? YELLOW : BLUE;
}

void ChangeSubTest::init(GameStruct &gs) {
    mMouseSub = ServiceHandler::Get<MouseService>()->mouse$.subscribe(
        std::bind(&ChangeSubTest::onClick, this, std::placeholders::_1, std::placeholders::_2),
        mPos);
    mRenderSub = ServiceHandler::Get<RenderService>()->render$.subscribe(
        std::bind(&ChangeSubTest::onRender, this, std::placeholders::_1), mPos);
}

void ChangeSubTest::onClick(Event::MouseButton b, bool clicked) {
    if (clicked) {
        color = !color;
        if (color) {
            mRenderSub->unsubscribe();
            mRenderSub.reset();
        } else {
            mRenderSub = ServiceHandler::Get<RenderService>()->render$.subscribe(
                std::bind(&ChangeSubTest::onRender, this, std::placeholders::_1), mPos);
        }
    }
}

// UnsubTest
UnsubTest::UnsubTest(Rect r, int e) : TestBase(r, e) {
    Game::registerComponent(this);
}

SDL_Color UnsubTest::getColor() const {
    return color ? ORANGE : PURPLE;
}

void UnsubTest::init(GameStruct &gs) {
    mMouseSub = ServiceHandler::Get<MouseService>()->mouse$.subscribe(
        std::bind(&UnsubTest::onClick, this, std::placeholders::_1, std::placeholders::_2),
        mPos);
    mRenderSub = ServiceHandler::Get<RenderService>()->render$.subscribe(
        std::bind(&UnsubTest::onRender, this, std::placeholders::_1), mPos);
}

void UnsubTest::onClick(Event::MouseButton b, bool clicked) {
    if (clicked) {
        color = !color;
        if (color) {
            mRenderSub->changeSubscription([](SDL_Renderer *) {});
        } else {
            mRenderSub->changeSubscription(std::bind(&UnsubTest::onRender, this, std::placeholders::_1));
        }
    }
}

// UpdateTest
UpdateTest::UpdateTest(Rect r, int e) : TestBase(r, e) {
    Game::registerComponent(this);
}

SDL_Color UpdateTest::getColor() const {
    SDL_Color color = BLACK;
    if (delayMs > 0) {
        color.r = color.b = color.g = 255 * (std::pow(delayMs / 1500., .15) - 1);
    }
    return color;
}

void UpdateTest::init(GameStruct &gs) {
    mUpdateSub = ServiceHandler::Get<UpdateService>()->update$.subscribe(
        std::bind(&UpdateTest::onUpdate, this, std::placeholders::_1));
    mMouseSub = ServiceHandler::Get<MouseService>()->mouse$.subscribe(
        std::bind(&UpdateTest::onClick, this, std::placeholders::_1, std::placeholders::_2),
        mPos);
    mRenderSub = ServiceHandler::Get<RenderService>()->render$.subscribe(
        std::bind(&UpdateTest::onRender, this, std::placeholders::_1), mPos);
}

void UpdateTest::onClick(Event::MouseButton b, bool clicked) {
    if (clicked) {
        delayMs = 1500;
    }
}

void UpdateTest::onUpdate(Time dt) {
    if (delayMs > 0) {
        delayMs -= dt;
    }
}

// VisibilityTest
VisibilityTest::VisibilityTest(Rect r, int e) : TestBase(r, e) {
    Game::registerComponent(this);
}

void VisibilityTest::init(GameStruct &gs) {
    mUpdateSub = ServiceHandler::Get<UpdateService>()->update$.subscribe(
        std::bind(&VisibilityTest::onUpdate, this, std::placeholders::_1));
    mMouseSub = ServiceHandler::Get<MouseService>()->mouse$.subscribe(
        std::bind(&VisibilityTest::onClick, this, std::placeholders::_1, std::placeholders::_2),
        mPos);
    mRenderSub = ServiceHandler::Get<RenderService>()->render$.subscribe(
        std::bind(&VisibilityTest::onRender, this, std::placeholders::_1), mPos);
}

void VisibilityTest::onClick(Event::MouseButton b, bool clicked) {
    if (clicked) {
        mPos->visible = false;
        delayMs = 1500;
    }
}

void VisibilityTest::onUpdate(Time dt) {
    if (delayMs > 0) {
        delayMs -= dt;
        if (delayMs <= 0) {
            mPos->visible = true;
        }
    }
}

// InheritanceTestBase
InheritanceTestBase::InheritanceTestBase(Rect r, int e) : TestBase(r, e) {
    Game::registerComponent(this);
}

SDL_Color InheritanceTestBase::getColor() const {
    return color;
}

void InheritanceTestBase::init(GameStruct &gs) {
    ServiceHandler::Get<MouseService>()->mouse$.updateSubscription(
        mMouseSub,
        [this](Event::MouseButton b, bool clicked) {
            if (clicked) {
                if (increaseColor) {
                    color.r = std::min(255, color.r + colorInc);
                    if (color.r == 255) {
                        increaseColor = false;
                    }
                } else {
                    color.r = std::max(0, color.r - colorInc);
                    if (color.r == 0) {
                        increaseColor = true;
                    }
                }
            }
        },
        mPos);
    mRenderSub = ServiceHandler::Get<RenderService>()->render$.subscribe(
        std::bind(&InheritanceTestBase::onRender, this, std::placeholders::_1), mPos);
}

// InheritanceTestDerived
InheritanceTestDerived::InheritanceTestDerived(Rect r, int e) : InheritanceTestBase(r, e) {
    Game::registerComponent(this);
}

void InheritanceTestDerived::init(GameStruct &gs) {
    ServiceHandler::Get<MouseService>()->mouse$.updateSubscription(
        mMouseSub,
        [this](Event::MouseButton b, bool clicked) {
            if (clicked) {
                if (increaseColor) {
                    color.g = std::min(255, color.g + colorInc);
                    if (color.g == 255) {
                        increaseColor = false;
                    }
                } else {
                    color.g = std::max(0, color.g - colorInc);
                    if (color.g == 0) {
                        increaseColor = true;
                    }
                }
            }
        },
        mPos);
}

// Generate random test component
std::shared_ptr<TestBase> randomTestComponent(int w, int h) {
    Rect r;
    r.x = rand() % w - 10;
    r.y = rand() % h - 10;
    r.w = rand() % (w - r.x);
    r.h = rand() % (h - r.y);
    int e = rand() % 20 - 10;
    switch (rand() % 3) {
        case 1:
            return std::make_shared<ChangeSubTest>(r, e);
        case 2:
            return std::make_shared<UnsubTest>(r, e);
        default:
            return std::make_shared<ClickRenderTest>(r, e);
    }
}