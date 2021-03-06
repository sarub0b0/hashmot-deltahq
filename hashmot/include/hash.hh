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
#ifndef HASH_H
#define HASH_H

#include <vector>
#include <array>
#include <unordered_map>

#include "using.hh"

namespace std {

inline bool operator==(const array<uint32_t, K_FUNCS> &lhs,
                       const array<uint32_t, K_FUNCS> &rhs) {

    for (int i = 0; i < K_FUNCS; i++) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }

    return true;
}
template <>
struct hash<array<uint32_t, K_FUNCS>> {
    size_t operator()(const array<uint32_t, K_FUNCS> &key) const {
        size_t seed = 0;

        for (auto &&k : key) {
            auto h = hash<int>()(k);
            seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        return seed;
    }
};

} // namespace std
#endif
