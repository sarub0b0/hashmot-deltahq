// MIT License

// Copyright (c) 2019 kosay

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <array>
#include <cmath>

#include <lsh/l2hash_family.hh>

namespace neighbor_search {

L2HashFamily::L2HashFamily() : w_(0), d_(0) {
}

L2HashFamily::L2HashFamily(float w, int d) : w_(w), d_(d) {
    engine_  = mt19937(seed_gen_());
    gauss_   = normal_distribution<>(0.0, 1.0);
    uniform_ = uniform_real_distribution<>(0.0, w_);
}

L2HashFamily::~L2HashFamily() {
}
L2Hash L2HashFamily::CreateHashFunc() {
    return L2Hash(RandVec(), RandOffset(), w_);
}
inline array<float, 2> L2HashFamily::RandVec() {
    array<float, 2> rand_vec;
    for (int i = 0; i < 2; i++) {
        rand_vec[i] = gauss_(engine_);
    }

    return rand_vec;
}

inline float L2HashFamily::RandOffset() {
    return uniform_(engine_);
}

} // namespace neighbor_search
