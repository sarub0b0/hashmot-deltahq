#ifndef LINEAR_H
#define LINEAR_H

#include <array>

#include <rapidjson/document.h>

#include <using.hh>
#include <neighbor_search.hh>
#include "linear_index.hh"

namespace neighbor_search {

class Linear : public NeighborSearch {
   public:
    Linear();
    ~Linear();

    void Init(const Value &json);
    void Init(const vector<Node> &nodes);
    // void Add(const Value &json);
    void Update(const Value &json);
    vector<int> GetNeighbor(const Value &json);
    vector<int> GetNeighbor(const Node &nodes);
    void SendDeltaHQ(vector<int> &neighbor, const Value &json, string &key);
    void SendDeltaHQ(vector<int> &neighbor, const Node &node, string &key);
    void InitDGram(const string &host, const string &port);

   private:
    vector<Node> nodes_;
    LinearIndex linear_;
};

} // namespace neighbor_search
#endif
