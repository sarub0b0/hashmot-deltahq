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
    array<int, 2> range;
    list<int> list;
    int own_idx;
    TreeNode() : axis(-1) {
        this->left    = nullptr;
        this->right   = nullptr;
        this->range   = {-1, -1};
        this->own_idx = -1;
    }
    ~TreeNode() {
        this->list.clear();
        this->left  = nullptr;
        this->right = nullptr;
    }
    // TODO なんかのリストか宛先を持つと早くなる？
    // 隣のノード？
};

class KdTreeIndex {
   public:
    KdTreeIndex();
    ~KdTreeIndex();

    void Index(vector<Node> &nodes);
    void Update(Node &node);
    vector<int> Query(Node &query, int radius);
    void Clear();

    void Validation(vector<Node> &nodes);

    void PrintBack(int id);

   private:
    TreeNode *tree_;

    vector<TreeNode *> tnodes_;
    vector<Node> nodes_;
    // vector<Node> id_sorted_nodes_;
    vector<int> neighbor_;

    TreeNode *MakeTree(
        TreeNode *parent, vector<Node> &nodes, int begin, int end, int axis);
    TreeNode *MakeTree(vector<Node> &nodes, int begin, int end, int axis);

    TreeNode *MakeTree(vector<Node> &nodes, int axis);

    void RangeSearch(TreeNode *tnode, Node &query, int radius);

    TreeNode *Search(TreeNode *tnode, Node &node);

    bool IsRemakeTree(TreeNode *tnode, const Node &node);

    void AddIndex(TreeNode *tnode, Node &node);

    void RemoveIndex();

    TreeNode *ReMakeTree(TreeNode *tnode, Node &node);
    void ClearTree(TreeNode *tnode);

    void showTrunks(Trunk *p);
    void printTree(TreeNode *root, Trunk *prev, bool isLeft);

    float Distance(const array<float, 2> &a, const array<float, 2> &b);

    static int Median(vector<Node> &nodes, int begin, int end);
    static vector<Node> &Sort(vector<Node> &nodes,
                              int begin,
                              int end,
                              int axis);

    static vector<Node> &Sort(vector<Node> &nodes, int axis);
    static vector<Node> &Sort(vector<Node> &nodes);
};
} // namespace neighbor_search

#endif
