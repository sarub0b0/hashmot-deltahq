#include <list>
#include <iostream>
#include <cfloat>
#include <cmath>

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
    tree_ = MakeTree(points, 0, points.size(), 0);

    points_.resize(points.size());
    copy(points.begin(), points.end(), points_.begin());

    points_ = Sort(points_);

    id_sorted_points_.resize(points_.size());
    copy(points_.begin(), points_.end(), id_sorted_points_.begin());

    // printf("---------------------- Init -------------------\n\n");
    // printTree(tree_, nullptr, false);
}

void KdTreeIndex::Update(Point &point) {

    // printf("search: %d\n", point.id);

    Node *node = nullptr;

    // Remove
    // Rebuild
    node  = ReMakeTree(tree_, id_sorted_points_[point.id]);
    tree_ = node;

    if (node != nullptr) {
        // printf("find: %d\n", node->point.id);

        // printf("---------------------- Remake -------------------\n\n");

    } else {
        printf("Can't find id=%d\n", point.id);
    }

    // printTree(tree_, nullptr, false);

    // Add
    id_sorted_points_[point.id].pos = point.pos;
    AddIndex(tree_, point);
    // printTree(tree_, nullptr, false);
}

void KdTreeIndex::AddIndex(Node *node, Point &point) {
    if (node == nullptr) {
        return;
    }
    int axis     = node->axis;
    bool is_left = false;
    Node *next   = nullptr;

    if (point.pos[axis] < node->point.pos[axis]) {
        next    = node->left;
        is_left = true;
    } else {
        next = node->right;
    }

    node->list.push_back(point.id);

    if (next == nullptr) {
        next = new Node();
        axis = axis == 0 ? 1 : 0;

        next->point = point;
        next->axis  = axis;
        next->list.push_back(point.id);

        if (is_left) {
            node->left = next;
        } else {
            node->right = next;
        }

        return;
    } else {
        AddIndex(next, point);
    }
    return;
}

Node *KdTreeIndex::Search(Node *node, Point &point) {
    return nullptr;
}

Node *KdTreeIndex::ReMakeTree(Node *node, Point &point) {
    if (node == nullptr) return nullptr;

    int axis;

    axis = node->axis;

    Node *next;

    bool is_left = false;
    if (point.pos[axis] < node->point.pos[axis]) {
        next    = node->left;
        is_left = true;
    } else {
        next = node->right;
    }

    for (list<int>::iterator itr = node->list.begin();
         itr != node->list.end();
         itr++) {

        if (*itr == point.id) {
            node->list.erase(itr);
            break;
        }
    }

    if (node->point.id == point.id ||
        (next != nullptr && next->point.id == point.id)) {
        // printf("remake: %d\n", node->point.id);
        // remove point from points_
        // printf("list_size=%lu range[%d, %d]\n",
        //        node->list.size(),
        //        node->range[0],
        //        node->range[1]);

        // printf("reamke list: ");

        int idx = 0;

        vector<Point> list(node->list.size());

        for (auto &&i : node->list) {
            list[idx] = id_sorted_points_[i];
            // printf("%d ", list[idx].id);
            idx++;
        }

        // puts("");

        ClearTree(node->left);
        ClearTree(node->right);

        node = MakeTree(list, 0, list.size(), node->axis);

        return node;
    } else {
        Node *new_tree;
        new_tree = ReMakeTree(next, point);
        if (is_left) {
            node->left = new_tree;
        } else {
            node->right = new_tree;
        }
    }

    return node;
}

void KdTreeIndex::ClearTree(Node *node) {
    if (node == nullptr) {
        return;
    }

    if (node->left != nullptr) {
        ClearTree(node->left);
    }
    if (node->right != nullptr) {
        ClearTree(node->right);
    }
    delete node;
    node = nullptr;
    return;
}

