#ifndef L2HASH_FAMILY_H
#define L2HASH_FAMILY_H

// #include <vector>
// #include <string>
#include <random>

#include "global.hh"
#include "l2hash.hh"

namespace neighbor_search {

class L2HashFamily {
   public:
    L2HashFamily();
    L2HashFamily(float w, int d);
    L2HashFamily(const L2HashFamily &l2hash_family);
    ~L2HashFamily();
    L2Hash CreateHashFunc();
    vector<float> RandVec();
    float RandOffset();
    string Combine(vector<int> &hashes);

    L2HashFamily &operator=(const L2HashFamily &) {
        return *this;
    }

   private:
    float w_;
    int d_;

    std::random_device seed_gen_;
    // std::default_random_engine engine_;
    std::mt19937 engine_;
    std::normal_distribution<> gauss_;
    std::uniform_real_distribution<> uniform_;
};
} // namespace neighbor_search
#endif
