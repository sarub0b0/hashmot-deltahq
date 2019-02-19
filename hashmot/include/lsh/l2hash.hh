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

#ifndef L2HASH_H
#define L2HASH_H

#include <vector>
#include <array>

#include "point.hh"
#include <using.hh>

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
