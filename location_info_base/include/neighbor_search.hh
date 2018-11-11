#ifndef NEIGHBOR_SEARCH_H
#define NEIGHBOR_SEARCH_H

#include <cstdio>
#include <array>

#include <using.hh>

namespace neighbor_search {

struct NodeParam {
    int id;
    float x, y;
    int radius;
};

class NeighborSearch {
   public:
    // NeighborSearch();
    virtual ~NeighborSearch(){
        // fprintf(stderr, "NeighborSearch Destructor\n");
    };

    virtual void Init(const Value &json) {
        fprintf(stderr, "NeighborSearch Init\n");
    };
    virtual void Init(const vector<NodeParam> &node) {
        fprintf(stderr, "NeighborSearch Init\n");
    };
    // virtual void Add(const Value &json);
    virtual void Update(const Value &json) {
        fprintf(stderr, "NeighborSearch Update\n");
    };
    virtual vector<int> GetNeighbor(const Value &json) {
        fprintf(stderr, "NeighborSearch GetNeighbor\n");
        return vector<int>();
    };
    virtual vector<int> GetNeighbor(const NodeParam &ndoe) {
        fprintf(stderr, "NeighborSearch GetNeighbor\n");
        return vector<int>();
    };
    virtual void SendDeltaHQ(vector<int> &neighbor,
                             const Value &json,
                             string &key) {
        fprintf(stderr, "NeighborSearch SendDeltaHQ\n");
    };
    virtual void SendDeltaHQ(vector<int> &neighbor,
                             const NodeParam &node,
                             string &key) {
        fprintf(stderr, "NeighborSearch SendDeltaHQ\n");
    };
}; // namespace neighbor_search
}; // namespace neighbor_search
#endif
