#include "UUID.h"

namespace Entities {
const UUID& NullId() {
    static UUID ID = {0};
    return ID;
}

UUID generateUUID() {
    std::random_device rd;
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<uint64_t> dist;
    UUID id{dist(mt), dist(mt)};
    while (id == NullId()) {
        id = UUID{dist(mt), dist(mt)};
    }
    return;
}

bool CompareUUID::operator()(const UUID& lhs, const UUID& rhs) {
    return lhs < rhs;
}
}  // namespace Entities

bool operator==(const Entities::UUID& lhs, const Entities::UUID& rhs) {
    return lhs[0] == rhs[0] && lhs[1] == rhs[1];
}
bool operator!=(const Entities::UUID& lhs, const Entities::UUID& rhs) {
    return !(lhs == rhs);
}
bool operator<(const Entities::UUID& lhs, const Entities::UUID& rhs) {
    return lhs[0] < rhs[0] || (lhs[0] == rhs[0] && lhs[1] < rhs[1]);
}

std::ostream& operator<<(std::ostream& os, const Entities::UUID& rhs) {
    if (rhs[0] != 0) {
        os << rhs[0];
    }
    os << rhs[1];
    return os;
}

std::string std::to_string(const Entities::UUID& id) {
    return to_string(id[0]) + to_string(id[1]);
}
