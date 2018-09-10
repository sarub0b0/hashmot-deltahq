#ifndef LSH_H
#define LSH_H

#include <vector>
#include <set>

#include <rapidjson/document.h>

#include "global.hh"
#include "lsh_index.hh"
#include <neighbor_search.hh>

namespace neighbor_search {

class LSH : public NeighborSearch {
   public:
    LSH(int d, int k, int L);
    ~LSH();
    void Init(const Value &json);
    void Update(const Value &json);
    vector<int> GetNeighbor(const Value &json);
    void SendDeltaHQ(vector<int> &neighbor, const Value &json, string &key);

   private:
    int d_;
    int k_;
    int L_;
    set<int> radius_;

    vector<LSHIndex *> lsh_;
};

} // namespace neighbor_search
#endif
