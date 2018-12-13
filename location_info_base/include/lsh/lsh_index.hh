#ifndef LSH_INDEX_H
#define LSH_INDEX_H

// #include <bits/stdc++.h>

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
    // unordered_map<vector<int>, list<int>> table;
};

class LSHIndex {

   public:
    LSHIndex(float w, int d, int k, int L, int radius);
    ~LSHIndex();

    void Resize(int L);

    // vector<int> Hash(vector<L2Hash> &g, array<float, 2> &v);
    array<uint32_t, K_FUNCS> Hash(vector<L2Hash> &g,
                                  array<float, DIMENSION> &v);

    void Index(vector<array<float, DIMENSION>> &points);
    void Update(int id, array<float, DIMENSION> &point);
    void Query(array<float, DIMENSION> &q, vector<int> *neighbor);

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
