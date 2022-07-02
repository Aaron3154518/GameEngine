#ifndef UPDATE_SERVICE_H
#define UPDATE_SERVICE_H

#include "../../Utils/Time.h"
#include "../Observable.h"
#include "../ServiceSystem.h"

typedef Observable<Time, void(Time)> UpdateObservable;

class UpdateService : public Service<UpdateObservable> {};

#endif