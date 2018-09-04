#ifndef NEIGHBOR_SEARCH_H
#define NEIGHBOR_SEARCH_H

#include <cstdio>

#include <global.hh>

namespace neighbor_search {

class NeighborSearch {
   public:
    // NeighborSearch();
    virtual ~NeighborSearch() {
        fprintf(stderr, "NeighborSearch Destructor\n");
    };

    virtual void Init(const Value &json) {
        fprintf(stderr, "NeighborSearch Init\n");
    };
    virtual void Update(const Value &json) {
        fprintf(stderr, "NeighborSearch Update\n");
    };
    virtual void GetNeighbor(const Value &json) {
        fprintf(stderr, "NeighborSearch GetNeighbor\n");
    };
    virtual void SendDeltaHQ(vector<int> &neighbor,
                             const Value &json,
                             string &key) {
        fprintf(stderr, "NeighborSearch SendDeltaHQ\n");
    };
};
}; // namespace neighbor_search
#endif
