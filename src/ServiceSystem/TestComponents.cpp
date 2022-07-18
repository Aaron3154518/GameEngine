#include "TestComponents.h"

// TestBase
TestBase::TestBase(Rect r, int e) : mPos(std::make_shared<UIComponent>(r, e)) {}

SDL_Color TestBase::getColor() const { return BLACK; }

void TestBase::init() {
    mRenderSub =
        ServiceSystem::Get<RenderService, RenderObservable>()->subscribe(
            std::bind(&TestBase::onRender, this, std::placeholders::_1), mPos);
    mRenderSub->setUnsubscriber(unsub);
}

void TestBase::onRender(SDL_Renderer *renderer) {
    SDL_Color c = getColor();
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(renderer, mRenderSub->getData()->rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, mRenderSub->getData()->rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

// ClickRenderTest
ClickRenderTest::ClickRenderTest(Rect r, int e) : TestBase(r, e) {}

SDL_Color ClickRenderTest::getColor() const { return color ? GREEN : RED; }

void ClickRenderTest::init() {
    TestBase::init();
    mMouseSub = ServiceSystem::Get<MouseService, MouseObservable>()->subscribe(
        std::bind(&ClickRenderTest::onClick, this, std::placeholders::_1,
                  std::placeholders::_2),
        mPos);
    mMouseSub->setUnsubscriber(unsub);
}

void ClickRenderTest::onClick(Event::MouseButton b, bool clicked) {
    color = clicked;
}

// ChangeSubTest
ChangeSubTest::ChangeSubTest(Rect r, int e) : TestBase(r, e) {}

SDL_Color ChangeSubTest::getColor() const { return color ? ORANGE : PURPLE; }

void ChangeSubTest::init() {
    TestBase::init();
    mMouseSub = ServiceSystem::Get<MouseService, MouseObservable>()->subscribe(
        std::bind(&ChangeSubTest::onClick, this, std::placeholders::_1,
                  std::placeholders::_2),
        mPos);
    mMouseSub->setUnsubscriber(unsub);
}

void ChangeSubTest::onClick(Event::MouseButton b, bool clicked) {
    if (clicked) {
        color = !color;
        if (color) {
            ServiceSystem::Get<RenderService, RenderObservable>()
                ->updateSubscription(mRenderSub, [](SDL_Renderer *) {});
        } else {
            ServiceSystem::Get<RenderService, RenderObservable>()
                ->updateSubscription(mRenderSub,
                                     std::bind(&ChangeSubTest::onRender, this,
                                               std::placeholders::_1));
        }
    }
}

// UnsubTest
UnsubTest::UnsubTest(Rect r, int e) : TestBase(r, e) {}

SDL_Color UnsubTest::getColor() const { return color ? YELLOW : BLUE; }

void UnsubTest::init() {
    TestBase::init();
    mMouseSub = ServiceSystem::Get<MouseService, MouseObservable>()->subscribe(
        std::bind(&UnsubTest::onClick, this, std::placeholders::_1,
                  std::placeholders::_2),
        mPos);
    mMouseSub->setUnsubscriber(unsub);
}

void UnsubTest::onClick(Event::MouseButton b, bool clicked) {
    if (clicked) {
        color = !color;
        if (color) {
            mRenderSub->unsubscribe();
        } else {
            mRenderSub = ServiceSystem::Get<RenderService, RenderObservable>()
                             ->subscribe(std::bind(&UnsubTest::onRender, this,
                                                   std::placeholders::_1),
                                         mPos);
        }
    }
}

// UpdateTest
UpdateTest::UpdateTest(Rect r, int e) : TestBase(r, e) {}

SDL_Color UpdateTest::getColor() const {
    SDL_Color color = BLACK;
    if (delayMs > 0) {
        color.r = color.b = color.g =
            255 * (std::pow(delayMs / 1500., .15) - 1);
    }
    return color;
}

void UpdateTest::init() {
    TestBase::init();
    mUpdateSub =
        ServiceSystem::Get<UpdateService, UpdateObservable>()->subscribe(
            std::bind(&UpdateTest::onUpdate, this, std::placeholders::_1));
    mUpdateSub->setUnsubscriber(unsub);
    mMouseSub = ServiceSystem::Get<MouseService, MouseObservable>()->subscribe(
        std::bind(&UpdateTest::onClick, this, std::placeholders::_1,
                  std::placeholders::_2),
        mPos);
    mMouseSub->setUnsubscriber(unsub);
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
    TestBase::init();
    mUpdateSub =
        ServiceSystem::Get<UpdateService, UpdateObservable>()->subscribe(
            std::bind(&VisibilityTest::onUpdate, this, std::placeholders::_1));
    mUpdateSub->setUnsubscriber(unsub);
    mMouseSub = ServiceSystem::Get<MouseService, MouseObservable>()->subscribe(
        std::bind(&VisibilityTest::onClick, this, std::placeholders::_1,
                  std::placeholders::_2),
        mPos);
    mMouseSub->setUnsubscriber(unsub);
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

SDL_Color InheritanceTestBase::getColor() const { return color; }

void InheritanceTestBase::init() {
    TestBase::init();
    mMouseSub = ServiceSystem::Get<MouseService, MouseObservable>()->subscribe(
        std::bind(&InheritanceTestBase::onClick, this, std::placeholders::_1,
                  std::placeholders::_2, true),
        mPos);
    mMouseSub->setUnsubscriber(unsub);
}

void InheritanceTestBase::onClick(Event::MouseButton b, bool clicked,
                                  bool red) {
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
InheritanceTestDerived::InheritanceTestDerived(Rect r, int e)
    : InheritanceTestBase(r, e) {}

void InheritanceTestDerived::init() {
    InheritanceTestBase::init();
    mMouseSub->unsubscribe();
    mMouseSub = ServiceSystem::Get<MouseService, MouseObservable>()->subscribe(
        std::bind(&InheritanceTestDerived::onClick, this, std::placeholders::_1,
                  std::placeholders::_2, false),
        mPos);
}

// MultiUnsubTest
MultiUnsubTest::MultiUnsubTest(Rect r, int e) : TestBase(r, e) {
    mUpdateSubs = std::vector<UpdateObservable::SubscriptionPtr>(3);
}

SDL_Color MultiUnsubTest::getColor() const { return ctr == 0 ? MAGENTA : BLUE; }

void MultiUnsubTest::init() {
    TestBase::init();
    for (auto updateSub : mUpdateSubs) {
        updateSub =
            ServiceSystem::Get<UpdateService, UpdateObservable>()->subscribe(
                std::bind(&MultiUnsubTest::onUpdate, this,
                          std::placeholders::_1));
        updateSub->setUnsubscriber(updateUnsub);
    }
    mMouseSub = ServiceSystem::Get<MouseService, MouseObservable>()->subscribe(
        std::bind(&MultiUnsubTest::onClick, this, std::placeholders::_1,
                  std::placeholders::_2),
        mPos);
    mMouseSub->setUnsubscriber(unsub);
    ServiceSystem::Get<RenderService, RenderObservable>()->updateSubscription(
        mRenderSub,
        std::bind(&MultiUnsubTest::onRender, this, std::placeholders::_1));
}

void MultiUnsubTest::onUpdate(Time dt) { ctr++; }

void MultiUnsubTest::onClick(Event::MouseButton b, bool clicked) {
    if (clicked) {
        if (updateUnsub) {
            updateUnsub.unsubscribe();
        } else {
            updateUnsub = Unsubscriber();
            for (auto updateSub : mUpdateSubs) {
                updateSub =
                    ServiceSystem::Get<UpdateService, UpdateObservable>()
                        ->subscribe(std::bind(&MultiUnsubTest::onUpdate, this,
                                              std::placeholders::_1));
                updateSub->setUnsubscriber(updateUnsub);
            }
        }
    }
}

void MultiUnsubTest::onRender(SDL_Renderer *renderer) {
    TestBase::onRender(renderer);
    ctr = 0;
}

// MouseLockTest
MouseLockTest::MouseLockTest(Rect r, int e) : TestBase(r, e) {}

SDL_Color MouseLockTest::getColor() const {
    return mMouseLock ? PURPLE : GREEN;
}

void MouseLockTest::init() {
    TestBase::init();
    mUpdateSub =
        ServiceSystem::Get<UpdateService, UpdateObservable>()->subscribe(
            std::bind(&MouseLockTest::onUpdate, this, std::placeholders::_1));
    mUpdateSub->setUnsubscriber(unsub);
    mMouseSub = ServiceSystem::Get<MouseService, MouseObservable>()->subscribe(
        std::bind(&MouseLockTest::onClick, this, std::placeholders::_1,
                  std::placeholders::_2),
        mPos);
    mMouseSub->setUnsubscriber(unsub);
}

void MouseLockTest::onUpdate(Time dt) {
    if (mMouseLock && std::rand() % 150 == 0) {
        ServiceSystem::Get<MouseService, MouseObservable>()->releaseLock(
            mMouseLock);
    }
}

void MouseLockTest::onClick(Event::MouseButton b, bool clicked) {
    if (clicked) {
        mMouseLock =
            ServiceSystem::Get<MouseService, MouseObservable>()->requestLock();
    }
}

// DragTest
DragTest::DragTest(Rect r, int e, int d)
    : TestBase(r, e), mPos(std::make_shared<DragComponent>(r, e, d)) {
    mPos->onDragStart = std::bind(&DragTest::onDragStart, this);
    mPos->onDrag = std::bind(&DragTest::onDrag, this, std::placeholders::_1,
                             std::placeholders::_2, std::placeholders::_3,
                             std::placeholders::_4);
    mPos->onDragEnd = std::bind(&DragTest::onDragEnd, this);
}

SDL_Color DragTest::getColor() const {
    return mPos->dragging ? dragColor : YELLOW;
}

void DragTest::init() {
    TestBase::init();
    ServiceSystem::Get<RenderService, RenderObservable>()
        ->updateSubscriptionData(mRenderSub, mPos);
    mDragSub =
        ServiceSystem::Get<DragService, DragObservable>()->subscribe(mPos);
    mDragSub->setUnsubscriber(unsub);
}

void DragTest::onDragStart() {
    dragColor = SDL_Color{(Uint8)(rand() % 256), (Uint8)(rand() % 256),
                          (Uint8)(rand() % 256), 255};
}

void DragTest::onDrag(int x, int y, float dx, float dy) {
    if (mPos->dragDelay >= 0) {
        mPos->rect.setPos(x, y, Rect::Align::CENTER);
    } else {
        mPos->rect.move(dx, dy);
    }
}

void DragTest::onDragEnd() {}

// TimerTest
TimerTest::TimerTest(Rect r, int e) : TestBase(r, e) {}

SDL_Color TimerTest::getColor() const { return color ? PURPLE : ORANGE; }

void TimerTest::init() {
    TestBase::init();
    mTimerSub = ServiceSystem::Get<TimerService, TimerObservable>()->subscribe(
        std::bind(&TimerTest::onTimer, this), 200);
    mTimerSub->setUnsubscriber(unsub);
}

bool TimerTest::onTimer() {
    color = !color;
    mTimerSub->getData()->length = rand() % 500;
    return rand() % 25 != 0;
}

// Generate random test component
std::shared_ptr<TestBase> randomTestComponent(int w, int h) {
    Rect r;
    r.setPos(rand() % w - 10, rand() % h - 10);
    r.setDim(rand() % (w - r.X()), rand() % (w - r.Y()));
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