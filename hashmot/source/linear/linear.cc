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

#include <rapidjson/document.h>
// #include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <using.hh>
#include <linear/linear.hh>

namespace neighbor_search {

Linear::Linear() {

    is_socket_     = false;
    max_neighbors_ = (9000 - sizeof(struct send_data)) / sizeof(int32_t);
}
Linear::~Linear() {
}
void Linear::Init(const Value &json) {

    int node_number = json["node"].Size();
    // printf("node_number(%d)\n", node_number);
    // for (auto &&node : json["node"].GetArray()) {
    //     ++node_number;
    // }

    nodes_.reserve(node_number + 1);
    // neighbor_.reserve(node_number);

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
int Linear::Update(const Value &json, struct send_data &send_data) {
    int id;
    // float x, y;
    id = json["id"].GetInt();
    // x                 = json["x"].GetDouble();
    // y                 = json["y"].GetDouble();
    nodes_[id].pos[0] = json["x"].GetDouble();
    nodes_[id].pos[1] = json["y"].GetDouble();
    nodes_[id].radius = json["r"].GetInt();

    send_data.id = id;
    send_data.x  = nodes_[id].pos[0];
    send_data.y  = nodes_[id].pos[1];
    // array<float, 2> pos{x, y};
    // Node p(id, pos);
    // nodes_[id] = p;

    linear_.Update(nodes_[id]);
    return id;
}
// vector<int> Linear::GetNeighbor(int id) {
void Linear::GetNeighbor(int id, struct send_data &send_data) {
    int r;
    r = nodes_[id].radius;

    send_data.x = nodes_[id].pos[0];
    send_data.y = nodes_[id].pos[1];

    // neighbor->clear();
    // neighbor_ = linear_.Query(nodes_[id], r);
    linear_.Query(nodes_[id], send_data);
    if (send_data.neighbor_size == 0) {
        send_data.neighbor[0] = -1;
        return;
        // return neighbor;
    }
    // return neighbor_;
}
void Linear::SendDeltaHQ(const struct send_data &send_data) {

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
void Linear::SendDeltaHQ(void) {
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
void Linear::InitDGram(const string &host, const string &port) {
    dgram_.Open("AF_INET", true);
    // dgram_.Bind(host, port);
    dgram_.SetTo(host, port);
    is_socket_ = true;
}
} // namespace neighbor_search
