#include <list>
#include <iostream>
#include <cfloat>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <cassert>

#include <using.hh>
#include <kdtree/kdtree_index.hh>

namespace neighbor_search {

// Helper function to print branches of the binary tree
void KdTreeIndex::showTrunks(Trunk *p) {
    if (p == nullptr) return;
    showTrunks(p->prev);
    cout << p->str;
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
    cout << root->node.id << endl;
    if (prev) prev->str = prev_str;

    trunk->str = "    |";
    printTree(root->right, trunk, false);
}

KdTreeIndex::KdTreeIndex() {
}
KdTreeIndex::~KdTreeIndex() {
}

void KdTreeIndex::Index(vector<Node> &nodes) {

    for (auto &&n : nodes) {
        TreeNode *tn = new TreeNode();
        tn->node     = n;
        tnodes_.push_back(tn);
    }

    nodes_.resize(nodes.size());
    copy(nodes.begin(), nodes.end(), nodes_.begin());

    chrono::high_resolution_clock::time_point begin =
        chrono::high_resolution_clock::now();

    tree_ = MakeTree(nullptr, nodes_, 0, nodes.size(), 0);

    // tree_ = new TreeNode();
    // for (int i = 0; i < nodes.size(); i++) {
    //     AddIndex(tree_, nodes[i]);
    // }

    chrono::high_resolution_clock::time_point end =
        chrono::high_resolution_clock::now();

    chrono::nanoseconds make_elapsed =
        chrono::duration_cast<chrono::nanoseconds>(end - begin);

    // printf("make elapsed=%lld\n", make_elapsed.count());

    // printTree(tree_, nullptr, false);

    // nodes_ = Sort(nodes_);

    // id_sorted_nodes_.resize(nodes_.size());
    // copy(nodes_.begin(), nodes_.end(), id_sorted_nodes_.begin());

    // printf("---------------------- Init -------------------\n\n");
    // printTree(tree_, nullptr, false);
}

bool KdTreeIndex::IsRemakeTree(TreeNode *tnode, const Node &node) {
    int axis = tnode->axis;
    if (!tnode->left && !tnode->right) {
        return false;
    }
    // int idx = 0;

    // for (auto &&l : tnode->list) {
    //     if (l == tnode->node.id) {
    //         break;
    //     }

    //     idx++;
    // }

    if (!tnode->left) {
        if (node.pos[axis] < tnode->right->node.pos[axis]) {
            return false;
        } else {
            return true;
        }
    }

    if (!tnode->right) {
        if ((tnode->left->node.pos[axis] < node.pos[axis])) {
            return false;
        } else {
            return true;
        }
    }
    if ((tnode->left->node.pos[axis] < node.pos[axis]) &&
        (node.pos[axis] < tnode->right->node.pos[axis])) {
        return false;
    }

    return true;
}

void KdTreeIndex::Update(Node &node) {

    // printf("search: %d\n", node.id);

    // TreeNode *tnode = nullptr;

    // Remove
    // Rebuild
    if (!IsRemakeTree(tnodes_[node.id], node)) {

        tnodes_[node.id]->node = node;
        // printTree(tree_, nullptr, false);
        return;
    }
    // puts("==============================================================");
    // printf("Update: %d\n", node.id);
    // printTree(tree_, nullptr, false);
    ReMakeTree(tnodes_[node.id], node);
    // printTree(tree_, nullptr, false);

    // tree_ = tnode;

    // if (tnode != nullptr) {
    // printf("find: %d\n", tnode->node.id);

    // printf("---------------------- Remake -------------------\n\n");

    // } else {
    //     printf("Can't find id=%d\n", node.id);
    // }

    // Add
    // id_sorted_nodes_[node.id].pos = node.pos;
    // printf("Add: %d\n", node.id);
    AddIndex(tree_, node);
    // printTree(tree_, nullptr, false);
}

void KdTreeIndex::AddIndex(TreeNode *tnode, Node &node) {
    if (tnode == nullptr) {
        return;
    }

    int axis       = tnode->axis;
    bool is_left   = false;
    TreeNode *next = nullptr;

    if (node.pos[axis] < tnode->node.pos[axis]) {
        next    = tnode->left;
        is_left = true;
    } else {
        next = tnode->right;
    }

    tnode->list.push_back(node.id);

    // if (tnode->left) assert(tnode->left != tnode);
    // if (tnode->right) assert(tnode->right != tnode);
    // printf("node=%d\n", tnode->node.id);

    // printf("parent=%p current=%p left=%p right=%p\n",
    //        tnode->parent,
    //        tnode,
    //        tnode->left,
    //        tnode->right);
    if (next == nullptr) {
        // next = new TreeNode();
        axis = axis == 0 ? 1 : 0;

        next         = tnodes_[node.id];
        next->node   = node;
        next->axis   = axis;
        next->parent = tnode;

        // next->left  = nullptr;
        // next->right = nullptr;

        next->list.clear();
        next->list.push_back(node.id);

        TreeNode *parent = tnode;

        // printf("parent=%d ", parent->node.id);
        // if (parent->left) {
        //     printf("left=%d ", parent->left->node.id);
        //     if (parent->left->left)
        //         printf("lleft=%d ", parent->left->left->node.id);
        //     if (parent->left->right)
        //         printf("rleft=%d ", parent->left->right->node.id);
        // }
        // if (parent->right) {
        //     printf("right=%d ", parent->right->node.id);
        //     if (parent->right->left)
        //         printf("lright=%d ", parent->right->left->node.id);
        //     if (parent->right->right)
        //         printf("rright=%d ", parent->right->right->node.id);
        // }
        // puts("");

        if (is_left) {
            tnode->left = next;
        } else {
            tnode->right = next;
        }

        // if (tnode->left) assert(tnode->left != tnode);
        // if (tnode->right) assert(tnode->right != tnode);

        return;
    } else {
        AddIndex(next, node);
    }
    return;
}

TreeNode *KdTreeIndex::Search(TreeNode *tnode, Node &node) {
    return nullptr;
}

TreeNode *KdTreeIndex::ReMakeTree(TreeNode *tnode, Node &node) {

    // TreeNode *parent = tnode->parent;

    // printTree(tree_, nullptr, false);

    // if (tnode->left) tnode->left->parent = nullptr;
    // if (tnode->right) tnode->right->parent = nullptr;

    tnode->left  = nullptr;
    tnode->right = nullptr;

    TreeNode *tn = tnode;
    // printf("\nupdate=%d\n", node.id);

    // printf("before list size=%lu\n", tnode->list.size());
    while (tn) {
        // int idx = 0;
        // printf("tnode list before\n");
        // for (list<int>::iterator itr = tn->list.begin();
        //      itr != tn->list.end();
        //      itr++) {
        //     printf("%d ", *itr);
        // }
        // puts("");

        for (list<int>::iterator itr = tn->list.begin();
             itr != tn->list.end();
             itr++) {

            if (*itr == node.id) {
                itr = tn->list.erase(itr);
                itr = tn->list.end();
                // break;
            }
            // idx++;
        }
        // tn->list.remove(node.id);

        // printf("tnode list after\n");
        // for (list<int>::iterator itr = tn->list.begin();
        //      itr != tn->list.end();
        //      itr++) {
        //     printf("%d ", *itr);
        // }
        // puts("");
        tn = tn->parent;
    }
    vector<Node> list(tnode->list.size());

    // printf("after list size=%lu\n", tnode->list.size());
    // printf("update=%d\n", node.id);
    int idx = 0;
    // printf("len(%lu): ", list.size());
    for (auto &&i : tnode->list) {
        list[idx] = tnodes_[i]->node;

        // printf("%d ", list[idx].id);

        idx++;
    }
    // printf("len(%lu): %d ", list.size(), list[idx].id);

    // puts("");

    TreeNode *parent = tnode->parent;
    TreeNode *ret    = MakeTree(parent, list, 0, list.size(), tnode->axis);
    if (!parent) {
        tree_ = ret;
        return nullptr;
    }

    // TreeNode *ret;
    if (parent->left == tnode) {
        // parent->left =
        // parent->left = MakeTree(parent, list, 0, list.size(), tnode->axis);
        // printf("parent=%d left=%d left->parent=%d\n",
        //        parent->node.id,
        //        ret->node.id,
        //        ret->parent->node.id);
        parent->left = ret;
    } else {
        // parent->right =
        // parent->right = MakeTree(parent, list, 0, list.size(),
        // tnode->axis); printf("parent=%d right=%d left->parent=%d\n",
        //        parent->node.id,
        //        ret->node.id,
        //        ret->parent->node.id);
        parent->right = ret;
    }

    // if (tnode->left) assert(tnode->left != tnode);
    // if (tnode->right) assert(tnode->right != tnode);

    // printf("parent=%d ", parent->node.id);
    // if (parent->left) {
    //     printf("left=%d ", parent->left->node.id);
    //     if (parent->left->left)
    //         printf("lleft=%d ", parent->left->left->node.id);
    //     if (parent->left->right)
    //         printf("rleft=%d ", parent->left->right->node.id);
    // }
    // if (parent->right) {
    //     printf("right=%d ", parent->right->node.id);
    //     if (parent->right->left)
    //         printf("lright=%d ", parent->right->left->node.id);
    //     if (parent->right->right)
    //         printf("rright=%d ", parent->right->right->node.id);
    // }
    // puts("");

    // printTree(parent, nullptr, false);

    return nullptr;
    //     if (tnode == nullptr) return nullptr;

    //     // TODO 更新修正
    //     // 親ノードのポインタを扱えるようにする
    //     // 更新ノードに直接アクセスできるようにする
    //     // ノードの位置関係が変わらなければ値のみ更新
    //     //
    //     int axis;

    //     axis = tnode->axis;

    //     TreeNode *next;

    //     bool is_left = false;
    //     if (node.pos[axis] < tnode->node.pos[axis]) {
    //         next    = tnode->left;
    //         is_left = true;
    //     } else {
    //         next = tnode->right;
    //     }

    //     for (list<int>::iterator itr = tnode->list.begin();
    //          itr != tnode->list.end();
    //          itr++) {

    //         if (*itr == node.id) {
    //             tnode->list.erase(itr);
    //             break;
    //         }
    //     }

    //     if (tnode->node.id == node.id ||
    //         (next != nullptr && next->node.id == node.id)) {
    //         // printf("remake: %d\n", tnode->node.id);
    //         // remove node from nodes_
    //         // printf("list_size=%lu range[%d, %d]\n",
    //         //        tnode->list.size(),
    //         //        tnode->range[0],
    //         //        tnode->range[1]);

    //         // printf("reamke list: ");

    //         int idx = 0;

    //         vector<Node> list(tnode->list.size());

    //         for (auto &&i : tnode->list) {
    //             // list[idx] = id_sorted_nodes_[i];
    //             list[idx] = tnodes_[i]->node;
    //             // printf("%d ", list[idx].id);
    //             idx++;
    //         }

    //         // puts("");

    //         ClearTree(tnode->left);
    //         ClearTree(tnode->right);

    //         tnode = MakeTree(list, 0, list.size(), tnode->axis);

    //         return tnode;
    //     } else {
    //         TreeNode *new_tree;
    //         new_tree = ReMakeTree(next, node);
    //         if (is_left) {
    //             tnode->left = new_tree;
    //         } else {
    //             tnode->right = new_tree;
    //         }
    //     }

    //     return tnode;
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

TreeNode *KdTreeIndex::MakeTree(
    TreeNode *parent, vector<Node> &nodes, int begin, int end, int axis) {

    int nodes_size = -1;

    nodes_size = end - begin;
    if (nodes_size == 0) {
        return nullptr;
    }

    int median = -1;

    nodes = Sort(nodes, begin, end, axis);

    median = nodes_size / 2;

    int median_idx = -1;

    median_idx = median + begin;

    // printf("median=%d\tid=%d\t(%.2f, %.2f) axis=%d [%d, %d]\n",
    //        median,
    //        nodes[median_idx].id,
    //        nodes[median_idx].pos[0],
    //        nodes[median_idx].pos[1],
    //        axis,
    //        begin,
    //        end);

    TreeNode *tnode;

    tnode           = tnodes_[nodes[median_idx].id];
    tnode->own_idx  = median_idx;
    tnode->axis     = axis;
    tnode->range[0] = begin;
    tnode->range[1] = end;

    tnode->list.clear();

    // printf("tnode list(%d): ", tnode->node.id);
    // int idx = 0;
    for (int i = begin; i < end; i++) {
        tnode->list.push_back(nodes[i].id);
        // printf("%d ", nodes[i].id);
        // idx++;
    }
    // puts("");

    tnode->parent = parent;

    axis         = axis == 0 ? 1 : 0;
    tnode->left  = MakeTree(tnode, nodes, begin, median_idx, axis);
    tnode->right = MakeTree(tnode, nodes, median_idx + 1, end, axis);

    // if (tnode->left) {
    //     printf("tnode.id=%d tnode.left.id=%d range(%d, %d)\n",
    //            tnode->node.id,
    //            tnode->left->node.id,
    //            begin,
    //            end);
    //     assert(tnode->left != tnode);
    // }
    // if (tnode->right) {
    //     printf("tnode.id=%d tnode.right.id=%d range(%d, %d)\n",
    //            tnode->node.id,
    //            tnode->right->node.id,
    //            begin,
    //            end);
    //     assert(tnode->right != tnode);
    // }

    return tnode;
}
TreeNode *KdTreeIndex::MakeTree(vector<Node> &nodes, int axis) {
    //     int nodes_size = -1;
    //     nodes_size     = nodes.size();
    //     if (nodes_size == 0) {
    //         return nullptr;
    //     }

    //     int median = -1;

    //     nodes = Sort(nodes, axis);

    //     median = nodes_size / 2;

    //     printf("median=%d\tid=%d\t(%.2f, %.2f) axis=%d\n",
    //            median,
    //            nodes[median].id,
    //            nodes[median].pos[0],
    //            nodes[median].pos[1],
    //            axis);

    //     int left_size  = -1;
    //     int right_size = -1;

    //     left_size  = median;
    //     right_size = nodes_size - left_size - 1;

    //     vector<Node> left(left_size);
    //     vector<Node> right(right_size);

    //     copy(nodes.begin(), nodes.begin() + left_size, left.begin());
    //     copy(nodes.begin() + left_size + 1, nodes.end(), right.begin());

    TreeNode *tnode = new TreeNode();
    //     tnode->node     = nodes[median];
    //     tnode->axis     = axis;

    //     axis         = axis == 0 ? 1 : 0;
    //     tnode->left  = MakeTree(left, axis);
    //     tnode->right = MakeTree(right, axis);

    return tnode;
}

vector<int> KdTreeIndex::Query(Node &query, int radius) {
    neighbor_.clear();
    RangeSearch(tree_, query, radius);

    return neighbor_;
}
void KdTreeIndex::RangeSearch(TreeNode *tnode, Node &query, int radius) {

    if (tnode == nullptr) return;

    array<float, 2> &node = tnode->node.pos;

    float dist = Distance(query.pos, node);

    if (dist < radius && query.id != tnode->node.id) {
        neighbor_.push_back(tnode->node.id);
    }

    // printf("search: %d %.2f\n", tnode->node.id, dist);

    int axis;
    axis = tnode->axis;

    TreeNode *next, *nnext;

    if (query.pos[axis] < node[axis]) {
        next  = tnode->left;
        nnext = tnode->right;
    } else {
        next  = tnode->right;
        nnext = tnode->left;
    }

    if (tnode->left) assert(tnode->left != tnode);
    if (tnode->right) assert(tnode->right != tnode);
    RangeSearch(next, query, radius);

    float diff = query.pos[axis] - node[axis];

    if (fabs(diff) < radius) RangeSearch(nnext, query, radius);
    return;
}

float KdTreeIndex::Distance(array<float, 2> &a, array<float, 2> &b) {
    return sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2));
}

