#ifndef LSH_H
#define LSH_H

#include <vector>
#include <set>

#include <rapidjson/document.h>

#include "using.hh"
#include <dgram.hh>
#include <neighbor_search.hh>
#include <lsh/lsh_index.hh>

namespace neighbor_search {

class LSH : public NeighborSearch {
   public:
    LSH(int d, int k, int L);
    ~LSH();
    void Init(const Value &json);
    // void Init(const vector<Node> &nodes);
    int Update(const Value &json);
    // vector<int> GetNeighbor(const Value &json);
    vector<int> GetNeighbor(int id);
    // void SendDeltaHQ(vector<int> &neighbor, const Value &json, string
    // &key);
    void SendDeltaHQ(const vector<int> &neighbor, int id, string &key);
    void SendDeltaHQ(void);
    void InitDGram(const string &host, const string &port);

   private:
    int d_;
    int k_;
    int L_;
    set<int> radius_;

    vector<LSHIndex *> lsh_;

    vector<Node> nodes_;
    vector<int> neighbor_;

    DGram dgram_;
    bool is_socket_;
    char send_init_buffer_[9000];
    char send_update_buffer_[9000];
    int init_buffer_pos_;
    int update_buffer_pos_;
};

} // namespace neighbor_search
#endif
