#ifndef UPDATE_SERVICE_H
#define UPDATE_SERVICE_H

#include "../../EventSystem/Time.h"
#include "../../Utils/Observable/Observable.h"
#include "../ServiceHandler.h"

typedef Observable<Time, void(Time)> UpdateObservable;

class UpdateService : public Service<UpdateObservable> {};

#endif