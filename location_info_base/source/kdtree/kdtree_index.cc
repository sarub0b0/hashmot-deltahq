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
    // cout << p->str;
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
    // cout << root->node.id << endl;
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
    // nodes_.shrink_to_fit();

    for (auto &&tn : tnodes_) {
        delete tn;
    }
    // for (auto &&rm : remove_tnodes_) {
    //     delete rm;
    // }
}

void KdTreeIndex::Index(vector<Node> &nodes) {
    max_depth_ = log2(nodes.size()) + 10;
    // max_remove_count_ = pow(2, max_depth_);
    max_remove_count_ = nodes.size() * 0.8;
    remove_pos_       = 0;

    for (auto &&n : nodes) {
        TreeNode *tn = new TreeNode();
        tn->node     = n;
        tnodes_.push_back(tn);
        sorted_tnodes_.push_back(tn);
    }

    rmtnodes_.resize(max_remove_count_);

    nodes_.resize(nodes.size());
    copy(nodes.begin(), nodes.end(), nodes_.begin());

    chrono::high_resolution_clock::time_point begin =
        chrono::high_resolution_clock::now();

    tree_ = MakeTree(nullptr, nodes_, 0, nodes.size(), 0, 1);

    // tree_ = tnodes_[0];

    // int node_size = nodes_.size();
    // for (int i = 1; i < node_size; ++i) {
    //     nodes_[i] = tnodes_[i]->node;
    //     AddIndex(tree_, nodes_[i]);
    // }
    // tree_ = MakeTree(nullptr, sorted_tnodes_, 0, sorted_tnodes_.size(), 0,
    // 1);

    // tree_ = new TreeNode();
    // for (int i = 0; i < nodes.size(); i++) {
    //     AddIndex(tree_, nodes[i]);
    // }

    chrono::high_resolution_clock::time_point end =
        chrono::high_resolution_clock::now();

    chrono::nanoseconds make_elapsed =
        chrono::duration_cast<chrono::nanoseconds>(end - begin);

    printf("make elapsed=%lld.%09lld\n",
           make_elapsed.count() / 1000000000,
           make_elapsed.count() % 1000000000);

    float min_x, min_y, max_x, max_y;
    min_x = nodes_[0].pos[0];
    min_y = nodes_[0].pos[1];
    for (int i = 1; i < nodes_.size(); i++) {
        if (nodes_[i].pos[0] < min_x) {
            min_x = nodes_[i].pos[0];
        }
        if (max_x < nodes_[i].pos[0]) {
            max_x = nodes_[i].pos[0];
        }
        if (nodes_[i].pos[1] < min_y) {
            min_y = nodes_[i].pos[1];
        }
        if (max_y < nodes_[i].pos[1]) {
            max_y = nodes_[i].pos[1];
        }
    }

    fprintf(stderr,
            "-- Init density(%.2f) top-left(%.2f, %.2f), bottom-right(%.2f, %.2f)\n",
            nodes_.size() / ((max_x - min_x) * (max_y - min_y) * 0.000001),
            min_x,
            min_y,
            max_x,
            max_y);

    // printTree(tree_, nullptr, false);

    // nodes_ = Sort(nodes_);

    // id_sorted_nodes_.resize(nodes_.size());
    // copy(nodes_.begin(), nodes_.end(), id_sorted_nodes_.begin());

    // printf("---------------------- Init -------------------\n\n");
    // printTree(tree_, nullptr, false);
}

// TODO 正しい動作をしません．-> 正しい近傍結果を返せません
// bool KdTreeIndex::IsRemakeTree(TreeNode *tnode, const Node &node) {
//     int axis = tnode->axis;
//     if (!tnode->left && !tnode->right) {
//         return false;
//     }

//     if (!tnode->left) {
//         if (node.pos[axis] < tnode->right->node.pos[axis]) {
//             return false;
//         }
//         return true;
//     }

//     if (!tnode->right) {
//         if ((tnode->left->node.pos[axis] < node.pos[axis])) {
//             return false;
//         }
//         return true;
//     }
//     if ((tnode->left->node.pos[axis] < node.pos[axis]) &&
//         (node.pos[axis] < tnode->right->node.pos[axis])) {
//         return false;
//     }

//     return true;
// }

