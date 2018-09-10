#ifndef KDTREE_H
#define KDTREE_H

#include <rapidjson/document.h>

#include <global.hh>
#include <neighbor_search.hh>
#include "kdtree_index.hh"

namespace neighbor_search {

class KdTree : public NeighborSearch {
   public:
    KdTree();
    ~KdTree();

    void Init(const Value &json);
    void Update(const Value &json);
    vector<int> GetNeighbor(const Value &json);
    void SendDeltaHQ(vector<int> &neighbor, const Value &json, string &key);

   private:
    KdTreeIndex kdtree_;

};

} // namespace neighbor_search
#endif
