#include "ServiceSystem.h"

void ServiceBase::init() {}

std::unordered_map<std::type_index, std::shared_ptr<ServiceBase>>&
ServiceSystem::Services() {
    static std::unordered_map<std::type_index, std::shared_ptr<ServiceBase>>
        SERVICES;
    return SERVICES;
}