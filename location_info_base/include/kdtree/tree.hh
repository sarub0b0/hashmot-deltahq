#ifndef TREE_H
#define TREE_H

#include <array>
#include <vector>

#include <global.hh>

namespace neighbor_search {

struct Point {
    int id;
    array<float, 2> point;
};

class Tree {
   public:
    Tree();
    ~Tree();

    static int Median(vector<Point> &points, int begin, int end);
    // static vector<Point> &Sort(vector<Point> &points,
    //                            int begin,
    //                            int end,
    //                            int axis);

    static vector<Point> &Sort(vector<Point> &points, int axis);
}; // namespace neighbor_search

} // namespace neighbor_search
#endif
