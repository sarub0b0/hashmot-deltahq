#include <vector>
#include <array>

#include <rapidjson/document.h>

#include <global.hh>
#include <kdtree/kdtree.hh>

namespace neighbor_search {

KdTree::KdTree() {
}

KdTree::~KdTree() {
}

void KdTree::Init(const Value &json) {
    vector<Point> points;

    int id = 0;
    for (auto &&n : json["node"].GetArray()) {

        Point p;
        p.id    = id;
        p.point = array<float, 2>{static_cast<float>(n["x"].GetDouble()),
                                  static_cast<float>(n["y"].GetDouble())};
        points.push_back(p);
        id++;
    }

    kdtree_.Index(points);
}

void KdTree::Update(const Value &json) {
}

vector<int> KdTree::GetNeighbor(const Value &json) {
    vector<int> neighbor;
    return neighbor;
}

void KdTree::SendDeltaHQ(vector<int> &neighbor,
                         const Value &json,
                         string &key) {
}

} // namespace neighbor_search
