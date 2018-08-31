#ifndef LSH_H
#define LSH_H

#include <vector>
#include <set>

#include <rapidjson/document.h>

#include "global.hh"
#include "lsh_index.hh"
#include "neighbor_search.hh"

namespace neighbor_search {

class LSH : public NeighborSearch {
   public:
    LSH(float w, int d, int k, int L);
    ~LSH();
    void Init(Value *object, Json *json);
    void Update(Value *object, Json *json);
    void GetNeighbor(Json *json);
    void SendDeltaHQ(vector<int> neighbor, Json *json, string key);

   private:
    float w_;
    int d_;
    int k_;
    int L_;
    set<int> radius_;

    vector<LSHIndex *> lsh_;
};

} // namespace neighbor_search
#endif
