#ifndef LINEAR_INDEX_H
#define LINEAR_INDEX_H

#include <array>
#include <list>

#include <using.hh>
#include <neighbor_search.hh>

namespace neighbor_search {

class LinearIndex {
   public:
    LinearIndex();
    ~LinearIndex();

    void Index(vector<Node> &nodes);
    void Update(Node &node);
    void Query(Node &query, vector<int> &neighbor);
    void Clear();

   private:
    vector<Node> nodes_;
    vector<int> neighbor_;

    // void Search(Node &query, int radius, vector<int> *neighbor);

    // float Distance(array<float, 2> &a, array<float, 2> &b);

    // static int Median(vector<Node> &nodes, int begin, int end);
    // static vector<Node> &Sort(vector<Node> &nodes,
    //                            int begin,
    //                            int end,
    //                            int axis);

    // static vector<Node> &Sort(vector<Node> &nodes, int axis);
    // static vector<Node> &Sort(vector<Node> &nodes);
};
} // namespace neighbor_search

#endif
