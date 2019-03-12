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

#ifndef KDTREE_INDEX_H
#define KDTREE_INDEX_H

#include <array>
#include <list>

#include <using.hh>
#include <neighbor_search.hh>

namespace neighbor_search {

struct Trunk {
    Trunk *prev;
    string str;
    Trunk(Trunk *prev, string str) {
        this->prev = prev;
        this->str  = str;
    }
};

struct TreeNode {
    Node node;
    TreeNode *parent;
    TreeNode *left;
    TreeNode *right;
    int axis;

    int depth;
    int remove_count;
    TreeNode() {
        this->parent       = nullptr;
        this->left         = nullptr;
        this->right        = nullptr;
        this->axis         = -1;
        this->depth        = 0;
        this->remove_count = 0;
    }
    ~TreeNode() {
        this->parent = nullptr;
        this->left   = nullptr;
        this->right  = nullptr;
    }
};

class KdTreeIndex {
   public:
    KdTreeIndex();
    ~KdTreeIndex();

    void Index(vector<Node> &nodes);
    void Update(Node &node);
    void Query(Node &query, struct send_data &send_data);
    void Clear();

    void Validation(vector<Node> &nodes);

    void PrintBack(int id);

   private:
    int max_depth_;
    int max_remove_count_;
    int remove_count_;
    bool should_remake_;

    TreeNode *tree_;

    vector<TreeNode *> remove_tnodes_;
    vector<TreeNode> rmtnodes_;
    int remove_pos_;

    vector<TreeNode *> tnodes_;
    vector<TreeNode *> sorted_tnodes_;
    vector<Node> nodes_;

    TreeNode *MakeTree(TreeNode *parent,
                       vector<Node> &nodes,
                       int begin,
                       int end,
                       int axis,
                       int depth);

    void RangeSearch(TreeNode *tnode,
                     Node &query,
                     int radius,
                     struct send_data &send_data);

    void AddIndex(TreeNode *tnode, Node &add_node);

    TreeNode *SwapRemoveNode(TreeNode *tnode);

    TreeNode *ReMakeTree(TreeNode *tnode, Node &node);
    void ClearTree(TreeNode *tnode);

    void showTrunks(Trunk *p);
    void printTree(TreeNode *root, Trunk *prev, bool isLeft);

    static vector<Node> &Sort(vector<Node> &nodes,
                              int begin,
                              int end,
                              int axis);
};
} // namespace neighbor_search

#endif
