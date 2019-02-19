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
