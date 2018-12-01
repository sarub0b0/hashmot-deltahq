#ifndef KDTREE_H
#define KDTREE_H

#include <array>

#include <rapidjson/document.h>

#include <using.hh>
#include <neighbor_search.hh>
#include <dgram.hh>
#include "kdtree_index.hh"

namespace neighbor_search {

class KdTree : public NeighborSearch {
   public:
    KdTree();
    ~KdTree();

    void Init(const Value &json);
    // void Init(const vector<Node> &nodes);
    // void Add(const Value &json);
    int Update(const Value &json);
    // vector<int> GetNeighbor(const Value &json);
    vector<int> GetNeighbor(int id);
    void SendDeltaHQ(vector<int> &neighbor, int id, string &key);
    // void SendDeltaHQ(vector<int> &neighbor, const Value &json, string
    // &key);
    void SendDeltaHQ(void);

    void InitDGram(const string &host, const string &port);

   private:
    KdTreeIndex kdtree_;
    vector<Node> nodes_;
    DGram dgram_;
    bool is_socket_;

    // Node *update_node_;

    string send_init_, send_update_;
    char send_init_buffer_[9000];
    char send_update_buffer_[9000];
    int init_buffer_pos_;
    int update_buffer_pos_;
};

} // namespace neighbor_search
#endif