Node *KdTreeIndex::MakeTree(vector<Point> &points,
                            int begin,
                            int end,
                            int axis) {
    int points_size = -1;
    points_size     = end - begin;
    if (points_size == 0) {
        return nullptr;
    }

    int median = -1;

    points = Sort(points, begin, end, axis);

    median = points_size / 2;

    int median_idx = -1;

    median_idx = median + begin;

    // printf("median=%d\tid=%d\t(%.2f, %.2f) axis=%d [%d, %d]\n",
    //        median,
    //        points[median_idx].id,
    //        points[median_idx].pos[0],
    //        points[median_idx].pos[1],
    //        axis,
    //        begin,
    //        end);

    Node *node = new Node();

    node->point    = points[median_idx];
    node->axis     = axis;
    node->range[0] = begin;
    node->range[1] = end;

    // printf("node list(%d): ", node->point.id);
    int idx = 0;
    for (int i = begin; i < end; i++) {
        node->list.push_back(points[i].id);
        // printf("%d ", points[i].id);
        idx++;
    }
    // puts("");

    axis        = axis == 0 ? 1 : 0;
    node->left  = MakeTree(points, begin, median_idx, axis);
    node->right = MakeTree(points, median_idx + 1, end, axis);

    return node;
}
Node *KdTreeIndex::MakeTree(vector<Point> &points, int axis) {
    int points_size = -1;
    points_size     = points.size();
    if (points_size == 0) {
        return nullptr;
    }

    int median = -1;

    points = Sort(points, axis);

    median = points_size / 2;

    printf("median=%d\tid=%d\t(%.2f, %.2f) axis=%d\n",
           median,
           points[median].id,
           points[median].pos[0],
           points[median].pos[1],
           axis);

    int left_size  = -1;
    int right_size = -1;

    left_size  = median;
    right_size = points_size - left_size - 1;

    vector<Point> left(left_size);
    vector<Point> right(right_size);

    copy(points.begin(), points.begin() + left_size, left.begin());
    copy(points.begin() + left_size + 1, points.end(), right.begin());

    Node *node  = new Node();
    node->point = points[median];
    node->axis  = axis;

    axis        = axis == 0 ? 1 : 0;
    node->left  = MakeTree(left, axis);
    node->right = MakeTree(right, axis);

    return node;
}

vector<int> KdTreeIndex::Query(Point &query, int radius) {
    neighbor_.clear();
    RangeSearch(tree_, query, radius);

    return neighbor_;
}
void KdTreeIndex::RangeSearch(Node *node, Point &query, int radius) {

    if (node == nullptr) return;

    array<float, 2> &point = node->point.pos;

    float dist = Distance(query.pos, point);

    // TODO queryと同じidはネイバーに入れない処理を追加
    if (dist < radius && query.id != node->point.id) {
        neighbor_.push_back(node->point.id);
    }

    // printf("search: %d %.2f\n", node->point.id, dist);

    int axis;
    axis = node->axis;

    Node *next, *nnext;

    if (query.pos[axis] < point[axis]) {
        next  = node->left;
        nnext = node->right;
    } else {
        next  = node->right;
        nnext = node->left;
    }

    RangeSearch(next, query, radius);

    float diff = fabs(query.pos[axis] - point[axis]);

    if (diff < radius) RangeSearch(nnext, query, radius);
    return;
}

float KdTreeIndex::Distance(array<float, 2> &a, array<float, 2> &b) {
    return sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2));
}

int KdTreeIndex::Median(vector<Point> &points, int begin, int end) {
    return (end - begin) / 2;
}
vector<Point> &KdTreeIndex::Sort(vector<Point> &points,
                                 int begin,
                                 int end,
                                 int axis) {

    sort(points.begin() + begin,
         points.begin() + end,
         [&](const Point &x, const Point &y) {
             return x.pos[axis] < y.pos[axis];
         });
    return points;
}
vector<Point> &KdTreeIndex::Sort(vector<Point> &points, int axis) {

    sort(points.begin(), points.end(), [&](const Point &x, const Point &y) {
        return x.pos[axis] < y.pos[axis];
    });
    return points;
}

vector<Point> &KdTreeIndex::Sort(vector<Point> &points) {
    sort(points.begin(), points.end(), [](const Point &x, const Point &y) {
        return x.id < y.id;
    });
    return points;
}

void KdTreeIndex::Clear() {
    ClearTree(tree_);
}
} // namespace neighbor_search
