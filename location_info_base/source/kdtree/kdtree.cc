#include <iostream>
#include <cmath>
#include <vector>
#include <array>

#include <rapidjson/document.h>
// #include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <using.hh>
#include <kdtree/kdtree.hh>

#define FLOAT(x) static_cast<float>(x)

namespace neighbor_search {

KdTree::KdTree() {
    is_socket_ = false;
}

KdTree::~KdTree() {
    // fprintf(stderr, "KdTree Destructor\n");
    // kdtree_.Clear();
}

void KdTree::Init(const Value &json) {

    int id = 0;
    for (auto &&node : json["node"].GetArray()) {

        Node n;
        n.id     = id;
        n.pos[0] = FLOAT(node["x"].GetDouble());
        n.pos[1] = FLOAT(node["y"].GetDouble());
        n.radius = node["radius"].GetInt();
        nodes_.push_back(n);
        id++;
    }

    kdtree_.Index(nodes_);
}
void KdTree::Init(const vector<Node> &nodes) {

    nodes_ = nodes;

    // for (auto &&n : nodes) {
    //     Node p;
    //     p.id  = n.id;
    //     p.pos = array<float, 2>{n.x, n.y};
    //     points_.push_back(p);
    // }

    kdtree_.Index(nodes_);
}
void KdTree::Update(const Value &json) {
    int id, radius;
    float x, y;
    id     = json["id"].GetInt();
    x      = json["x"].GetDouble();
    y      = json["y"].GetDouble();
    radius = json["r"].GetInt();

    Node n(id, array<float, 2>{x, y}, radius);
    nodes_[id] = n;

    // fprintf(stderr,
    //         " ========= Update % d (%.2f, %.2f)========= \n",
    //         id,
    //         n.pos[0],
    //         n.pos[1]);

    kdtree_.Update(n);
}

vector<int> KdTree::GetNeighbor(const Value &json) {
    vector<int> neighbor;
    int id, r;
    float x, y;
    id = json["id"].GetInt();
    x  = json["x"].GetDouble();
    y  = json["y"].GetDouble();
    if (json.FindMember("r") != json.MemberEnd()) {
        r = json["r"].GetInt();
    } else {
        r = json["radius"].GetInt();
    }

    array<float, 2> pos{x, y};

    Node query(id, pos, r);

    neighbor = kdtree_.Query(query, r);

    // kdtree_.Validation(nodes_);

    // if (neighbor.size() == 0) {
    //     return neighbor;
    // }
    // fprintf(stderr, "neighbor:\n");
    // sort(neighbor.begin(), neighbor.end());
    // for (auto &&i : neighbor) {
    //     fprintf(stderr, "%d ", i);
    // }
    // fprintf(stderr, "\n");

    // fprintf(stderr, "exact:\n");
    // vector<int> exact;
    // float dist = 0;
    // for (auto &&n : nodes_) {
    //     if (id == n.id) {
    //         continue;
    //     }

    //     dist = sqrt(pow(n.pos[0] - pos[0], 2) + pow(n.pos[1] - pos[1], 2));
    //     if (dist <= r) {
    //         exact.push_back(n.id);
    //     }
    // }

    // sort(exact.begin(), exact.end());
    // for (auto &&i : exact) {
    //     fprintf(stderr, "%d ", i);
    // }
    // fprintf(stderr, "\n");
    // bool is_same = false;
    // if (exact.size()) {
    //     for (auto &&exa : exact) {
    //         is_same = false;
    //         for (auto &&nei : neighbor) {
    //             if (nei == exa) {
    //                 is_same = true;
    //                 break;
    //             }
    //         }

    //         if (!is_same) {
    //             fprintf(stderr, "miss:\n");
    //             kdtree_.PrintBack(exa);
    //         }
    //     }
    // }

    // assert(exact.size() == neighbor.size());
    return neighbor;
} // namespace neighbor_search

vector<int> KdTree::GetNeighbor(const Node &node) {
    vector<int> neighbor;
    array<float, 2> pos{node.pos[0], node.pos[1]};

    Node query(node.id, pos);

    neighbor = kdtree_.Query(query, node.radius);

    return neighbor;
}

void KdTree::SendDeltaHQ(vector<int> &neighbor,
                         const Value &json,
                         string &key) {

    Json root;

    root.SetObject();

    Value neighbor_object(kObjectType);
    Value neighbor_array(kArrayType);
    Value center_object(kObjectType);
    Value neighbors_object(kObjectType);
    Value update_object(kObjectType);
    Value node_object(kObjectType);

    node_object.AddMember("id", json["id"].GetInt(), root.GetAllocator());
    node_object.AddMember("x", json["x"].GetDouble(), root.GetAllocator());
    node_object.AddMember("y", json["y"].GetDouble(), root.GetAllocator());

    neighbors_object.AddMember("center", node_object, root.GetAllocator());

    for (auto &&i : neighbor) {
        Value n(kObjectType);
        neighbor_array.PushBack(i, root.GetAllocator());
    }

    neighbors_object.AddMember(
        "neighbor", neighbor_array, root.GetAllocator());

    update_object.AddMember(
        "neighbors", neighbors_object, root.GetAllocator());

    if (key == "init") {
        root.AddMember("init", update_object, root.GetAllocator());
    }

    if (key == "update") {
        root.AddMember("update", update_object, root.GetAllocator());
    }
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    writer.SetMaxDecimalPlaces(4);

    root.Accept(writer);
    printf("%s\n", buffer.GetString());
    std::flush(std::cout);
    if (is_socket_) {
        dgram_.SendTo(buffer.GetString(), strlen(buffer.GetString()), 0);
    }
}

void KdTree::SendDeltaHQ(void) {
    string finish = R"({"finish":"finish"})";
    if (is_socket_) {
        dgram_.SendTo(finish.c_str(), finish.size(), 0);
    }
    printf("%s\n", finish.c_str());
    std::flush(std::cout);
}
void KdTree::SendDeltaHQ(vector<int> &neighbor,
                         const Node &node,
                         string &key) {

    //     Json root;

    //     root.SetObject();

    //     Value neighbor_object(kObjectType);
    //     Value neighbor_array(kArrayType);
    //     Value center_object(kObjectType);
    //     Value neighbors_object(kObjectType);
    //     Value update_object(kObjectType);
    //     Value node_object(kObjectType);

    //     node_object.AddMember("id", node.id, root.GetAllocator());
    //     node_object.AddMember("x", node.pos[0], root.GetAllocator());
    //     node_object.AddMember("y", node.pos[1], root.GetAllocator());

    //     neighbors_object.AddMember("center", node_object,
    //     root.GetAllocator());

    //     for (auto &&i : neighbor) {
    //         Value n(kObjectType);
    //         neighbor_array.PushBack(i, root.GetAllocator());
    //     }

    //     neighbors_object.AddMember(
    //         "neighbor", neighbor_array, root.GetAllocator());

    //     update_object.AddMember(
    //         "neighbors", neighbors_object, root.GetAllocator());

    //     if (key == "init") {
    //         root.AddMember("init", update_object, root.GetAllocator());
    //     }

    //     if (key == "update") {
    //         root.AddMember("update", update_object, root.GetAllocator());
    //     }
    //     StringBuffer buffer;
    //     Writer<StringBuffer> writer(buffer);
    //     writer.SetMaxDecimalPlaces(4);

    //     root.Accept(writer);
    //     printf("%s", buffer.GetString());
    //     std::flush(std::cout);
}

void KdTree::InitDGram(const string &host, const string &port) {

    dgram_.Open("AF_INET", true);
    // dgram_.Bind(host, port);
    dgram_.SetTo(host, port);
    is_socket_ = true;
}
} // namespace neighbor_search
