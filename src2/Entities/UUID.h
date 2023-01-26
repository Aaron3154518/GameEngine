#ifndef UUID_H
#define UUID_H

#include <array>
#include <ostream>
#include <random>
#include <string>

namespace Entities {
typedef std::array<uint64_t, 2> UUID;

const UUID& NullId();

UUID generateUUID();

struct CompareUUID {
    bool operator()(const UUID& lhs, const UUID& rhs);
};
}  // namespace Entities

bool operator==(const Entities::UUID& lhs, const Entities::UUID& rhs);
bool operator!=(const Entities::UUID& lhs, const Entities::UUID& rhs);
bool operator<(const Entities::UUID& lhs, const Entities::UUID& rhs);

std::ostream& operator<<(std::ostream& os, const Entities::UUID& rhs);

namespace std {
template <>
struct hash<Entities::UUID> {
    // Based on Boost's hash_combine
    size_t operator()(const Entities::UUID& id) const {
        std::hash<uint64_t> hasher;
        size_t hash = 0;
        hash ^= hasher(id[0]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= hasher(id[1]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }

    namespace std {
    std::string to_string(const std::array<uint64_t, 2>& arr) {
        return std::to_string(arr[0]) + std::to_string(arr[1]);
    }
    }  // namespace std
};

std::string to_string(const Entities::UUID& id);
}  // namespace std

#endif