// ============================================================================
// 動かない．木構造のバランス変化のチェックは簡単にはできなさそう
// 木の状態が変化しないかのチェックをする
// return 1 : 葉ノード削除，追加
// return 2 : 自ノード以下を再構築，追加
// return 3 : 親ノードから再構築，追加
// ============================================================================
// int KdTreeIndex::IsRemakeTree(TreeNode *tnode, Node &node) {

// 葉ノードの時
// 親ノードから削除，ルートまで辿りながらlistから削除，新たに追加する
// if (tnode->left == nullptr && tnode->right == nullptr) {
//     return 1;
// }

// 左子ノードのみ

// 右子ノードのみ

// ノードを両方持つ
//
//
// TreeNode *parent = tnode->parent;
// bool is_left     = false;
// int axis         = tnode->axis;
// // 葉ノードの時
// if (!tnode->left && !tnode->right) {
//     return 1;
// }

// // 右子ノードだけ持つとき
// if (!tnode->left) {
//     if (tnode->right->node.pos[axis] < node.pos[axis]) {
//         return 2;
//     }
// }

// // 左子ノードだけ持つとき
// if (!tnode->right) {
//     if ((node.pos[axis] < tnode->left->node.pos[axis])) {
//         return 2;
//     }
// }

// // 両方の子ノードを持つ時
// if ((tnode->left && tnode->right) &&
//     ((node.pos[axis] < tnode->left->node.pos[axis]) ||
//      (tnode->right->node.pos[axis] < node.pos[axis]))) {
//     return 2;
// }

// if (parent) {

//     // 親ノードいる時
//     if (parent->left == tnode) {
//         is_left = true;
//     }

//     axis = parent->axis;
//     if (is_left) {
//         if (parent->node.pos[axis] < node.pos[axis]) {
//             return 3;
//         }
//     } else {
//         if (node.pos[axis] < parent->node.pos[axis]) {
//             return 3;
//         }
//     }
// }

//     return 0;
// }
// TreeNode *KdTreeIndex::CheckDestroyBalance(TreeNode *tnode, Node &node) {
//     // TreeNode *tn = tnode;

//     // int axis = tnode->axis;

//     // while (tn) {

//     //     tn = tn->parent;
//     // }

//     return nullptr;
// }

void KdTreeIndex::Update(Node &node) {

    // printf("search: %d\n", node.id);

    TreeNode *tnode = tnodes_[node.id];

    TreeNode *remove_tn;

    // printTree(tree_, nullptr, false);
    remove_tn = SwapRemoveNode(tnode);
    // printTree(tree_, nullptr, false);
    // ++remove_count_;

    // Remove
    // Rebuild
    // ret = CheckDestroyBalance(tnode, node);
    // int ret = IsRemakeTree(tnode, node);

    // if (ret == 0) {
    //     tnode->node = node;
    //     return;
    // }

    // ReMakeTree(tnode, node);

    AddIndex(tree_, node);

    // printf("tnode parent=%p left=%p right=%p\n",
    //        tnode->parent,
    //        tnode->left,
    //        tnode->right);

    // printTree(tree_, nullptr, false);

    if (max_remove_count_ == remove_pos_ || should_remake_) {
        // if (max_remove_count_ == remove_pos_) {

#ifdef TCHK_ELAPSED
        chrono::high_resolution_clock::time_point remake_begin =
            chrono::high_resolution_clock::now();
#endif
        int i = 0;
        tree_ = nullptr;
        for (auto &&tn : tnodes_) {
            nodes_[i] = tn->node;
            AddIndex(tree_, nodes_[i]);
            ++i;
        }

        // for (auto &&tn : tnodes_) {
        //     tn->parent       = nullptr;
        //     tn->left         = nullptr;
        //     tn->right        = nullptr;
        //     tn->axis         = 0;
        //     tn->depth        = 0;
        //     tn->remove_count = 0;
        // }
        // nodes_[0] = tnodes_[0]->node;
        // tree_     = tnodes_[0];

        // int node_size = nodes_.size();
        // for (int i = 1; i < node_size; ++i) {
        //     nodes_[i] = tnodes_[i]->node;
        //     AddIndex(tree_, nodes_[i]);
        // }
        tree_ = MakeTree(nullptr, nodes_, 0, nodes_.size(), 0, 1);

        // tree_ =
        //     MakeTree(nullptr, sorted_tnodes_, 0, sorted_tnodes_.size(), 0,
        //     1);
        should_remake_ = false;

#ifdef TCHK_ELAPSED
        chrono::high_resolution_clock::time_point remake_end =
            chrono::high_resolution_clock::now();

        chrono::nanoseconds remake_elapsed =
            chrono::duration_cast<chrono::nanoseconds>(remake_end -
                                                       remake_begin);
#endif
        remove_pos_ = 0;
        // TODO 遅くなるかもやからちょっと考える
        // for (auto &&rm : remove_tnodes_) {
        //     delete rm;
        // }
        // remove_tnodes_.clear();

#ifdef TCHK_ELAPSED
        printf("remake elapsed=%lld.%09lld\n",
               remake_elapsed.count() / 1000000000,
               remake_elapsed.count() % 1000000000);
#endif
    }

    // if (max_remove_count_ < remove_tn->remove_count) {
    //     ReMakeTree(tnode, node);
    // }

    // printTree(tree_, nullptr, false);
}

