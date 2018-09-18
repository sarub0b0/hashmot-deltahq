#ifndef KDTREE_INDEX_H
#define KDTREE_INDEX_H

#include <array>
#include <list>

#include <using.hh>

namespace neighbor_search {

struct Trunk {
    Trunk *prev;
    string str;
    Trunk(Trunk *prev, string str) {
        this->prev = prev;
        this->str  = str;
    }
};

struct Point {
    int id;
    array<float, 2> pos;
    Point() : id(-1) {
        pos = {0, 0};
    }
    Point(int id, array<float, 2> pos) {
        this->id  = id;
        this->pos = pos;
    }
};

struct Node {
    Point point;
    Node *left;
    Node *right;
    int axis;
    array<int, 2> range;
    list<int> list;
    Node() : axis(-1) {
        left  = nullptr;
        right = nullptr;
        range = {-1, -1};
    }
    ~Node() {
        list.clear();
        left  = nullptr;
        right = nullptr;
    }
    // TODO なんかのリストか宛先を持つと早くなる？
    // 隣のノード？
};

class KdTreeIndex {
   public:
    KdTreeIndex();
    ~KdTreeIndex();

    void Index(vector<Point> &points);
    void Update(Point &point);
    vector<int> Query(Point &query, int radius);
    void Clear();

   private:
    Node *tree_;

    vector<Point> points_;
    vector<Point> id_sorted_points_;
    vector<int> neighbor_;

    Node *MakeTree(vector<Point> &points, int begin, int end, int axis);

    Node *MakeTree(vector<Point> &points, int axis);

    void RangeSearch(Node *node, Point &query, int radius);

    Node *Search(Node *node, Point &point);

    void AddIndex(Node *node, Point &point);
    void RemoveIndex();
    Node *ReMakeTree(Node *node, Point &point);
    void ClearTree(Node *node);

    void showTrunks(Trunk *p);
    void printTree(Node *root, Trunk *prev, bool isLeft);

    float Distance(array<float, 2> &a, array<float, 2> &b);

    static int Median(vector<Point> &points, int begin, int end);
    static vector<Point> &Sort(vector<Point> &points,
                               int begin,
                               int end,
                               int axis);

    static vector<Point> &Sort(vector<Point> &points, int axis);
    static vector<Point> &Sort(vector<Point> &points);
};
} // namespace neighbor_search

#endif
