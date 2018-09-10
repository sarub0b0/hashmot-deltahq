#ifndef L2HASH_H
#define L2HASH_H

#include <vector>
#include <array>

#include "point.hh"
#include <global.hh>

namespace neighbor_search {

class L2Hash {
   public:
    L2Hash(array<float, 2> rand, float offset, float w);
    // L2Hash(const L2Hash &l2hash);
    ~L2Hash();

    int Hash(point &vec);

    // L2Hash &operator=(const L2Hash &) {
    //     return *this;
    // }

    static float Norm(point &a, point &b);

   private:
    point rand_;
    float b_;
    float w_;

    static float Dot(point &a, point &b);
};

} // namespace neighbor_search
#endif
