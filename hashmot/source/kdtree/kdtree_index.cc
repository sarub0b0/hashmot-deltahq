// MIT License

// Copyright (c) 2019 kosay

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <list>
#include <iostream>
#include <cfloat>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <cassert>
#include <x86intrin.h>

#include <using.hh>
#include <kdtree/kdtree_index.hh>

namespace neighbor_search {

// Helper function to print branches of the binary tree
void KdTreeIndex::showTrunks(Trunk *p) {
    if (p == nullptr) return;
    showTrunks(p->prev);
    printf("%s", p->str.c_str());
}

// Recursive function to print binary tree. It uses inorder traversal
// call as printTree(root, nullptr, false);
void KdTreeIndex::printTree(TreeNode *root, Trunk *prev, bool isLeft) {
    if (root == nullptr) return;

    string prev_str = "     ";
    Trunk *trunk    = new Trunk(prev, prev_str);
    printTree(root->left, trunk, true);

    if (!prev)
        trunk->str = "---";
    else if (isLeft) {
        trunk->str = ".---";
        prev_str   = "    |";
    } else {
        trunk->str = "`---";
        prev->str  = prev_str;
    }

    showTrunks(trunk);
    if (root->node.id == -1) {
        printf("x\n");
    } else {
        printf("%d\n", root->node.id);
    }
    if (prev) prev->str = prev_str;

    trunk->str = "    |";
    printTree(root->right, trunk, false);
}

KdTreeIndex::KdTreeIndex() {
}
KdTreeIndex::~KdTreeIndex() {
    tree_ = nullptr;
    nodes_.clear();

    for (auto &&tn : tnodes_) {
        delete tn;
    }
}

void KdTreeIndex::Index(vector<Node> &nodes) {
    max_depth_        = log2(nodes.size()) + 15;
    max_remove_count_ = nodes.size() * 1.0;
    remove_pos_       = 0;

    int nodes_size = nodes.size();
    tnodes_.reserve(nodes_size + 1);
    sorted_tnodes_.reserve(nodes_size + 1);

    for (auto &&n : nodes) {
        TreeNode *tn = new TreeNode();
        tn->node     = n;
        tnodes_.push_back(tn);
        sorted_tnodes_.push_back(tn);
    }

    rmtnodes_.resize(max_remove_count_);

    nodes_.resize(nodes.size());
    copy(nodes.begin(), nodes.end(), nodes_.begin());

#ifndef MEASURE
    chrono::high_resolution_clock::time_point begin =
        chrono::high_resolution_clock::now();
#endif

    tree_ = MakeTree(nullptr, nodes_, 0, nodes.size(), 0, 1);

#ifndef MEASURE
    chrono::high_resolution_clock::time_point end =
        chrono::high_resolution_clock::now();

    chrono::nanoseconds make_elapsed =
        chrono::duration_cast<chrono::nanoseconds>(end - begin);

    fprintf(stderr,
            "make elapsed=%lld.%09lld\n",
            make_elapsed.count() / 1000000000,
            make_elapsed.count() % 1000000000);
#endif
}

void KdTreeIndex::Update(Node &node) {

    TreeNode *tnode = tnodes_[node.id];

    TreeNode *remove_tn;

    remove_tn = SwapRemoveNode(tnode);

    AddIndex(tree_, node);

    if (max_remove_count_ == remove_pos_ || should_remake_) {

#ifdef TCHK_ELAPSED
        chrono::high_resolution_clock::time_point remake_begin =
            chrono::high_resolution_clock::now();
        fprintf(stderr, "remake\n");
#endif
        int i = 0;
        tree_ = nullptr;
        for (auto &&tn : tnodes_) {
            nodes_[i] = tn->node;
            AddIndex(tree_, nodes_[i]);
            ++i;
        }

        tree_ = MakeTree(nullptr, nodes_, 0, nodes_.size(), 0, 1);

        should_remake_ = false;

#ifdef TCHK_ELAPSED
        chrono::high_resolution_clock::time_point remake_end =
            chrono::high_resolution_clock::now();

        chrono::nanoseconds remake_elapsed =
            chrono::duration_cast<chrono::nanoseconds>(remake_end -
                                                       remake_begin);
#endif
        remove_pos_ = 0;

#ifdef TCHK_ELAPSED
        printf("remake elapsed=%lld.%09lld\n",
               remake_elapsed.count() / 1000000000,
               remake_elapsed.count() % 1000000000);
#endif
    }
}

TreeNode *KdTreeIndex::SwapRemoveNode(TreeNode *tnode) {
    TreeNode *parent, *left, *right;

    parent = tnode->parent;
    left   = tnode->left;
    right  = tnode->right;

    TreeNode *remove_tn = &rmtnodes_[remove_pos_];
    ++remove_pos_;

    remove_tn->node    = tnode->node;
    remove_tn->node.id = -1;
    remove_tn->parent  = parent;
    remove_tn->left    = left;
    remove_tn->right   = right;
    remove_tn->axis    = tnode->axis;

    remove_tn->depth = tnode->depth;

    if (parent == nullptr) {
        tree_ = remove_tn;
    } else {
        if (parent->left == tnode) {
            parent->left = remove_tn;
        } else {
            parent->right = remove_tn;
        }
    }

    if (left != nullptr) left->parent = remove_tn;
    if (right != nullptr) right->parent = remove_tn;

    tnode->parent       = nullptr;
    tnode->left         = nullptr;
    tnode->right        = nullptr;
    tnode->depth        = 0;
    tnode->remove_count = 0;

    return remove_tn;
}

void KdTreeIndex::AddIndex(TreeNode *tnode, Node &add_node) {
    if (tnode == nullptr) {
        return;
    }

    int axis, depth;
    bool is_left   = false;
    TreeNode *next = nullptr;
    TreeNode *curr = tnode;

    while (true) {

        axis    = curr->axis;
        depth   = curr->depth;
        is_left = false;
        next    = nullptr;

        next = curr->right;
        if (add_node.pos[axis] < curr->node.pos[axis]) {
            next    = curr->left;
            is_left = true;
        }

        if (!next) {

            next         = tnodes_[add_node.id];
            next->node   = add_node;
            next->axis   = axis == 0 ? 1 : 0;
            next->depth  = depth + 1;
            next->parent = curr;

            if (max_depth_ < next->depth) {
                should_remake_ = true;
            }

            if (is_left) {
                curr->left = next;
            } else {
                curr->right = next;
            }

            return;
        }
        curr = next;
    }
}

void KdTreeIndex::ClearTree(TreeNode *tnode) {
    if (tnode == nullptr) {
        return;
    }

    if (tnode->left != nullptr) {
        ClearTree(tnode->left);
    }
    if (tnode->right != nullptr) {
        ClearTree(tnode->right);
    }
    delete tnode;
    tnode = nullptr;
    return;
}

TreeNode *KdTreeIndex::MakeTree(TreeNode *parent,
                                vector<Node> &nodes,
                                int begin,
                                int end,
                                int axis,
                                int depth) {

    int nodes_size = end - begin;

    if (nodes_size == 0) {
        return nullptr;
    }

    vector<Node> &sorted_nodes = Sort(nodes, begin, end, axis);

    int median = nodes_size / 2;

    int median_idx = -1;

    median_idx = median + begin;

    TreeNode *tnode;

    tnode               = tnodes_[sorted_nodes[median_idx].id];
    tnode->axis         = axis;
    tnode->remove_count = 0;
    tnode->depth        = depth;

    tnode->parent = parent;

    axis = axis == 0 ? 1 : 0;
    tnode->left =
        MakeTree(tnode, sorted_nodes, begin, median_idx, axis, depth + 1);
    tnode->right =
        MakeTree(tnode, sorted_nodes, median_idx + 1, end, axis, depth + 1);

    return tnode;
}

void KdTreeIndex::Query(Node &query, struct send_data &send_data) {
    RangeSearch(tree_, query, query.radius, send_data);
}

void KdTreeIndex::RangeSearch(TreeNode *tnode,
                              Node &query,
                              int radius,
                              struct send_data &send_data) {