int KdTreeIndex::Median(vector<Node> &nodes, int begin, int end) {
    return (end - begin) / 2;
}
vector<Node> &KdTreeIndex::Sort(vector<Node> &nodes,
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
vector<Node> &KdTreeIndex::Sort(vector<Node> &nodes, int axis) {

    sort(nodes.begin(), nodes.end(), [&](const Node &x, const Node &y) {
        return x.pos[axis] < y.pos[axis];
    });
    return nodes;
}

vector<Node> &KdTreeIndex::Sort(vector<Node> &nodes) {
    sort(nodes.begin(), nodes.end(), [](const Node &x, const Node &y) {
        return x.id < y.id;
    });
    return nodes;
}

void KdTreeIndex::Clear() {
    ClearTree(tree_);
}

void KdTreeIndex::Validation(vector<Node> &nodes) {
    // Sort(nodes);
    for (unsigned int i = 0; i < nodes.size(); i++) {
        Node a = tnodes_[i]->node;
        Node b = nodes[i];
        // printf("tnode=%d node=%d\n", a.id, b.id);
        assert(tnodes_[i]->node.id == nodes[i].id);
        // printf(
        //     "a(%f, %f) b(%f, %f)\n", a.pos[0], a.pos[1], b.pos[0],
        //     b.pos[1]);
        assert(FLT_EPSILON > fabs(a.pos[0] - b.pos[0]));
        assert(FLT_EPSILON > fabs(a.pos[1] - b.pos[1]));
        // printf("ar=%d br=%d\n", a.radius, b.radius);
        assert(a.radius == b.radius);
    }
}
} // namespace neighbor_search
