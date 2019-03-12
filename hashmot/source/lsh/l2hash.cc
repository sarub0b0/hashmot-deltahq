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


#include <array>
#include <cmath>

#include <lsh/l2hash.hh>

namespace neighbor_search {

L2Hash::L2Hash(array<float, 2> rand, float offset, float w)
    : rand_(point(rand[0], rand[1])), b_(offset), w_(w) {
}

L2Hash::~L2Hash() {
}

int L2Hash::Hash(point &vec) {

    return int((Dot(vec, rand_) + b_) / w_);
}
float L2Hash::Norm(point &a, point &b) {
    return sqrt(powf(a.x - b.x, 2) + powf(a.y - b.y, 2));
}

float L2Hash::Dot(point &a, point &b) {
    return (a.x * b.x) + (a.y * b.y);
}

} // namespace neighbor_search
