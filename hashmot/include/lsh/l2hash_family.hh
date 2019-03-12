// MIT License

// Copyright (c) 2019 kosay

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef L2HASH_FAMILY_H
#define L2HASH_FAMILY_H

#include <array>
#include <random>

#include "using.hh"
#include "l2hash.hh"

namespace neighbor_search {

class L2HashFamily {
   public:
    L2HashFamily();
    L2HashFamily(float w, int d);
    L2HashFamily(const L2HashFamily &l2hash_family);
    ~L2HashFamily();
    L2Hash CreateHashFunc();
    array<float, 2> RandVec();
    float RandOffset();
    string Combine(vector<int32_t> &hashes);

    L2HashFamily &operator=(const L2HashFamily &) {
        return *this;
    }

   private:
    float w_;
    int d_;

    std::random_device seed_gen_;
    std::mt19937 engine_;
    std::normal_distribution<> gauss_;
    std::uniform_real_distribution<> uniform_;
};
} // namespace neighbor_search
#endif
