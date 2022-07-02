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

void TestBase::init() {}

void TestBase::onRender(SDL_Renderer *renderer) {
    SDL_Color c = getColor();
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(renderer, &mPos->rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &mPos->rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

// ClickRenderTest
ClickRenderTest::ClickRenderTest(Rect r, int e) : TestBase(r, e) {}

SDL_Color ClickRenderTest::getColor() const {
    return color ? GREEN : RED;
}

void ClickRenderTest::init() {
    mMouseSub = ServiceSystem::Get<MouseService>()->Get<MouseObservable>()->subscribe(
        std::bind(&ClickRenderTest::onClick, this, std::placeholders::_1, std::placeholders::_2),
        mPos);
    mMouseSub->setUnsubscriber(unsub);
    mRenderSub = ServiceSystem::Get<RenderService>()->Get<RenderObservable>()->subscribe(
        std::bind(&ClickRenderTest::onRender, this, std::placeholders::_1), mPos);
    mRenderSub->setUnsubscriber(unsub);
}

void ClickRenderTest::onClick(Event::MouseButton b, bool clicked) {
    color = clicked;
}

// ChangeSubTest
ChangeSubTest::ChangeSubTest(Rect r, int e) : TestBase(r, e) {}

SDL_Color ChangeSubTest::getColor() const {
    return color ? ORANGE : PURPLE;
}

void ChangeSubTest::init() {
    mMouseSub = ServiceSystem::Get<MouseService>()->Get<MouseObservable>()->subscribe(
        std::bind(&ChangeSubTest::onClick, this, std::placeholders::_1, std::placeholders::_2),
        mPos);
    mMouseSub->setUnsubscriber(unsub);
    mRenderSub = ServiceSystem::Get<RenderService>()->Get<RenderObservable>()->subscribe(
        std::bind(&ChangeSubTest::onRender, this, std::placeholders::_1), mPos);
    mRenderSub->setUnsubscriber(unsub);
}

void ChangeSubTest::onClick(Event::MouseButton b, bool clicked) {
    if (clicked) {
        color = !color;
        if (color) {
            mRenderSub->changeSubscription([](SDL_Renderer *) {});
        } else {
            mRenderSub->changeSubscription(std::bind(&ChangeSubTest::onRender, this, std::placeholders::_1));
        }
    }
}

// UnsubTest
UnsubTest::UnsubTest(Rect r, int e) : TestBase(r, e) {}

SDL_Color UnsubTest::getColor() const {
    return color ? YELLOW : BLUE;
}

void UnsubTest::init() {
    mMouseSub = ServiceSystem::Get<MouseService>()->Get<MouseObservable>()->subscribe(
        std::bind(&UnsubTest::onClick, this, std::placeholders::_1, std::placeholders::_2),
        mPos);
    mMouseSub->setUnsubscriber(unsub);
    mRenderSub = ServiceSystem::Get<RenderService>()->Get<RenderObservable>()->subscribe(
        std::bind(&UnsubTest::onRender, this, std::placeholders::_1), mPos);
    mRenderSub->setUnsubscriber(unsub);
}

void UnsubTest::onClick(Event::MouseButton b, bool clicked) {
    if (clicked) {
        color = !color;
        if (color) {
            mRenderSub->unsubscribe();
        } else {
            mRenderSub = ServiceSystem::Get<RenderService>()->Get<RenderObservable>()->subscribe(
                std::bind(&UnsubTest::onRender, this, std::placeholders::_1), mPos);
        }
    }
}

// UpdateTest
UpdateTest::UpdateTest(Rect r, int e) : TestBase(r, e) {}

SDL_Color UpdateTest::getColor() const {
    SDL_Color color = BLACK;
    if (delayMs > 0) {
        color.r = color.b = color.g = 255 * (std::pow(delayMs / 1500., .15) - 1);
    }
    return color;
}

void UpdateTest::init() {
    mUpdateSub = ServiceSystem::Get<UpdateService>()->Get<UpdateObservable>()->subscribe(
        std::bind(&UpdateTest::onUpdate, this, std::placeholders::_1));
    mUpdateSub->setUnsubscriber(unsub);
    mMouseSub = ServiceSystem::Get<MouseService>()->Get<MouseObservable>()->subscribe(
        std::bind(&UpdateTest::onClick, this, std::placeholders::_1, std::placeholders::_2),
        mPos);
    mMouseSub->setUnsubscriber(unsub);
    mRenderSub = ServiceSystem::Get<RenderService>()->Get<RenderObservable>()->subscribe(
        std::bind(&UpdateTest::onRender, this, std::placeholders::_1), mPos);
    mRenderSub->setUnsubscriber(unsub);
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
VisibilityTest::VisibilityTest(Rect r, int e) : TestBase(r, e) {}

void VisibilityTest::init() {
    mUpdateSub = ServiceSystem::Get<UpdateService>()->Get<UpdateObservable>()->subscribe(
        std::bind(&VisibilityTest::onUpdate, this, std::placeholders::_1));
    mUpdateSub->setUnsubscriber(unsub);
    mMouseSub = ServiceSystem::Get<MouseService>()->Get<MouseObservable>()->subscribe(
        std::bind(&VisibilityTest::onClick, this, std::placeholders::_1, std::placeholders::_2),
        mPos);
    mMouseSub->setUnsubscriber(unsub);
    mRenderSub = ServiceSystem::Get<RenderService>()->Get<RenderObservable>()->subscribe(
        std::bind(&VisibilityTest::onRender, this, std::placeholders::_1), mPos);
    mRenderSub->setUnsubscriber(unsub);
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
const Uint8 InheritanceTestBase::COLOR_INC = 50;

InheritanceTestBase::InheritanceTestBase(Rect r, int e) : TestBase(r, e) {}

SDL_Color InheritanceTestBase::getColor() const {
    return color;
}

void InheritanceTestBase::init() {
    ServiceSystem::Get<MouseService>()->Get<MouseObservable>()->updateSubscription(
        mMouseSub,
        std::bind(&InheritanceTestBase::onClick, this, std::placeholders::_1, std::placeholders::_2, true),
        mPos);
    mMouseSub->setUnsubscriber(unsub);
    mRenderSub = ServiceSystem::Get<RenderService>()->Get<RenderObservable>()->subscribe(
        std::bind(&InheritanceTestBase::onRender, this, std::placeholders::_1), mPos);
    mRenderSub->setUnsubscriber(unsub);
}

void InheritanceTestBase::onClick(Event::MouseButton b, bool clicked, bool red) {
    if (clicked) {
        Uint8 &val = red ? color.r : color.g;
        if (increaseColor) {
            val = std::min(255, val + COLOR_INC);
            if (val == 255) {
                increaseColor = false;
            }
        } else {
            val = std::max(0, val - COLOR_INC);
            if (val == 0) {
                increaseColor = true;
            }
        }
    }
}

// InheritanceTestDerived
InheritanceTestDerived::InheritanceTestDerived(Rect r, int e) : InheritanceTestBase(r, e) {}

void InheritanceTestDerived::init() {
    InheritanceTestBase::init();
    ServiceSystem::Get<MouseService>()->Get<MouseObservable>()->updateSubscription(
        mMouseSub,
        std::bind(&InheritanceTestDerived::onClick, this, std::placeholders::_1, std::placeholders::_2, false),
        mPos);
}

// MultiUnsubTest
MultiUnsubTest::MultiUnsubTest(Rect r, int e) : TestBase(r, e) {
    mUpdateSubs = std::vector<UpdateObservable::SubscriptionPtr>(3);
}

SDL_Color MultiUnsubTest::getColor() const {
    return ctr == 0 ? MAGENTA : BLUE;
}

void MultiUnsubTest::init() {
    for (auto updateSub : mUpdateSubs) {
        updateSub = ServiceSystem::Get<UpdateService>()->Get<UpdateObservable>()->subscribe(
            std::bind(&MultiUnsubTest::onUpdate, this, std::placeholders::_1));
        updateSub->setUnsubscriber(updateUnsub);
    }
    mMouseSub = ServiceSystem::Get<MouseService>()->Get<MouseObservable>()->subscribe(
        std::bind(&MultiUnsubTest::onClick, this, std::placeholders::_1, std::placeholders::_2),
        mPos);
    mMouseSub->setUnsubscriber(unsub);
    mRenderSub = ServiceSystem::Get<RenderService>()->Get<RenderObservable>()->subscribe(
        std::bind(&MultiUnsubTest::onRender, this, std::placeholders::_1), mPos);
    mRenderSub->setUnsubscriber(unsub);
}

void MultiUnsubTest::onUpdate(Time dt) {
    ctr++;
}

void MultiUnsubTest::onClick(Event::MouseButton b, bool clicked) {
    if (clicked) {
        if (updateUnsub) {
            updateUnsub.unsubscribe();
        } else {
            updateUnsub = Unsubscriber();
            for (auto updateSub : mUpdateSubs) {
                updateSub = ServiceSystem::Get<UpdateService>()->Get<UpdateObservable>()->subscribe(
                    std::bind(&MultiUnsubTest::onUpdate, this, std::placeholders::_1));
                updateSub->setUnsubscriber(updateUnsub);
            }
        }
    }
}

void MultiUnsubTest::onRender(SDL_Renderer *renderer) {
    TestBase::onRender(renderer);
    ctr = 0;
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