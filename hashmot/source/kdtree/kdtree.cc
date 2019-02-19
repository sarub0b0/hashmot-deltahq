// MIT License

// Copyright (c) 2019 kosay

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include <iostream>
#include <cmath>
#include <vector>
#include <array>
#include <chrono>

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
    // char init_prefix[]   = "{\"init\":{\"neighbors\":{\"center\":{\"id\":";
    // char update_prefix[] =
    // "{\"update\":{\"neighbors\":{\"center\":{\"id\":";

    // strcpy(send_init_buffer_, init_prefix);
    // strcpy(send_update_buffer_, update_prefix);

    // init_buffer_pos_   = strlen(init_prefix);
    // update_buffer_pos_ = strlen(update_prefix);

    max_neighbors_ = (9000 - sizeof(struct send_data)) / sizeof(int32_t);
}

KdTree::~KdTree() {
    // fprintf(stderr, "KdTree Destructor\n");
    // kdtree_.Clear();
}

void KdTree::Init(const Value &json) {

    // {"init":{"neighbors":{"center":{"id":99983,"x":8833.24,"y":12147.32},"neighbor":[40646]}}}
    // {"update":{"neighbors":{"center":{"id":99985,"x":16287.1,"y":18899.9},"neighbor":[42784]}}}
    // string send_init_   = R"({"init":{"neighbors":{"center":{"id":)";
    // string send_update_ = R"({"update":{"neighbors":{"center":{"id":)";
    int node_number = json["node"].Size();
    // printf("node_number(%d)\n", node_number);

    // int node_number = 0;
    // for (auto &&node : json["node"].GetArray()) {
    //     ++node_number;
    // }

    nodes_.reserve(node_number + 1);

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
} // namespace neighbor_search
// void KdTree::Init(const vector<Node> &nodes) {

//     nodes_ = nodes;

//     // for (auto &&n : nodes) {
//     //     Node p;
//     //     p.id  = n.id;
//     //     p.pos = array<float, 2>{n.x, n.y};
//     //     points_.push_back(p);
//     // }

//     kdtree_.Index(nodes_);
// }
int KdTree::Update(const Value &json, struct send_data &send_data) {
    int id;
    // int radius;
    // float x, y;
    id = json["id"].GetInt();
    // x      = json["x"].GetDouble();
    // y      = json["y"].GetDouble();
    // radius = json["r"].GetInt();

    // Node n(id, array<float, 2>{x, y}, radius);
    // nodes_[id] = n;

    nodes_[id].pos[0] = json["x"].GetDouble();
    nodes_[id].pos[1] = json["y"].GetDouble();
    nodes_[id].radius = json["r"].GetInt();

    send_data.id = id;
    send_data.x  = nodes_[id].pos[0];
    send_data.y  = nodes_[id].pos[1];
    // fprintf(stderr,
    //         " ========= Update % d (%.2f, %.2f)========= \n",
    //         id,
    //         n.pos[0],
    //         n.pos[1]);

    // kdtree_.Update(n);
    kdtree_.Update(nodes_[id]);
    return id;
}

// vector<int> KdTree::GetNeighbor(int id) {
void KdTree::GetNeighbor(int id, struct send_data &send_data) {
    // vector<int> neighbor;
    // int id, r;
    // float x, y;
    // id = json["id"].GetInt();
    // x  = json["x"].GetDouble();
    // y  = json["y"].GetDouble();
    // if (json.FindMember("r") != json.MemberEnd()) {
    //     r = json["r"].GetInt();
    // } else {
    //     r = json["radius"].GetInt();
    // }
    send_data.x = nodes_[id].pos[0];
    send_data.y = nodes_[id].pos[1];

    // array<float, 2> pos{x, y};

    // Node query(id, pos, r);

    // neighbor = kdtree_.Query(nodes_[id]);
    kdtree_.Query(nodes_[id], send_data);

    // if (neighbor.size() == 0) {
    if (send_data.neighbor_size == 0) {
        send_data.neighbor[0] = -1;
        return;
        // return neighbor;
    }
#ifdef QUERY_VALIDATION
    kdtree_.Validation(nodes_);

    fprintf(stderr, "neighbor:\n");
    sort(neighbor.begin(), neighbor.end());
    for (auto &&i : neighbor) {
        fprintf(stderr, "%d ", i);
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "exact:\n");
    vector<int> exact;
    exact.reserve(neighbor.size());
    float dist           = 0;
    int r                = nodes_[id].radius;
    array<float, 2> &pos = nodes_[id].pos;
    for (auto &&n : nodes_) {
        if (id == n.id) {
            continue;
        }

        dist = sqrt(pow(n.pos[0] - pos[0], 2) + pow(n.pos[1] - pos[1], 2));
        if (dist <= r) {
            exact.push_back(n.id);
        }
    }

    sort(exact.begin(), exact.end());
    for (auto &&i : exact) {
        fprintf(stderr, "%d ", i);
    }
    fprintf(stderr, "\n");
    bool is_same = false;
    if (exact.size()) {
        for (auto &&exa : exact) {
            is_same = false;
            for (auto &&nei : neighbor) {
                if (nei == exa) {
                    is_same = true;
                    break;
                }
            }

            if (!is_same) {
                fprintf(stderr, "miss:\n");
                kdtree_.PrintBack(exa);
            }
        }
    }

    assert(exact.size() == neighbor.size());
#endif
    // return neighbor;
} // namespace neighbor_search
void KdTree::SendDeltaHQ(const struct send_data &send_data) {

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
void KdTree::SendDeltaHQ(void) {
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
void KdTree::InitDGram(const string &host, const string &port) {
    dgram_.Open("AF_INET", true);
    // dgram_.Bind(host, port);
    dgram_.SetTo(host, port);
    is_socket_ = true;
}
} // namespace neighbor_search
