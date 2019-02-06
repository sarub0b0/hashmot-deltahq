#ifndef NEIGHBOR_SEARCH_H
#define NEIGHBOR_SEARCH_H

#include <cstdio>
#include <array>

#include <using.hh>

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
    virtual ~NeighborSearch(){
        // fprintf(stderr, "NeighborSearch Destructor\n");
    };

    virtual void Init(const Value &json) {
        fprintf(stderr, "NeighborSearch Init\n");
    };
    // virtual void Init(const vector<Node> &node) {
    //     fprintf(stderr, "NeighborSearch Init\n");
    // };
    // virtual void Add(const Value &json);
    virtual int Update(const Value &json) {
        fprintf(stderr, "NeighborSearch Update\n");
        return 0;
    };
    // virtual vector<int> GetNeighbor(const Value &json) {
    //     fprintf(stderr, "NeighborSearch GetNeighbor\n");
    //     return vector<int>();
    // };
    virtual void GetNeighbor(int id, vector<int32_t> &neighbor) {
        fprintf(stderr, "NeighborSearch GetNeighbor\n");
    };
    // virtual vector<int> GetNeighbor(const Node &node) {
    //     fprintf(stderr, "NeighborSearch GetNeighbor\n");
    //     return vector<int>();
    // };
    virtual void SendDeltaHQ(const vector<int32_t> &neighbor,
                             int id,
                             // const Value &json,
                             string &key) {
        fprintf(stderr, "NeighborSearch SendDeltaHQ\n");
    };
    // virtual void SendDeltaHQ(vector<int> &neighbor,
    //                          const Node &node,
    //                          string &key) {
    //     fprintf(stderr, "NeighborSearch SendDeltaHQ\n");
    // };
    virtual void SendDeltaHQ(void) {
        fprintf(stderr, "NeighborSearch SendDeltaHQ\n");
    };
    virtual void InitDGram(const string &host, const string &port) {
        fprintf(stderr, "NeighborSearch InitDGram\n");
    };
}; // namespace neighbor_search
}; // namespace neighbor_search
#endif
