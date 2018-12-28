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

    is_socket_           = false;
    char init_prefix[]   = "{\"init\":{\"neighbors\":{\"center\":{\"id\":";
    char update_prefix[] = "{\"update\":{\"neighbors\":{\"center\":{\"id\":";

    strcpy(send_init_buffer_, init_prefix);
    strcpy(send_update_buffer_, update_prefix);

    init_buffer_pos_   = strlen(init_prefix);
    update_buffer_pos_ = strlen(update_prefix);
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
int KdTree::Update(const Value &json) {
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
void KdTree::GetNeighbor(int id, vector<int> &neighbor) {
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

    // array<float, 2> pos{x, y};

    // Node query(id, pos, r);

    // neighbor = kdtree_.Query(nodes_[id]);
    kdtree_.Query(nodes_[id], neighbor);

    if (neighbor.size() == 0) {
        neighbor.push_back(-1);
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

// vector<int> KdTree::GetNeighbor(const Value &json) {
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

//     Node query(id, pos, r);

//     neighbor = kdtree_.Query(query);

// #ifdef QUERY_VALIDATION
//     kdtree_.Validation(nodes_);

//     if (neighbor.size() == 0) {
//         return neighbor;
//     }
//     fprintf(stderr, "neighbor:\n");
//     sort(neighbor.begin(), neighbor.end());
//     for (auto &&i : neighbor) {
//         fprintf(stderr, "%d ", i);
//     }
//     fprintf(stderr, "\n");

//     fprintf(stderr, "exact:\n");
//     vector<int> exact;
//     float dist = 0;
//     for (auto &&n : nodes_) {
//         if (id == n.id) {
//             continue;
//         }

//         dist = sqrt(pow(n.pos[0] - pos[0], 2) + pow(n.pos[1] - pos[1], 2));
//         if (dist <= r) {
//             exact.push_back(n.id);
//         }
//     }

//     sort(exact.begin(), exact.end());
//     for (auto &&i : exact) {
//         fprintf(stderr, "%d ", i);
//     }
//     fprintf(stderr, "\n");
//     bool is_same = false;
//     if (exact.size()) {
//         for (auto &&exa : exact) {
//             is_same = false;
//             for (auto &&nei : neighbor) {
//                 if (nei == exa) {
//                     is_same = true;
//                     break;
//                 }
//             }

//             if (!is_same) {
//                 fprintf(stderr, "miss:\n");
//                 kdtree_.PrintBack(exa);
//             }
//         }
//     }

//     assert(exact.size() == neighbor.size());
// #endif
//     return neighbor;
// } // namespace neighbor_search

// vector<int> KdTree::GetNeighbor(const Node &node) {
//     vector<int> neighbor;
//     array<float, 2> pos{node.pos[0], node.pos[1]};

//     Node query(node.id, pos);

//     neighbor = kdtree_.Query(query, node.radius);

//     return neighbor;
// }

void KdTree::SendDeltaHQ(const vector<int> &neighbor,
                         // const Value &json,
                         int id,
                         string &key) {

#ifdef MEASURE
    // chrono::high_resolution_clock::time_point begin =
    //     chrono::high_resolution_clock::now();
#endif

    // {"init":{"neighbors":{"center":{"id":0,"x":3.24,"y":47.32},"neighbor":[6]}}}
    // {"update":{"neighbors":{"center":{"id":0,"x":87.1,"y":9.9},"neighbor":[4]}}}
    float x, y;
    char *buffer;

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
            if (9000 < str_number) {
                fprintf(stderr, "ERROR too many neighbors\n");
                return;
            }
        }
        str_number += sprintf(&buffer[str_number - 1], "]}}}");
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
            if (9000 < str_number) {
                fprintf(stderr, "ERROR too many neighbors\n");
                return;
            }
        }
        str_number += sprintf(&buffer[str_number - 1], "]}}}");
    }

#ifndef MEASURE
    // printf("%s\n", buffer.GetString());
    if (!is_socket_) {
        printf("%s\n", buffer);
        std::flush(std::cout);
    }
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
        dgram_.SendTo(buffer, str_number, 0);
    }
    // valueAllocator_.Clear();
    // parseAllocator_.Clear();
}

void KdTree::SendDeltaHQ(void) {
    string finish = R"({"finish":"finish"})";
    if (is_socket_) {
        dgram_.SendTo(finish.c_str(), finish.size(), 0);
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
