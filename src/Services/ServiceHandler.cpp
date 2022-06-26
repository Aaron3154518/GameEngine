#include "ServiceHandler.h"

std::unordered_map<std::type_index, std::shared_ptr<ServiceBase>>& ServiceHandler::Services() {
    static std::unordered_map<std::type_index, std::shared_ptr<ServiceBase>> SERVICES;
    return SERVICES;
}