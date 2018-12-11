
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
        p.id     = id;
        p.pos    = array<float, 2>{static_cast<float>(n["x"].GetDouble()),
                                static_cast<float>(n["y"].GetDouble())};
        p.radius = n["radius"].GetInt();
        nodes_.push_back(p);
        id++;
    }

    linear_.Index(nodes_);
}
// void Linear::Init(const vector<Node> &nodes) {
//     nodes_ = nodes;

//     for (auto &&n : nodes) {
//         Node p;
//         p.id  = n.id;
//         p.pos = n.pos;
//         nodes_.push_back(p);
//     }
//     linear_.Index(nodes_);
// }
int Linear::Update(const Value &json) {
    int id;
    // float x, y;
    id = json["id"].GetInt();
    // x                 = json["x"].GetDouble();
    // y                 = json["y"].GetDouble();
    nodes_[id].pos[0] = json["x"].GetDouble();
    nodes_[id].pos[1] = json["y"].GetDouble();
    nodes_[id].radius = json["r"].GetInt();

    // array<float, 2> pos{x, y};
    // Node p(id, pos);
    // nodes_[id] = p;

    linear_.Update(nodes_[id]);
    return id;
}
vector<int> Linear::GetNeighbor(int id) {
    vector<int> neighbor;
    int r;
    r = nodes_[id].radius;

    neighbor = linear_.Query(nodes_[id], r);

    if (neighbor.size() == 0) {
        neighbor.push_back(-1);
    }
    return neighbor;
}
// vector<int> Linear::GetNeighbor(const Value &json) {
//     vector<int> neighbor;
//     int id, r;
//     float x, y;
//     id = json["id"].GetInt();
//     x  = json["x"].GetDouble();
//     y  = json["y"].GetDouble();
//     if (json.FindMember("r") != json.MemberEnd()) {
//         r = json["r"].GetInt();
//     } else {
//         r = json["radius"].GetInt();
//     }

//     array<float, 2> pos{x, y};
//     Node query(id, pos);

//     neighbor = linear_.Query(query, r);
//     return neighbor;
// }

// void Linear::SendDeltaHQ(vector<int> &neighbor,
//                          const Value &json,
//                          string &key) {

//     Json root;

//     root.SetObject();

//     Value neighbor_object(kObjectType);
//     Value neighbor_array(kArrayType);
//     Value center_object(kObjectType);
//     Value neighbors_object(kObjectType);
//     Value update_object(kObjectType);
//     Value node_object(kObjectType);

//     node_object.AddMember("id", json["id"].GetInt(), root.GetAllocator());
//     node_object.AddMember("x", json["x"].GetDouble(), root.GetAllocator());
//     node_object.AddMember("y", json["y"].GetDouble(), root.GetAllocator());

//     neighbors_object.AddMember("center", node_object, root.GetAllocator());

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
//     printf("%s\n", buffer.GetString());
//     std::flush(std::cout);
// }

void Linear::SendDeltaHQ(const vector<int> &neighbor, int id, string &key) {

#ifdef MEASURE
    // chrono::high_resolution_clock::time_point begin =
    //     chrono::high_resolution_clock::now();
#endif

    // {"init":{"neighbors":{"center":{"id":0,"x":3.24,"y":47.32},"neighbor":[6]}}}
    // {"update":{"neighbors":{"center":{"id":0,"x":87.1,"y":9.9},"neighbor":[4]}}}
    // string buffer;
    // int id;
    float x, y;
    char *buffer;

    // id = json["id"].GetInt();
    x = nodes_[id].pos[0];
    y = nodes_[id].pos[1];

    int str_number = 0;
    if (key == "init") {
        buffer     = send_init_buffer_;
        str_number = init_buffer_pos_;
        str_number += sprintf(&buffer[str_number],
                              "%d,\"x\":%.2f,\"y\":%.2f},\"neighbor\":[",
                              id,
                              x,
                              y);

        for (auto &&n : neighbor) {
            str_number += sprintf(&buffer[str_number], "%d,", n);
        }
        str_number = sprintf(&buffer[str_number - 1], "]}}}");
    }

    if (key == "update") {
        buffer     = send_update_buffer_;
        str_number = update_buffer_pos_;
        str_number += sprintf(&buffer[str_number],
                              "%d,\"x\":%.2f,\"y\":%.2f},\"neighbor\":[",
                              id,
                              x,
                              y);

        for (auto &&n : neighbor) {
            str_number += sprintf(&buffer[str_number], "%d,", n);
        }
        str_number = sprintf(&buffer[str_number - 1], "]}}}");
    }
#ifndef MEASURE
    // printf("%s\n", buffer.GetString());
    printf("%s\n", buffer);
    std::flush(std::cout);
#else
    // chrono::high_resolution_clock::time_point end =
    //     chrono::high_resolution_clock::now();
    // std::chrono::nanoseconds elapsed =
    //     chrono::duration_cast<chrono::nanoseconds>(end - begin);
    // printf("send:%lld.%09lld\n",
    //        elapsed.count() / 1000000000,
    //        elapsed.count() % 1000000000);
#endif
    if (is_socket_) {
        // dgram_.SendTo(buffer.GetString(), strlen(buffer.GetString()), 0);
        dgram_.SendTo(buffer, strlen(buffer), 0);
    }
}
void Linear::SendDeltaHQ(void) {
    string finish = R"({"finish":"finish"})";
    if (is_socket_) {
        dgram_.SendTo(finish.c_str(), finish.size(), 0);
    }
#ifndef MEASURE
    printf("%s\n", finish.c_str());
    std::flush(std::cout);
#endif
}

void Linear::InitDGram(const string &host, const string &port) {
    dgram_.Open("AF_INET", true);
    // dgram_.Bind(host, port);
    dgram_.SetTo(host, port);
    is_socket_ = true;
}
} // namespace neighbor_search
