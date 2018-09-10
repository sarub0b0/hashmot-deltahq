#ifndef LSH_INDEX_H
#define LSH_INDEX_H

#include <array>
#include <string>
#include <unordered_map>
#include <list>

#include <global.hh>
#include "l2hash_family.hh"

namespace neighbor_search {

typedef struct table {
    vector<L2Hash> g;
    unordered_map<string, list<int>> table;
} Table;

class LSHIndex {
   public:
    LSHIndex(float w, int d, int k, int L, int radius);
    ~LSHIndex();

    void Resize(int L);

    string Hash(vector<L2Hash> &g, array<float, 2> &v);

    void Index(vector<array<float, 2>> &points);
    void Update(int id, array<float, 2> &point);
    vector<int> Query(array<float, 2> &q);

    bool IsSameRadius(int radius);

   private:
    L2HashFamily *hash_family_;
    float w_;
    int d_;
    int k_;
    int L_;
    int radius_;

    vector<array<float, 2>> points_;

    vector<Table> hash_table_;
};
} // namespace neighbor_search

#endif
