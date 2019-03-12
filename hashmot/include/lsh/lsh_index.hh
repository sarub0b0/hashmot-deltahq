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

#ifndef LSH_INDEX_H
#define LSH_INDEX_H

#include <array>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <vector>

#include <using.hh>
#include "l2hash_family.hh"
#include <hash.hh>

namespace neighbor_search {

struct Table {
    vector<L2Hash> g;
    unordered_map<array<uint32_t, K_FUNCS>, list<int>> table;
};

class LSHIndex {

   public:
    LSHIndex(float w, int d, int k, int L, int radius);
    ~LSHIndex();

    void Resize(int L);

    array<uint32_t, K_FUNCS> Hash(vector<L2Hash> &g,
                                  array<float, DIMENSION> &v);

    void Index(vector<array<float, DIMENSION>> &points);
    void Update(int id, array<float, DIMENSION> &point);
    void Query(array<float, DIMENSION> &q, struct send_data &send_data);

    bool IsSameRadius(int radius);

   private:
    L2HashFamily *hash_family_;
    float w_;
    int d_;
    int k_;
    int L_;
    int radius_;

    vector<array<float, DIMENSION>> points_;

    unordered_set<int> candidate_;

    vector<Table> hash_table_;
};
} // namespace neighbor_search

#endif
