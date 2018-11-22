
#include <iostream>
#include <cmath>
#include <vector>
#include <array>

#include <rapidjson/document.h>
// #include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <using.hh>
#include <linear/linear.hh>

namespace neighbor_search {

Linear::Linear() {
}
Linear::~Linear() {
}
void Linear::Init(const Value &json) {

    int id = 0;
    for (auto &&n : json["node"].GetArray()) {

        Node p;
        p.id  = id;
        p.pos = array<float, 2>{static_cast<float>(n["x"].GetDouble()),
                                static_cast<float>(n["y"].GetDouble())};
        nodes_.push_back(p);
        id++;
    }

    linear_.Index(nodes_);
}
void Linear::Init(const vector<Node> &nodes) {
    nodes_ = nodes;

    for (auto &&n : nodes) {
        Node p;
        p.id  = n.id;
        p.pos = n.pos;
        nodes_.push_back(p);
    }
    linear_.Index(nodes_);
}
void Linear::Update(const Value &json) {
    int id;
    float x, y;
    id = json["id"].GetInt();
    x  = json["x"].GetDouble();
    y  = json["y"].GetDouble();

    array<float, 2> pos{x, y};
    Node p(id, pos);
    nodes_[id] = p;

    linear_.Update(p);
}

vector<int> Linear::GetNeighbor(const Value &json) {
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
    Node query(id, pos);

    neighbor = linear_.Query(query, r);
    return neighbor;
}

vector<int> Linear::GetNeighbor(const Node &node) {
    vector<int> neighbor;

    Node query(node.id, node.pos);

    neighbor = linear_.Query(query, node.radius);

    return neighbor;
}

void Linear::SendDeltaHQ(vector<int> &neighbor,
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
}

void Linear::SendDeltaHQ(vector<int> &neighbor,
                         const Node &node,
                         string &key) {

    Json root;

    root.SetObject();

    Value neighbor_object(kObjectType);
    Value neighbor_array(kArrayType);
    Value center_object(kObjectType);
    Value neighbors_object(kObjectType);
    Value update_object(kObjectType);
    Value node_object(kObjectType);

    node_object.AddMember("id", node.id, root.GetAllocator());
    node_object.AddMember("x", node.pos[0], root.GetAllocator());
    node_object.AddMember("y", node.pos[1], root.GetAllocator());

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
}
void Linear::SendDeltaHQ(void) {
}

void Linear::InitDGram(const string &host, const string &port) {
}
} // namespace neighbor_search
