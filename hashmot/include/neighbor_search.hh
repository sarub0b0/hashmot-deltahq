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
