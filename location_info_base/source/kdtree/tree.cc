#include <algorithm>

#include <kdtree/tree.hh>

namespace neighbor_search {

Tree::Tree() {
}
Tree::~Tree() {
}

int Tree::Median(vector<Point> &points, int begin, int end) {
    return (end - begin) / 2;
}

vector<Point> &Tree::Sort(vector<Point> &points,
                          // int begin,
                          // int end,
                          int axis) {

    sort(points.begin(), points.end(), [&](const Point &x, const Point &y) {
        return x.point[axis] < y.point[axis];
    });
    // sort(points.begin() + begin,
    //      points.begin() + end,
    //      [&](const Point &x, const Point &y) {
    //          return x.point[axis] < y.point[axis];
    //      });
    return points;
}
} // namespace neighbor_search