    if (tnode == nullptr) return;

    const array<float, 2> &node = tnode->node.pos;
    const array<float, 2> &q    = query.pos;

    float dist = 0;
    if (tnode->node.id != -1 && query.id != tnode->node.id) {
        dist = sqrt(pow(node[0] - q[0], 2) + pow(node[1] - q[1], 2));

        if (dist <= radius) {
            send_data.neighbor[send_data.neighbor_size++] = tnode->node.id;
        }
    }

    if (!tnode->left && !tnode->right) {
        return;
    }

    int axis;
    axis = tnode->axis;

    TreeNode *next, *nnext;

    next  = tnode->left;
    nnext = tnode->right;
    if (node[axis] < q[axis]) {
        next  = tnode->right;
        nnext = tnode->left;
    }

    RangeSearch(next, query, radius, send_data);

    float diff = fabs(q[axis] - node[axis]);

    if (diff <= radius) RangeSearch(nnext, query, radius, send_data);
    return;
}

inline vector<Node> &KdTreeIndex::Sort(vector<Node> &nodes,
                                       int begin,
                                       int end,
                                       int axis) {

    sort(nodes.begin() + begin,
         nodes.begin() + end,
         [&](const Node &x, const Node &y) {
             return x.pos[axis] < y.pos[axis];
         });
    return nodes;
}

void KdTreeIndex::Clear() {
    ClearTree(tree_);
}

void KdTreeIndex::Validation(vector<Node> &nodes) {
    for (unsigned int i = 0; i < nodes.size(); ++i) {
        Node a = tnodes_[i]->node;
        Node b = nodes[i];
        assert(tnodes_[i]->node.id == nodes[i].id);
        assert(FLT_EPSILON > fabs(a.pos[0] - b.pos[0]));
        assert(FLT_EPSILON > fabs(a.pos[1] - b.pos[1]));
        assert(a.radius == b.radius);
    }
}

void KdTreeIndex::PrintBack(int id) {

    TreeNode *tn = tnodes_[id];
    while (tn->parent) {
        fprintf(stderr,
                "%d (%.2f, %.2f)\n",
                tn->node.id,
                tn->node.pos[0],
                tn->node.pos[1]);
        fprintf(stderr, "parent%d\n", tn->parent->node.id);
        tn = tn->parent;
        id = tn->node.id;
    }
}
} // namespace neighbor_search
