#ifndef UPDATE_SERVICE_H
#define UPDATE_SERVICE_H

#include <ServiceSystem/Observable.h>
#include <ServiceSystem/Service.h>
#include <Utils/Time.h>

typedef ForwardObservable<void(Time)> UpdateObservable;

class UpdateService : public Service<UpdateObservable> {};

#endif