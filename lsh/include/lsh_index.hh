#ifndef LSH_INDEX_H
#define LSH_INDEX_H

#include <string>
#include <unordered_map>
#include <list>

#include <global.hh>
#include <l2hash_family.hh>

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

    string Hash(vector<L2Hash> &g, vector<float> &v);

    void Index(vector<vector<float>> &points);
    void Update(int id, vector<float> &point);

    vector<int> Query(vector<float> &q);

    bool IsSameRadius(int radius);

   private:
    L2HashFamily *hash_family_;
    float w_;
    int d_;
    int k_;
    int L_;
    int radius_;

    vector<vector<float>> points_;

    vector<Table> hash_table_;
};
} // namespace neighbor_search

#endif
