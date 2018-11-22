#include <iostream>
#include <set>

// #include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <using.hh>
#include <lsh/lsh.hh>

namespace neighbor_search {

LSH::LSH(int d, int k, int L) : d_(d), k_(k), L_(L) {
}

LSH::~LSH() {
}
void LSH::Init(const Value &json) {
    // fprintf(stderr, "LSH Init\n");

    vector<array<float, 2>> points;

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);

    json.Accept(writer);

    // printf("%s\n", buffer.GetString());

    for (auto &&n : json["node"].GetArray()) {
        assert(n["radius"].IsNumber());
        radius_.insert(n["radius"].GetInt());

        array<float, 2> p{static_cast<float>(n["x"].GetDouble()),
                          static_cast<float>(n["y"].GetDouble())};
        points.push_back(p);
    }

    for (auto &&r : radius_) {
        lsh_.push_back(new LSHIndex(r * 1.05, d_, k_, L_, r));
        lsh_[lsh_.size() - 1]->Index(points);
    }
}
void LSH::Init(const vector<Node> &nodes) {

    nodes_ = nodes;

    vector<array<float, 2>> points;

    for (auto &&n : nodes) {
        radius_.insert(n.radius);

        array<float, 2> p = n.pos;
        points.push_back(p);
    }
    for (auto &&r : radius_) {
        lsh_.push_back(new LSHIndex(r * 1.05, d_, k_, L_, r));
        lsh_[lsh_.size() - 1]->Index(points);
    }
}
void LSH::Update(const Value &json) {
    int id, r;
    float x, y;
    id = json["id"].GetInt();
    x  = json["x"].GetDouble();
    y  = json["y"].GetDouble();
    r  = json["r"].GetInt();

    for (auto &&lsh : lsh_) {
        if (lsh->IsSameRadius(r)) {
            array<float, 2> point{x, y};

            lsh->Update(id, point);
        }
    }
}

vector<int> LSH::GetNeighbor(const Value &json) {
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

    vector<int> neighbor;
    for (auto &&lsh : lsh_) {
        if (lsh->IsSameRadius(r)) {
            array<float, 2> point{x, y};

            neighbor = lsh->Query(point);
        }
    }

    return neighbor;
}

vector<int> LSH::GetNeighbor(const Node &node) {
    vector<int> neighbor;

    for (auto &&lsh : lsh_) {
        if (lsh->IsSameRadius(node.radius)) {
            array<float, 2> p = node.pos;

            neighbor = lsh->Query(p);
        }
    }

    return neighbor;
}

void LSH::SendDeltaHQ(vector<int> &neighbor, const Value &json, string &key) {
    Document root;

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
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.SetMaxDecimalPlaces(4);

    root.Accept(writer);
    printf("%s\n", buffer.GetString());
    std::flush(std::cout);
}

void LSH::SendDeltaHQ(vector<int> &neighbor, const Node &node, string &key) {

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
void LSH::SendDeltaHQ(void){

}
void LSH::InitDGram(const string &host, const string &port) {
}

} // namespace neighbor_search
