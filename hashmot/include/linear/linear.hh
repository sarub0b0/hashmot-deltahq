#ifndef LINEAR_H
#define LINEAR_H

#include <array>

#include <rapidjson/document.h>

#include <using.hh>
#include <neighbor_search.hh>
#include <dgram.hh>
#include "linear_index.hh"

namespace neighbor_search {

class Linear : public NeighborSearch {
   public:
    Linear();
    ~Linear();

    void Init(const Value &json);
    // void Init(const vector<Node> &nodes);
    // void Add(const Value &json);
    int Update(const Value &json, struct send_data &send_data);
    // vector<int> GetNeighbor(const Value &json);
    void GetNeighbor(int id, struct send_data &send_data);
    // vector<int> GetNeighbor(int id, vector<int> *neighbor);
    // void SendDeltaHQ(const struct send_data &send_data);
    // void SendDeltaHQ(vector<int> &neighbor, const Value &json, string
    // &key);
    // void SendDeltaHQ(void);

    // void InitDGram(const string &host, const string &port);

    void SendDeltaHQ(const struct send_data &send_data);
    void SendDeltaHQ(void);
    void InitDGram(const string &host, const string &port);

   private:
    vector<Node> nodes_;
    LinearIndex linear_;

    // vector<int> neighbor_;

    DGram dgram_;
    bool is_socket_;
    int max_neighbors_;

    // string send_init_, send_update_;
    // char send_init_buffer_[9000];
    // char send_update_buffer_[9000];
    // int init_buffer_pos_;
    // int update_buffer_pos_;
};

} // namespace neighbor_search
#endif
