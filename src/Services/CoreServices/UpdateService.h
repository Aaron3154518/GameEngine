#ifndef UPDATE_SERVICE_H
#define UPDATE_SERVICE_H

#include "../../EventSystem/Time.h"
#include "../../Utils/Observable/Observable.h"
#include "../ServiceHandler.h"

class UpdateService : public Service {
   public:
    UpdateService() = default;
    ~UpdateService() = default;

    Observable<Time, void(Time)> update$;
};

REGISTER_SERVICE(UpdateService);

#endif