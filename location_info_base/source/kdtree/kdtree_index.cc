#include <iostream>

#include <kdtree/kdtree_index.hh>
#include <kdtree/tree.hh>

namespace neighbor_search {

// Helper function to print branches of the binary tree
void KdTreeIndex::showTrunks(Trunk *p) {
    if (p == nullptr) return;
    showTrunks(p->prev);
    cout << p->str;
}

// Recursive function to print binary tree. It uses inorder traversal
// call as printTree(root, nullptr, false);
void KdTreeIndex::printTree(Node *root, Trunk *prev, bool isLeft) {
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
    cout << root->point.id << endl;
    if (prev) prev->str = prev_str;

    trunk->str = "    |";
    printTree(root->right, trunk, false);
}

KdTreeIndex::KdTreeIndex() {
}
KdTreeIndex::~KdTreeIndex() {
}

void KdTreeIndex::Index(vector<Point> &points) {
    // tree_ = MakeTree(points, 0, points.size(), 0);
    tree_ = MakeTree(points, 0);

    printf("-----------------------------------------\n\n");
    printTree(tree_, nullptr, false);
}

void KdTreeIndex::Update(Point &point) {
}
void KdTreeIndex::Query(array<float, 2> &q) {
}

// Node *KdTreeIndex::MakeTree(vector<Point> &points,
//                             int begin,
//                             int end,
//                             int axis) {
//     if (end - begin == 1) {
//         return nullptr;
//     }

//     int median = -1;
//     int idx    = -1;

//     axis = axis == 0 ? 1 : 0;

//     points = Tree::Sort(points, begin, end, axis);

//     median = (end - begin) / 2;

//     idx = median + begin;

//     printf("begin=%d \tend=%d\tmedian=%d\tidx=%d\tid=%d\t(%.2f, %.2f)\n",
//            begin,
//            end,
//            median,
//            idx,
//            points[idx].id,
//            points[idx].point[0],
//            points[idx].point[1]);

//     Node *node = new Node();

//     node->point = points[idx];
//     node->axis  = axis;
//     node->left  = MakeTree(points, begin, idx, axis);
//     node->right = MakeTree(points, idx, end, axis);

//     return node;
// }
Node *KdTreeIndex::MakeTree(vector<Point> &points, int axis) {
    int points_size = -1;
    points_size     = points.size();
    if (points_size == 0) {
        return nullptr;
    }

    int median = -1;

    points = Tree::Sort(points, axis);

    median = points_size / 2;

    printf("median=%d\tid=%d\t(%.2f, %.2f) axis=%d\n",
           median,
           points[median].id,
           points[median].point[0],
           points[median].point[1],
           axis);

    int left_size  = -1;
    int right_size = -1;

    left_size  = median;
    right_size = points_size - left_size - 1;

    vector<Point> left(left_size);
    vector<Point> right(right_size);

    copy(points.begin(), points.begin() + left_size, left.begin());
    copy(points.begin() + left_size + 1, points.end(), right.begin());

    // printf("poits_size=%d\n", points_size);
    // printf("left_size=%lu\n", left.size());
    // for (auto &&p : left) {
    //     printf("%d ", p.id);
    // }
    // puts("");
    // printf("right_size=%lu\n", right.size());
    // for (auto &&p : right) {
    //     printf("%d ", p.id);
    // }
    // puts("");

    axis = axis == 0 ? 1 : 0;

    Node *node = new Node();

    node->point = points[median];
    node->axis  = axis;
    node->left  = MakeTree(left, axis);
    node->right = MakeTree(right, axis);

    return node;
}

vector<int> KdTreeIndex::RangeSearch(array<float, 2> &query, int radius) {
    vector<int> neighbor;
    return neighbor;
}
} // namespace neighbor_search
