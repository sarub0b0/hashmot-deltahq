// MIT License

// Copyright (c) 2019 kosay

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
    // int Update(const Value &json);
    int Update(const Value &json, struct send_data &send_data);
    // vector<int> GetNeighbor(const Value &json);
    // vector<int> GetNeighbor(int id, vector<int> *neighbor);
    // void GetNeighbor(int id, vector<int32_t> &neighbor);
    void GetNeighbor(int id, struct send_data &send_data);
    // void SendDeltaHQ(vector<int> &neighbor, const Value &json, string
    // &key);
    // void SendDeltaHQ(const vector<int32_t> &neighbor, int id, string &key);
    // void SendDeltaHQ(void);
    // void InitDGram(const string &host, const string &port);
    void SendDeltaHQ(const struct send_data &send_data);
    void SendDeltaHQ(void);
    void InitDGram(const string &host, const string &port);

   private:
    int d_;
    int k_;
    int L_;
    set<int> radius_;

    vector<LSHIndex *> lsh_;

    vector<Node> nodes_;
    // vector<int> neighbor_;

    DGram dgram_;
    bool is_socket_;
    int max_neighbors_;
    // char send_init_buffer_[9000];
    // char send_update_buffer_[9000];
    // int init_buffer_pos_;
    // int update_buffer_pos_;
};

} // namespace neighbor_search
#endif
