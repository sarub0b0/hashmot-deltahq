#ifndef KDTREE_H
#define KDTREE_H

#include <array>

#include <rapidjson/document.h>

#include <using.hh>
#include <neighbor_search.hh>
#include <dgram.hh>
#include "kdtree_index.hh"
namespace neighbor_search {

typedef GenericDocument<UTF8<>, MemoryPoolAllocator<>, MemoryPoolAllocator<>>
    DocumentType;

class KdTree : public NeighborSearch {
   public:
    KdTree();
    ~KdTree();

    void Init(const Value &json);
    // void Init(const vector<Node> &nodes);
    // void Add(const Value &json);
    int Update(const Value &json);
    // vector<int> GetNeighbor(const Value &json);
    void GetNeighbor(int id, vector<int32_t> &neighbor);
    // vector<int> GetNeighbor(int id, vector<int> *neighbor);
    void SendDeltaHQ(const vector<int32_t> &neighbor, int id, string &key);
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

    // char valueBuffer_[20000];
    // char parseBuffer_[9000];

    // MemoryPoolAllocator<> valueAllocator_;
    // MemoryPoolAllocator<> parseAllocator_;
    // DocumentType json_;

    // string send_init_, send_update_;
    char send_init_buffer_[9000];
    char send_update_buffer_[9000];
    int init_buffer_pos_;
    int update_buffer_pos_;

    int max_neighbors;
    struct send_data *send_data;
};

} // namespace neighbor_search
#endif
