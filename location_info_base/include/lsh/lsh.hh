#ifndef LSH_H
#define LSH_H

#include <vector>
#include <set>

#include <rapidjson/document.h>

#include "using.hh"
#include "lsh_index.hh"
#include <neighbor_search.hh>

namespace neighbor_search {

class LSH : public NeighborSearch {
   public:
    LSH(int d, int k, int L);
    ~LSH();
    void Init(const Value &json);
    void Init(const vector<Node> &nodes);
    void Update(const Value &json);
    vector<int> GetNeighbor(const Value &json);
    vector<int> GetNeighbor(const Node &nodes);
    void SendDeltaHQ(vector<int> &neighbor, const Value &json, string &key);
    void SendDeltaHQ(vector<int> &neighbor, const Node &node, string &key);
    void InitDGram(const string &host, const string &port);

   private:
    int d_;
    int k_;
    int L_;
    set<int> radius_;

    vector<LSHIndex *> lsh_;

    vector<Node> nodes_;
};

} // namespace neighbor_search
#endif
