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
    is_socket_ = false;

    max_neighbors_ = (9000 - sizeof(struct send_data)) / sizeof(int32_t);

    // char init_prefix[]   = "{\"init\":{\"neighbors\":{\"center\":{\"id\":";
    // char update_prefix[] =
    // "{\"update\":{\"neighbors\":{\"center\":{\"id\":";

    // strcpy(send_init_buffer_, init_prefix);
    // strcpy(send_update_buffer_, update_prefix);

    // init_buffer_pos_   = strlen(init_prefix);
    // update_buffer_pos_ = strlen(update_prefix);
}

LSH::~LSH() {
}
void LSH::Init(const Value &json) {
    // fprintf(stderr, "LSH Init\n");

    vector<array<float, 2>> points;

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);

    json.Accept(writer);

    // int node_number = 0;
    int node_number = json["node"].Size();
    // printf("node_number(%d)\n", node_number);
    // for (auto &&n : json["node"].GetArray()) {
    //     ++node_number;
    // }

    points.reserve(node_number + 1);
    points.resize(node_number);
    points.clear();

    nodes_.reserve(node_number + 1);
    nodes_.resize(node_number);
    nodes_.clear();

    // neighbor_.reserve(node_number);

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
int LSH::Update(const Value &json, struct send_data &send_data) {
    int id, r;
    // float x, y;
    id = json["id"].GetInt();
    // x  = json["x"].GetDouble();
    // y  = json["y"].GetDouble();
    r = json["r"].GetInt();

    nodes_[id].pos[0] = json["x"].GetDouble();
    nodes_[id].pos[1] = json["y"].GetDouble();
    nodes_[id].radius = r;

    send_data.id = id;
    send_data.x  = nodes_[id].pos[0];
    send_data.y  = nodes_[id].pos[1];

    for (auto &&lsh : lsh_) {
        if (lsh->IsSameRadius(r)) {
            // array<float, 2> p{x, y};
            // lsh->Update(id, p);
            lsh->Update(id, nodes_[id].pos);
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

// vector<int> LSH::GetNeighbor(int id) {
void LSH::GetNeighbor(int id, struct send_data &send_data) {
    int r;
    // float x, y;
    // x = nodes_[id].pos[0];
    // y = nodes_[id].pos[1];
    r = nodes_[id].radius;

    send_data.x = nodes_[id].pos[0];
    send_data.y = nodes_[id].pos[1];

    // neighbor_.clear();

    // neighbor.reserve(nodes_.size());
    for (auto &&lsh : lsh_) {
        if (lsh->IsSameRadius(r)) {
            // array<float, 2> point{x, y};
            // neighbor = lsh->Query(point);
            // neighbor_ = lsh->Query(nodes_[id].pos);
            lsh->Query(nodes_[id].pos, send_data);
        }
    }
    if (send_data.neighbor_size == 0) {
        send_data.neighbor[0] = -1;
        return;
        // return neighbor;
    }

    // return neighbor_;
}

void LSH::SendDeltaHQ(const struct send_data &send_data) {

    int data_len = 0;
    data_len     = sizeof(struct send_data) +
               (sizeof(int32_t) * (send_data.neighbor_size - 1));

#ifndef MEASURE
    if (!is_socket_) {

        printf("% 3d: %d(%.2f, %.2f) neighbor(%d): ",
               send_data.type,
               send_data.id,
               send_data.x,
               send_data.y,
               send_data.neighbor_size);

        for (int i = 0; i < send_data.neighbor_size; i++) {
            printf("%d ", send_data.neighbor[i]);
        }
        puts("");
        std::flush(std::cout);
    }
#else
#endif

    if (is_socket_) {
#ifdef DEBUG_PRINT
        printf("% 3d: %d(%.2f, %.2f) neighbor(%d): ",
               send_data.type,
               send_data.id,
               send_data.x,
               send_data.y,
               send_data.neighbor_size);

        for (int i = 0; i < send_data.neighbor_size; i++) {
            printf("%d ", send_data.neighbor[i]);
        }
        puts("");
#endif
        dgram_.SendTo(&send_data, data_len, 0);
    }
}
void LSH::SendDeltaHQ(void) {
    struct finish_data finish_data;
    finish_data.type = -1;
    string finish    = R"({"finish":"finish"})";
    if (is_socket_) {
        // dgram_.SendTo(finish.c_str(), finish.size(), 0);
        dgram_.SendTo(&finish_data, sizeof(struct finish_data), 0);
    }
#ifndef MEASURE
    if (!is_socket_) {
        printf("%s\n", finish.c_str());
        std::flush(std::cout);
    }
#endif
}
void LSH::InitDGram(const string &host, const string &port) {
    dgram_.Open("AF_INET", true);
    // dgram_.Bind(host, port);
    dgram_.SetTo(host, port);
    is_socket_ = true;
}
} // namespace neighbor_search
