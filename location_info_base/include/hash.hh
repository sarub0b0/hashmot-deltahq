#ifndef HASH_H
#define HASH_H

#include <vector>
#include <array>
#include <unordered_map>

namespace neighbor_search {

// struct umap_key {
//     std::vector<int> key;
//     umap_key() {
//         key.reserve(3);
//     }
// };

// inline bool operator==(const umap_key &lhs, const umap_key &rhs) {

//     int lhs_size = lhs.key.size();
//     int rhs_size = rhs.key.size();

//     if (lhs_size != rhs_size) {
//         return false;
//     }

//     for (int i = 0; i < lhs_size; ++i) {
//         if (lhs.key[i] != rhs.key[i]) {
//             return false;
//         }
//     }

//     return true;
// }

// } // namespace neighbor_search
// namespace std {
// template <>
// struct hash<neighbor_search::umap_key> {
//     size_t operator()(const neighbor_search::umap_key &key) const {
//         size_t seed = 0;

//         for (auto &&k : key.key) {
//             seed ^= k + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//         }

//         return seed;
//     }
// };

// struct umap_key {
//     std::array<int, 3> key;
// };

// inline bool operator==(const umap_key &lhs, const umap_key &rhs) {

//     return lhs.key[0] == rhs.key[0] && lhs.key[1] == rhs.key[1] &&
//            lhs.key[2] == rhs.key[2];
// }

// } // namespace neighbor_search
// namespace std {
// template <>
// struct hash<neighbor_search::umap_key> {
//     size_t operator()(const neighbor_search::umap_key &key) const {
//         size_t seed = 0;

//         seed ^= key.key[0] + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//         seed ^= key.key[1] + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//         seed ^= key.key[2] + 0x9e3779b9 + (seed << 6) + (seed >> 2);

//         return seed;
//     }
// };

} // namespace neighbor_search
namespace std {
inline bool operator==(const vector<int> &lhs, const vector<int> &rhs) {

    int lhs_size = lhs.size();
    int rhs_size = rhs.size();

    if (lhs_size != rhs_size) {
        return false;
    }

    for (int i = 0; i < lhs_size; ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }

    return true;
}
template <>
struct hash<vector<int>> {
    size_t operator()(const vector<int> &vec) const {
        size_t seed = 0;

        for (auto &&v : vec) {
            seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        return seed;
    }
};

} // namespace std

#endif
