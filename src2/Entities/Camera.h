#ifndef CAMERA_H
#define CAMERA_H

#include <Entities/Entity.h>
#include <Framework/EventSystem/EventSystem.h>
#include <Framework/FrameworkComponents.h>
#include <Framework/RenderSystem/RenderSystem.h>
#include <Utils/Time.h>

#include <memory>

class Camera : public Entities::Entity {
   public:
    class Tracker {
       public:
        virtual ~Tracker() = default;

        virtual Rect operator()(Rect pos, const Rect& target, Time dt);
    };

    typedef std::unique_ptr<Tracker> TrackerPtr;

    class ConstantTracker : public Tracker {
       public:
        ConstantTracker(float spd);

        Rect operator()(Rect pos, const Rect& target, Time dt);

       private:
        float mMaxSpeed;
    };

    class ScaleTracker : public Tracker {
       public:
        ScaleTracker(float a);

        Rect operator()(Rect pos, const Rect& target, Time dt);

       private:
        float mA;
    };

    static Camera& Get();

    const Rect& getRect();
    void setRect(const Rect& r);
    Rect getRelativeRect(Rect r);

    void track(const Entities::UUID& eId = Entities::NullId(),
               TrackerPtr tracker = nullptr);
    const Entities::UUID& getTracking() const;

   private:
    void init();

    Entities::UUID mTrackId = Entities::NullId();
    TrackerPtr mTracker;
};

#endif
