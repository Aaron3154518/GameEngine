#include "ServiceHandler.h"

std::unordered_map<std::type_index, std::shared_ptr<Service>>& ServiceHandler::Services() {
    static std::unordered_map<std::type_index, std::shared_ptr<Service>> SERVICES;
    return SERVICES;
}