#include <iostream>
#include <cmath>
#include <vector>
#include <array>

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <using.hh>
#include <kdtree/kdtree.hh>

namespace neighbor_search {

KdTree::KdTree() {
}

KdTree::~KdTree() {
    // fprintf(stderr, "KdTree Destructor\n");
    kdtree_.Clear();
}

void KdTree::Init(const Value &json) {

    int id = 0;
    for (auto &&n : json["node"].GetArray()) {

        Point p;
        p.id  = id;
        p.pos = array<float, 2>{static_cast<float>(n["x"].GetDouble()),
                                static_cast<float>(n["y"].GetDouble())};
        points_.push_back(p);
        id++;
    }

    kdtree_.Index(points_);
}

void KdTree::Update(const Value &json) {
    int id;
    float x, y;
    id = json["id"].GetInt();
    x  = json["x"].GetDouble();
    y  = json["y"].GetDouble();

    array<float, 2> pos{x, y};
    Point p(id, pos);
    points_[id] = p;

    kdtree_.Update(p);
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
    Point query(id, pos);

    neighbor = kdtree_.Query(query, r);

    // printf("neighbor:\n");
    // sort(neighbor.begin(), neighbor.end());
    // for (auto &&i : neighbor) {
    //     printf("%d ", i);
    // }
    // puts("");

    // printf("exact:\n");
    // vector<int> exact;
    // for (auto &&p : points_) {
    //     if (id == p.id) {
    //         continue;
    //     }
    //     if (sqrt(pow(p.pos[0] - pos[0], 2) + pow(p.pos[1] - pos[1], 2)) <
    //     r) {
    //         exact.push_back(p.id);
    //     }
    // }
    // sort(exact.begin(), exact.end());
    // for (auto &&i : exact) {
    //     printf("%d ", i);
    // }
    // puts("");
    return neighbor;
}

void KdTree::SendDeltaHQ(vector<int> &neighbor,
                         const Value &json,
                         string &key) {

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

    root.Accept(writer);
    printf("%s\n", buffer.GetString());
    std::flush(std::cout);
}

} // namespace neighbor_search
