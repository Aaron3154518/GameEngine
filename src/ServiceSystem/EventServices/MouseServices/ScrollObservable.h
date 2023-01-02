#ifndef SCROLL_SERVICE_H
#define SCROLL_SERVICE_H

#include <ServiceSystem/Component.h>
#include <ServiceSystem/CoreServices/RenderService.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/ServiceSystem.h>

#include <memory>

namespace EventServices {
typedef Observable<void(float), UIComponentPtr> ScrollObservableBase;

class ScrollObservable : public ScrollObservableBase {};

typedef std::shared_ptr<ScrollObservable> ScrollObservablePtr;
}  // namespace EventServices

#endif
