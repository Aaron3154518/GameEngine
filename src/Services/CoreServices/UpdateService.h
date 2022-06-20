#ifndef UPDATE_SERVICE_H
#define UPDATE_SERVICE_H

#include "../../EventSystem/Time.h"
#include "../../Utils/Observable/Observable.h"

class UpdateService {
   public:
    UpdateService() = default;
    ~UpdateService() = default;

    Observable<Time, void(Time)> update$;
};

#endif