#include <iostream>
#include <set>

// #include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <using.hh>
#include <lsh/lsh.hh>

#define FLOAT(x) static_cast<float>(x)
namespace neighbor_search {

LSH::LSH(int d, int k, int L) : d_(d), k_(k), L_(L) {
}

LSH::~LSH() {
}
void LSH::Init(const Value &json) {
    // fprintf(stderr, "LSH Init\n");

    char init_prefix[]   = "{\"init\":{\"neighbors\":{\"center\":{\"id\":";
    char update_prefix[] = "{\"update\":{\"neighbors\":{\"center\":{\"id\":";

    strcpy(send_init_buffer_, init_prefix);
    strcpy(send_update_buffer_, update_prefix);

    init_buffer_pos_   = strlen(init_prefix);
    update_buffer_pos_ = strlen(update_prefix);

    vector<array<float, 2>> points;

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);

    json.Accept(writer);

    // printf("%s\n", buffer.GetString());
    int id = 0;
    for (auto &&node : json["node"].GetArray()) {
        assert(node["radius"].IsNumber());
        radius_.insert(node["radius"].GetInt());

        array<float, 2> p{static_cast<float>(node["x"].GetDouble()),
                          static_cast<float>(node["y"].GetDouble())};
        points.push_back(p);

        Node n;
        n.id     = id;
        n.pos[0] = FLOAT(node["x"].GetDouble());
        n.pos[1] = FLOAT(node["y"].GetDouble());
        n.radius = node["radius"].GetInt();
        nodes_.push_back(n);
        id++;
    }

    for (auto &&r : radius_) {
        lsh_.push_back(new LSHIndex(r * 1.05, d_, k_, L_, r));
        lsh_[lsh_.size() - 1]->Index(points);
    }
}
// void LSH::Init(const vector<Node> &nodes) {

//     nodes_ = nodes;

//     vector<array<float, 2>> points;

//     for (auto &&n : nodes) {
//         radius_.insert(n.radius);

//         array<float, 2> p = n.pos;
//         points.push_back(p);
//     }
//     for (auto &&r : radius_) {
//         lsh_.push_back(new LSHIndex(r * 1.05, d_, k_, L_, r));
//         lsh_[lsh_.size() - 1]->Index(points);
//     }
// }
int LSH::Update(const Value &json) {
    int id, r;
    float x, y;
    id = json["id"].GetInt();
    x  = json["x"].GetDouble();
    y  = json["y"].GetDouble();
    r  = json["r"].GetInt();

    nodes_[id].pos[0] = x;
    nodes_[id].pos[1] = y;
    nodes_[id].radius = r;

    for (auto &&lsh : lsh_) {
        if (lsh->IsSameRadius(r)) {
            array<float, 2> point{x, y};

            lsh->Update(id, point);
        }
    }
    return id;
}

// vector<int> LSH::GetNeighbor(const Value &json) {
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

//     vector<int> neighbor;
//     for (auto &&lsh : lsh_) {
//         if (lsh->IsSameRadius(r)) {
//             array<float, 2> point{x, y};

//             neighbor = lsh->Query(point);
//         }
//     }

//     return neighbor;
// }

vector<int> LSH::GetNeighbor(int id) {
    int r;
    float x, y;
    x = nodes_[id].pos[0];
    y = nodes_[id].pos[1];
    r = nodes_[id].radius;

    vector<int> neighbor;
    for (auto &&lsh : lsh_) {
        if (lsh->IsSameRadius(r)) {
            array<float, 2> point{x, y};

            neighbor = lsh->Query(point);
        }
    }

    return neighbor;
}
// vector<int> LSH::GetNeighbor(const Node &node) {
//     vector<int> neighbor;

//     for (auto &&lsh : lsh_) {
//         if (lsh->IsSameRadius(node.radius)) {
//             array<float, 2> p = node.pos;

//             neighbor = lsh->Query(p);
//         }
//     }

//     return neighbor;
// }

// void LSH::SendDeltaHQ(vector<int> &neighbor, const Value &json, string
// &key) {
//     Document root;

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
//     rapidjson::StringBuffer buffer;
//     rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
//     writer.SetMaxDecimalPlaces(4);

//     root.Accept(writer);
//     printf("%s\n", buffer.GetString());
//     std::flush(std::cout);
// }

void LSH::SendDeltaHQ(const vector<int> &neighbor, int id, string &key) {

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
void LSH::SendDeltaHQ(void) {

    string finish = R"({"finish":"finish"})";
    if (is_socket_) {
        dgram_.SendTo(finish.c_str(), finish.size(), 0);
    }
#ifndef MEASURE
    printf("%s\n", finish.c_str());
    std::flush(std::cout);
#endif
}
void LSH::InitDGram(const string &host, const string &port) {

    dgram_.Open("AF_INET", true);
    // dgram_.Bind(host, port);
    dgram_.SetTo(host, port);
    is_socket_ = true;
}

} // namespace neighbor_search
