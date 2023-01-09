#ifndef UUID_H
#define UUID_H

#include <array>
#include <ostream>
#include <random>

typedef std::array<uint64_t, 2> UUID;

UUID generateUUID() {
    std::random_device rd;
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<uint64_t> dist;
    return UUID{dist(mt), dist(mt)};
}

bool operator==(const UUID& lhs, const UUID& rhs) {
    return lhs[0] == rhs[0] && lhs[1] == rhs[1];
}
bool operator!=(const UUID& lhs, const UUID& rhs) { return !(lhs == rhs); }
bool operator<(const UUID& lhs, const UUID& rhs) {
    return lhs[0] < rhs[0] || (lhs[0] == rhs[0] && lhs[1] < rhs[1]);
}

std::ostream& operator<<(std::ostream& os, const UUID& rhs) {
    if (rhs[0] != 0) {
        os << rhs[0];
    }
    os << rhs[1];
    return os;
}

struct CompareUUID {
    bool operator()(const UUID& lhs, const UUID& rhs) { return lhs < rhs; }
};

namespace std {
template <>
struct hash<UUID> {
    // Based on Boost's hash_combine
    size_t operator()(const UUID& id) const {
        std::hash<uint64_t> hasher;
        size_t hash = 0;
        hash ^= hasher(id[0]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= hasher(id[1]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};
}  // namespace std

#endif