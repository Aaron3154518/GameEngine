#ifndef SCROLL_SERVICE_H
#define SCROLL_SERVICE_H

#include <ServiceSystem/Component.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/ServiceSystem.h>

typedef Observable<void(float), UIComponentPtr> ScrollObservableBase;

class ScrollObservable : public ScrollObservableBase {};

class ScrollService : public Service<ScrollObservable> {};

#endif
