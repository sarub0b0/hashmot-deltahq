#ifndef KDTREE_H
#define KDTREE_H

#include <array>

#include <rapidjson/document.h>

#include <using.hh>
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
    vector<Point> points_;
};

} // namespace neighbor_search
#endif
