#ifndef HASH_H
#define HASH_H

#include <vector>
#include <array>
#include <unordered_map>

#include <using.hh>

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

} // namespace neighbor_search
namespace std {

inline bool operator==(const array<uint32_t, K_FUNCS> &lhs,
                       const array<uint32_t, K_FUNCS> &rhs) {

    return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];
}
template <>
struct hash<array<uint32_t, K_FUNCS>> {
    size_t operator()(const array<uint32_t, K_FUNCS> &key) const {
        size_t seed = 0;

        auto a = hash<int>()(key[0]);
        auto b = hash<int>()(key[1]);
        auto c = hash<int>()(key[2]);

        seed ^= a + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= b + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= c + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        return seed;
    }
};

} // namespace std
// namespace std {
// inline bool operator==(const vector<int> &lhs, const vector<int> &rhs) {

//     int lhs_size = lhs.size();
//     int rhs_size = rhs.size();

//     if (lhs_size != rhs_size) {
//         return false;
//     }

//     for (int i = 0; i < lhs_size; ++i) {
//         if (lhs[i] != rhs[i]) {
//             return false;
//         }
//     }

//     return true;
// }
// template <>
// struct hash<vector<int>> {
//     size_t operator()(const vector<int> &vec) const {
//         size_t seed = 0;

//         for (auto &&v : vec) {
//             seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//         }

//         return seed;
//     }
// };

// } // namespace std

#endif
