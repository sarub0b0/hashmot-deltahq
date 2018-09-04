#include <cmath>

#include <l2hash.hh>

namespace neighbor_search {

L2Hash::L2Hash(vector<float> rand, float offset, float w)
    : rand_(point(rand[0], rand[1])), b_(offset), w_(w) {
}

L2Hash::~L2Hash() {
}

int L2Hash::Hash(point &vec) {
    // printf("hash=%d vec=(%.2f, %.2f) rand=(%.2f, %.2f)\n",
    //        int(Dot(vec, rand_) + b_ / w_),
    //        vec.x,
    //        vec.y,
    //        rand_.x,
    //        rand_.y);

    return int((Dot(vec, rand_) + b_) / w_);
}
float L2Hash::Norm(point &a, point &b) {
    return sqrt(powf(a.x - b.x, 2) + powf(a.y - b.y, 2));
}

float L2Hash::Dot(point &a, point &b) {
    return (a.x * b.x) + (a.y * b.y);
}

} // namespace neighbor_search
