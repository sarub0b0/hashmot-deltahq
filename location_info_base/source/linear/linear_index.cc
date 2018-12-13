#include <cmath>

#include <using.hh>
#include <linear/linear_index.hh>

namespace neighbor_search {

LinearIndex::LinearIndex() {
}
LinearIndex::~LinearIndex() {
}

void LinearIndex::Index(vector<Node> &nodes) {
    nodes_ = nodes;
}
void LinearIndex::Update(Node &node) {
    nodes_[node.id] = node;
}
// vector<int> LinearIndex::Query(Node &query, int radius) {
void LinearIndex::Query(Node &query, vector<int> *neighbor) {
    // vector<int> neighbor;
    // neighbor.reserve(nodes_.size());
    int radius = query.radius;

    for (auto &&node : nodes_) {
        if (node.id == query.id) {
            continue;
        }
        float d = sqrt(pow(node.pos[0] - query.pos[0], 2) +
                       pow(node.pos[1] - query.pos[1], 2));
        if (d < radius) {
            neighbor->push_back(node.id);
        }
    }
    // return neighbor;
}
void LinearIndex::Clear() {
}

} // namespace neighbor_search
