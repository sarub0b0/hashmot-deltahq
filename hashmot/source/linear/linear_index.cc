// MIT License

// Copyright (c) 2019 kosay

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


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
void LinearIndex::Query(Node &query, struct send_data &send_data) {
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
            // neighbor.push_back(node.id);
            send_data.neighbor[send_data.neighbor_size++] = node.id;
        }
    }
    // return neighbor;
}
void LinearIndex::Clear() {
}

} // namespace neighbor_search
