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

#include <chrono>

#include <using.hh>
#include <neighbor_search.hh>
#include <lsh/lsh_index.hh>

namespace neighbor_search {

LSHIndex::LSHIndex(float w, int d, int k, int L, int radius)
    : w_(w), d_(d), k_(k), L_(0), radius_(radius) {
    hash_family_ = new L2HashFamily(w_, d_);
    Resize(L);
    L_ = L;
}
LSHIndex::~LSHIndex() {
}

void LSHIndex::Resize(int L) {
    if (L < L_) {
        hash_table_.resize(L);
    } else {
        vector<vector<L2Hash>> hash_func;
        hash_func.reserve(L - L_ + 1);
        int l = 0;
        for (int i = L_; i < L; ++i) {
            hash_func.push_back(vector<L2Hash>());
            hash_func[l].reserve(k_ + 1);
            ++l;
            for (int j = 0; j < k_; ++j) {
                hash_func[i].push_back(hash_family_->CreateHashFunc());
            }
        }

        for (auto &&g : hash_func) {
            Table t;
            t.g = g;
            t.g.reserve(k_ + 1);
            hash_table_.push_back(t);
        }
    }
}

array<uint32_t, K_FUNCS> LSHIndex::Hash(vector<L2Hash> &g,
                                        array<float, DIMENSION> &v) {
    array<uint32_t, K_FUNCS> hashes;
    for (int i = 0; i < K_FUNCS; ++i) {
        point p(v[0], v[1]);
        hashes[i] = g[i].Hash(p);
    }
    return hashes;
}

void LSHIndex::Index(vector<array<float, DIMENSION>> &points) {

    points_ = points;

    float min_x, min_y, max_x, max_y;
    min_x = INT_MAX;
    min_y = INT_MAX;
    max_x = 0;
    max_y = 0;
    float x, y;
    for (auto &&p : points_) {
        x = p[0];
        y = p[1];
        if (x < min_x) {
            min_x = x;
        }
        if (max_x < x) {
            max_x = x;
        }
        if (y < min_y) {
            min_y = y;
        }
        if (max_y < y) {
            max_y = y;
        }
    }

    int w, h;
    w = max_x - min_x;
    h = max_y - min_y;

    int table_size = w < h ? h / w_ : w / w_;
    table_size *= table_size;

    for (auto &&table : hash_table_) {
        table.table.reserve(table_size + 1);
        for (int i = 0; i < table_size; i++) {
            array<float, 2> p{static_cast<float>(i), static_cast<float>(i)};
            table.table[Hash(table.g, p)].push_back(i);
        }
        table.table.clear();
        for (unsigned int i = 0; i < points.size(); i++) {
            table.table[Hash(table.g, points[i])].push_back(i);
        }
    }

    int candidate_size = points_.size();
    candidate_.reserve(candidate_size + 1);

    for (int i = 0; i < candidate_size; i++) {
        candidate_.insert(i);
    }
    candidate_.clear();
}

void LSHIndex::Update(int id, array<float, DIMENSION> &point) {

#ifdef TCHK_ELAPSED
    chrono::high_resolution_clock::time_point begin, end;
    chrono::nanoseconds elapsed;

    begin = chrono::high_resolution_clock::now();

#endif

    array<float, DIMENSION> &old_point = points_[id];
    array<float, DIMENSION> &new_point = point;

    for (auto &&table : hash_table_) {
        const array<uint32_t, K_FUNCS> old_hash = Hash(table.g, old_point);
        const array<uint32_t, K_FUNCS> new_hash = Hash(table.g, new_point);

        if (old_hash == new_hash) {
            continue;
        }

        table.table[old_hash].remove(id);
        table.table[new_hash].push_back(id);
    }

    old_point = new_point;

#ifdef TCHK_ELAPSED

    end = chrono::high_resolution_clock::now();

    elapsed = chrono::duration_cast<chrono::nanoseconds>(end - begin);

    printf("update lsh: %lld.%09lld\n",
           elapsed.count() / 1000000000,
           elapsed.count() % 1000000000);
#endif
}

void LSHIndex::Query(array<float, DIMENSION> &q, struct send_data &send_data) {
    candidate_.clear();

    for (auto &&table : hash_table_) {
        list<int> &match = table.table[Hash(table.g, q)];
        candidate_.insert(match.begin(), match.end());
    }
    for (auto &&i : candidate_) {
        float d;

        array<float, 2> &a = q;
        array<float, 2> &b = points_[i];

        d = sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]));
        if (d <= radius_ && d != 0.0) {
            send_data.neighbor[send_data.neighbor_size++] = i;
        }
    }
}

bool LSHIndex::IsSameRadius(int radius) {
    if (radius_ == radius) {
        return true;
    } else {
        return false;
    }
}

} // namespace neighbor_search
