#include <cmath>

#include <l2hash_family.hh>

namespace neighbor_search {

L2HashFamily::L2HashFamily() : w_(0), d_(0) {
}

L2HashFamily::L2HashFamily(float w, int d) : w_(w), d_(d) {
    // engine_ = std::default_random_engine(seed_gen_());
    engine_ = mt19937(seed_gen_());
    gauss_  = normal_distribution<float>(0.0, 1.0);
}

L2HashFamily::~L2HashFamily() {
}
L2Hash L2HashFamily::CreateHashFunc() {
    return L2Hash(RandVec(), RandOffset(), w_);
}
vector<float> L2HashFamily::RandVec() {
    vector<float> rand_vec;
    for (int i = 0; i < d_; i++) {
        rand_vec.push_back(gauss_(engine_));
    }

    return rand_vec;
}

float L2HashFamily::RandOffset() {
    uniform_ = uniform_real_distribution<float>(0.0, w_);
    return uniform_(engine_);
}

string L2HashFamily::Combine(vector<int> hashes) {
    string key;
    key = "[";
    for (auto &&i : hashes) {
        key += to_string(i) + ",";
    }
    key.pop_back();
    key += "]";
    return key;
}

} // namespace neighbor_search
