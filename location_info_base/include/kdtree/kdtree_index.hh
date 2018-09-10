#ifndef KDTREE_INDEX_H
#define KDTREE_INDEX_H

#include "tree.hh"

namespace neighbor_search {

struct Trunk {
    Trunk *prev;
    string str;
    Trunk(Trunk *prev, string str) {
        this->prev = prev;
        this->str  = str;
    }
};

struct Node {
    Point point;
    Node *left;
    Node *right;
    int axis;
    // TODO なんかのリストか宛先を持つと早くなる？
    // 隣のノード？
};

class KdTreeIndex {
   public:
    KdTreeIndex();
    ~KdTreeIndex();

    void Index(vector<Point> &points);
    void Update(Point &point);
    void Query(array<float, 2> &q);

   private:
    Node *tree_;

    vector<Point> points_;

    // Node *MakeTree(vector<array<float, 2>> &points);
    // Node *MakeTree(vector<Point> &points, int begin, int end, int axis);
    Node *MakeTree(vector<Point> &points, int axis);
    vector<int> RangeSearch(array<float, 2> &query, int radius);

    void showTrunks(Trunk *p);
    void printTree(Node *root, Trunk *prev, bool isLeft);
};
} // namespace neighbor_search

#endif