TreeNode *KdTreeIndex::SwapRemoveNode(TreeNode *tnode) {
    // TreeNode *tn = tnode->parent;
    // int node_id  = tnode->node.id;

    // while (tn) {
    //     if (tn->node.id == -1) {
    //         tn = tn->parent;
    //         if (tn == nullptr) {
    //             break;
    //         }
    //     }
    //     for (list<int>::iterator itr = tn->list.begin();
    //          itr != tn->list.end();
    //          itr++) {

    //         if (*itr == node_id) {
    //             itr = tn->list.erase(itr);
    //             itr = tn->list.end();
    //         }
    //     }
    //     ++tn->remove_count;
    //     tn = tn->parent;
    // }

    TreeNode *parent, *left, *right;

    parent = tnode->parent;
    left   = tnode->left;
    right  = tnode->right;

    TreeNode *remove_tn = &rmtnodes_[remove_pos_];
    ++remove_pos_;
    // TreeNode *remove_tn = new TreeNode();

    remove_tn->node    = tnode->node;
    remove_tn->node.id = -1;
    remove_tn->parent  = parent;
    remove_tn->left    = left;
    remove_tn->right   = right;
    remove_tn->axis    = tnode->axis;

    // copy(tnode->list.begin(), tnode->list.end(), remove_tn->list.begin());

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

    // remove_tnodes_.push_back(remove_tn);

    // printf("remove_tn id=%d (%.2f, %.2f)\n",
    //        remove_tn->node.id,
    //        remove_tn->node.pos[0],
    //        remove_tn->node.pos[1]);
    // printf("remove_tn parent=%p left=%p right=%p\n",
    //        remove_tn->parent,
    //        remove_tn->left,
    //        remove_tn->right);

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

        // curr->list.push_back(add_node.id);

        if (!next) {

            next         = tnodes_[add_node.id];
            next->node   = add_node;
            next->axis   = axis == 0 ? 1 : 0;
            next->depth  = depth + 1;
            next->parent = curr;

            // next->list.clear();
            // next->list.push_back(add_node.id);

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

// TreeNode *KdTreeIndex::Search(TreeNode *tnode, Node &node) {
//     return nullptr;
// }

// TreeNode *KdTreeIndex::ReMakeTree(TreeNode *tnode, Node &node) {

//     TreeNode *parent = nullptr;

//     if (tnode->left) tnode->left->parent = nullptr;
//     if (tnode->right) tnode->right->parent = nullptr;
//     tnode->left  = nullptr;
//     tnode->right = nullptr;

//     TreeNode *tn     = tnode;
//     int remove_count = tnode->remove_count;

//     while (tn) {
//         for (list<int>::iterator itr = tn->list.begin();
//              itr != tn->list.end();
//              itr++) {

//             if (*itr == node.id) {
//                 itr = tn->list.erase(itr);
//                 itr = tn->list.end();
//             }
//         }

//         tn->remove_count -= remove_count;
//         tn = tn->parent;
//     }

//     vector<Node> list(tnode->list.size());

//     int idx = 0;
//     for (auto &&i : tnode->list) {
//         list[idx] = tnodes_[i]->node;

//         idx++;
//     }

//     parent        = tnode->parent;
//     tnode->parent = nullptr;

//     TreeNode *ret =
//         MakeTree(parent, list, 0, list.size(), tnode->axis, tnode->depth);
//     if (parent == nullptr) {
//         tree_ = ret;
//         return nullptr;
//     }

//     if (parent->left == tnode) {
//         parent->left = ret;
//     } else {
//         parent->right = ret;
//     }
//     return nullptr;
// }

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

    // printf("median=%d\tid=%d\t(%.2f, %.2f) axis=%d [%d, %d]\n",
    //        median,
    //        nodes[median_idx].id,
    //        nodes[median_idx].pos[0],
    //        nodes[median_idx].pos[1],
    //        axis,
    //        begin,
    //        end);

    TreeNode *tnode;

    tnode = tnodes_[sorted_nodes[median_idx].id];
    // tnode->own_idx = median_idx;
    tnode->axis = axis;
    // tnode->range[0]     = begin;
    // tnode->range[1]     = end;
    tnode->remove_count = 0;
    tnode->depth        = depth;

    // tnode->list.clear();

    // printf("tnode list(%d): ", tnode->node.id);
    // int idx = 0;
    // for (int i = begin; i < end; ++i) {
    //     tnode->list.push_back(sorted_nodes[i].id);
    //     // printf("%d ", nodes[i].id);
    //     // idx++;
    // }
    // puts("");

    tnode->parent = parent;

    axis = axis == 0 ? 1 : 0;
    tnode->left =
        MakeTree(tnode, sorted_nodes, begin, median_idx, axis, depth + 1);
    tnode->right =
        MakeTree(tnode, sorted_nodes, median_idx + 1, end, axis, depth + 1);

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

// TreeNode *KdTreeIndex::MakeTree(TreeNode *parent,
//                                 vector<TreeNode *> &tnodes,
//                                 int begin,
//                                 int end,
//                                 int axis,
//                                 int depth) {
//     int nodes_size = end - begin;

//     if (nodes_size == 0) {
//         return nullptr;
//     }

//     vector<TreeNode *> &sorted_nodes = Sort(tnodes, begin, end, axis);

//     int median = nodes_size / 2;

//     int median_idx = -1;

//     median_idx = median + begin;

//     // printf("median=%d\tid=%d\t(%.2f, %.2f) axis=%d [%d, %d]\n",
//     //        median,
//     //        nodes[median_idx].id,
//     //        nodes[median_idx].pos[0],
//     //        nodes[median_idx].pos[1],
//     //        axis,
//     //        begin,
//     //        end);

//     TreeNode *tn;

//     tn               = tnodes_[sorted_nodes[median_idx]->node.id];
//     tn->own_idx      = median_idx;
//     tn->axis         = axis;
//     tn->range[0]     = begin;
//     tn->range[1]     = end;
//     tn->remove_count = 0;
//     tn->depth        = depth;

//     tn->list.clear();

//     // printf("tnode list(%d): ", tnode->node.id);
//     // int idx = 0;
//     for (int i = begin; i < end; i++) {
//         tn->list.push_back(sorted_nodes[i]->node.id);
//         // printf("%d ", nodes[i].id);
//         // idx++;
//     }
//     // puts("");

//     tn->parent = parent;

//     axis     = axis == 0 ? 1 : 0;
//     tn->left = MakeTree(tn, sorted_nodes, begin, median_idx, axis, depth +
//     1); tn->right =
//         MakeTree(tn, sorted_nodes, median_idx + 1, end, axis, depth + 1);
//     return tn;
// }
// TreeNode *KdTreeIndex::MakeTree(vector<Node> &nodes, int axis) {
//     return nullptr;
// }

// vector<int> KdTreeIndex::Query(Node &query, int radius) {
//     neighbor_.clear();
//     // printTree(tree_, nullptr, false);

//     RangeSearch(tree_, query, radius);

//     return neighbor_;
// }
vector<int> KdTreeIndex::Query(Node &query) {
    neighbor_.clear();
    // neighbor_.shrink_to_fit();
    // printTree(tree_, nullptr, false);

    RangeSearch(tree_, query, query.radius);

    return neighbor_;
}

// static inline float euclidean_intrinsic_float(int n,
//                                               const float *x,
//                                               const float *y) {
//     float result     = 0;
//     __m128 euclidean = _mm_setzero_ps();
//     for (; n > 3; n -= 4) {
//         const __m128 a            = _mm_loadu_ps(x);
//         const __m128 b            = _mm_loadu_ps(y);
//         const __m128 a_minus_b    = _mm_sub_ps(a, b);
//         const __m128 a_minus_b_sq = _mm_mul_ps(a_minus_b, a_minus_b);
//         euclidean                 = _mm_add_ps(euclidean, a_minus_b_sq);
//         x += 4;
//         y += 4;
//     }
//     const __m128 shuffle1 =
//         _mm_shuffle_ps(euclidean, euclidean, _MM_SHUFFLE(1, 0, 3, 2));
//     const __m128 sum1 = _mm_add_ps(euclidean, shuffle1);
//     const __m128 shuffle2 =
//         _mm_shuffle_ps(sum1, sum1, _MM_SHUFFLE(2, 3, 0, 1));
//     const __m128 sum2 = _mm_add_ps(sum1, shuffle2);
//     // with SSE3, we could use hadd_ps, but the difference is negligible

//     _mm_store_ss(&result, sum2);
//     //    _mm_empty();
//     return result;
// }
void KdTreeIndex::RangeSearch(TreeNode *tnode, Node &query, int radius) {

    if (tnode == nullptr) return;

    const array<float, 2> &node = tnode->node.pos;
    const array<float, 2> &q    = query.pos;

    // #ifdef SSE
    // const float x[] = {tnode->node.pos[0], query.pos[0]};
    // const float y[] = {tnode->node.pos[1], query.pos[1]};

    // float dist = 0;
    // if (tnode->node.id != -1 && query.id != tnode->node.id) {
    //     dist = euclidean_intrinsic_float(2, &x[0], &y[0]);

    //     if (dist <= radius) {
    //         neighbor_.push_back(tnode->node.id);
    //     }
    // }
    // #else

    float dist = 0;
    if (tnode->node.id != -1 && query.id != tnode->node.id) {
        dist = sqrt(pow(node[0] - q[0], 2) + pow(node[1] - q[1], 2));

        if (dist <= radius) {
            neighbor_.push_back(tnode->node.id);
        }
    }
    // #endif

    if (!tnode->left && !tnode->right) {
        return;
    }

    int axis;
    axis = tnode->axis;

    // printf("% 2d (%.2f, %.2f) axis=%d r=%d dist=%.2f\n",
    //        tnode->node.id,
    //        node[0],
    //        node[1],
    //        axis,
    //        radius,
    //        dist);

    TreeNode *next, *nnext;

    next  = tnode->left;
    nnext = tnode->right;
    if (node[axis] < q[axis]) {
        next  = tnode->right;
        nnext = tnode->left;
    }

    RangeSearch(next, query, radius);

    float diff = fabs(q[axis] - node[axis]);

    if (diff <= radius) RangeSearch(nnext, query, radius);
    return;
} // namespace neighbor_search

// inline float KdTreeIndex::Distance(const array<float, 2> &a,
//                                    const array<float, 2> &b) {
//     return sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2));
// }

// inline int KdTreeIndex::Median(vector<Node> &nodes, int begin, int end) {
//     return (end - begin) / 2;
// }
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
// inline vector<TreeNode *> &KdTreeIndex::Sort(vector<TreeNode *> &tnodes,
//                                              int begin,
//                                              int end,
//                                              int axis) {

//     sort(tnodes.begin() + begin,
//          tnodes.begin() + end,
//          [&](const TreeNode *x, const TreeNode *y) {
//              return x->node.pos[axis] < y->node.pos[axis];
//          });
//     return tnodes;
// }
// inline vector<Node> &KdTreeIndex::Sort(vector<Node> &nodes, int axis) {

//     sort(nodes.begin(), nodes.end(), [&](const Node &x, const Node &y) {
//         return x.pos[axis] < y.pos[axis];
//     });
//     return nodes;
// }

// inline vector<Node> &KdTreeIndex::Sort(vector<Node> &nodes) {
//     sort(nodes.begin(), nodes.end(), [](const Node &x, const Node &y) {
//         return x.id < y.id;
//     });
//     return nodes;
// }

void KdTreeIndex::Clear() {
    ClearTree(tree_);
}

void KdTreeIndex::Validation(vector<Node> &nodes) {
    // Sort(nodes);
    for (unsigned int i = 0; i < nodes.size(); ++i) {
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
