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
#ifndef NEIGHBOR_SEARCH_H
#define NEIGHBOR_SEARCH_H

#include <cstdio>
#include <array>

#include <using.hh>
#include <dgram.hh>

namespace neighbor_search {

struct send_data {
    int8_t type;
    int32_t id;
    float x;
    float y;
    int16_t neighbor_size;
    int32_t neighbor[1];
};

struct finish_data {
    int8_t type;
};

struct Node {
    int id;
    array<float, 2> pos;
    // float x, y;
    int radius;
    // Node() : id(-1), x(0.0), y(0.0), radius(0) {
    Node() : id(-1), radius(0) {
        this->pos[0] = 0.0;
        this->pos[1] = 0.0;
    }
    Node(int id, array<float, 2> pos, int radius) {
        this->id     = id;
        this->pos    = pos;
        this->radius = radius;
    }
    Node(int id, array<float, 2> pos) {
        this->id     = id;
        this->pos    = pos;
        this->radius = 0;
    }
};

// struct Node {
//     int id;
//     float x, y;
//     int radius;
// };

class NeighborSearch {
   public:
    // NeighborSearch();
    virtual ~NeighborSearch(){};

    virtual void Init(const Value &json) {
        fprintf(stderr, "NeighborSearch Init\n");
    };
    virtual int Update(const Value &json, struct send_data &send_data) {
        fprintf(stderr, "NeighborSearch Update\n");
        return 0;
    };
    virtual void GetNeighbor(int id, struct send_data &send_data) {
        fprintf(stderr, "NeighborSearch GetNeighbor\n");
    };
    virtual void SendDeltaHQ(const struct send_data &send_data){};
    virtual void SendDeltaHQ(void){};
    virtual void InitDGram(const string &host, const string &port){};

    // DGram dgram_;
    // bool is_socket_;
    // int max_neighbors_;
}; // namespace neighbor_search
}; // namespace neighbor_search
#endif
