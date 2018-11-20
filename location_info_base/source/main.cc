#include <iostream>
#include <chrono>
#include <algorithm>

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/error/en.h>

#include <using.hh>
#include <neighbor_search.hh>
#include <lsh/lsh.hh>
#include <lsh/lsh_index.hh>
#include <kdtree/kdtree.hh>
#include <linear/linear.hh>

#define FLOAT(x) static_cast<float>(x)

using namespace neighbor_search;

void usage(void) {
    fprintf(stderr, "arguments $1 $2\n");
    fprintf(stderr, "$1: Set neighbor search algrothm ! \n");
    fprintf(stderr, "\th: LSH\n");
    fprintf(stderr, "\tt: kd-tree\n");
    fprintf(stderr, "\tl: linear\n");
    fprintf(stderr, "$2: Set single or multi line json\n");
    fprintf(stderr, "\ts: single line\n");
    fprintf(stderr, "\tm: multi line\n");
}

int main(int argc, char const *argv[]) {
    cin.tie(0);
    ios::sync_with_stdio(false);

    int d = 2;
    int k = 3;
    int L = 64;

    string read;

    NeighborSearch *ns;

    if (argc < 3 || 3 < argc) {
        usage();
        exit(1);
    }

    // TODO 引数で近傍探索のアルゴリズムを切り替える
    switch (argv[1][0]) {
        case 'h':
            ns = new LSH(d, k, L);
            break;
        case 't':
            ns = new KdTree();
            break;
        case 'l':
            ns = new Linear();
            break;
        default:
            usage();
            exit(1);
    }

    bool is_multiline = false;
    switch (argv[2][0]) {
        case 's':
            break;
        case 'm':
            is_multiline = true;
            break;
        default:
            usage();
            exit(1);
    }

    string init =
        R"({"init":{"node":[{"name":"node0","type":"regular","connection":"infrastructure","adapter":"orinoco","x":1.08,"y":2.46,"z":2.11,"Pt":20,"internal_delay":1,"radius":5},{"name":"node1","type":"regular","connection":"infrastructure","adapter":"orinoco","x":5.89,"y":2.99,"z":8.17,"Pt":20,"internal_delay":1,"radius":5},{"name":"node2","type":"regular","connection":"infrastructure","adapter":"orinoco","x":1.73,"y":2.91,"z":8.11,"Pt":20,"internal_delay":1,"radius":5},{"name":"node3","type":"regular","connection":"infrastructure","adapter":"orinoco","x":4.36,"y":9.4,"z":5.31,"Pt":20,"internal_delay":1,"radius":5},{"name":"node4","type":"regular","connection":"infrastructure","adapter":"orinoco","x":7.69,"y":1.37,"z":7.58,"Pt":20,"internal_delay":1,"radius":5},{"name":"node5","type":"regular","connection":"infrastructure","adapter":"orinoco","x":0.87,"y":2.51,"z":4.37,"Pt":20,"internal_delay":1,"radius":5},{"name":"node6","type":"regular","connection":"infrastructure","adapter":"orinoco","x":1.88,"y":6.48,"z":6.29,"Pt":20,"internal_delay":1,"radius":5},{"name":"node7","type":"regular","connection":"infrastructure","adapter":"orinoco","x":8.77,"y":7.68,"z":9.99,"Pt":20,"internal_delay":1,"radius":5},{"name":"node8","type":"regular","connection":"infrastructure","adapter":"orinoco","x":5.76,"y":4.1,"z":9.52,"Pt":20,"internal_delay":1,"radius":5},{"name":"node9","type":"regular","connection":"infrastructure","adapter":"orinoco","x":6.28,"y":4.43,"z":4.27,"Pt":20,"internal_delay":1,"radius":5}],"environment":[{"name":"env_outdoor","is_dynamic":false,"alpha":3.2,"sigma":0.0,"W":0.0,"noise_power":-100.0}],"connection":[{"from_node":"node0","to_node":"auto_connect","through_environment":"env_outdoor","standard":"802.11b","packet_size":1024,"consider_interference":false},{"from_node":"node1","to_node":"auto_connect","through_environment":"env_outdoor","standard":"802.11b","packet_size":1024,"consider_interference":false},{"from_node":"node2","to_node":"auto_connect","through_environment":"env_outdoor","standard":"802.11b","packet_size":1024,"consider_interference":false},{"from_node":"node3","to_node":"auto_connect","through_environment":"env_outdoor","standard":"802.11b","packet_size":1024,"consider_interference":false},{"from_node":"node4","to_node":"auto_connect","through_environment":"env_outdoor","standard":"802.11b","packet_size":1024,"consider_interference":false},{"from_node":"node5","to_node":"auto_connect","through_environment":"env_outdoor","standard":"802.11b","packet_size":1024,"consider_interference":false},{"from_node":"node6","to_node":"auto_connect","through_environment":"env_outdoor","standard":"802.11b","packet_size":1024,"consider_interference":false},{"from_node":"node7","to_node":"auto_connect","through_environment":"env_outdoor","standard":"802.11b","packet_size":1024,"consider_interference":false},{"from_node":"node8","to_node":"auto_connect","through_environment":"env_outdoor","standard":"802.11b","packet_size":1024,"consider_interference":false},{"from_node":"node9","to_node":"auto_connect","through_environment":"env_outdoor","standard":"802.11b","packet_size":1024,"consider_interference":false}]}})";

    vector<Node> init_nodes;

    int id = 0;
    while (1) {

        ///////////////////////////////////////
        // Read stream
        ///////////////////////////////////////
        getline(cin, read);
        // printf("%s\n", read.c_str());

        Json json;
        json.Parse(read.c_str());
        // json.Parse(init.c_str());

        ///////////////////////////////////////
        // Check parse error
        ///////////////////////////////////////
        bool error = json.HasParseError();
        if (error) {
            size_t offset       = json.GetErrorOffset();
            ParseErrorCode code = json.GetParseError();
            const char *msg     = GetParseError_En(code);

            printf("%lu:%d(%s)\n", offset, code, msg);
            exit(1);
        }

        const Value &root = json;
        for (Value::ConstMemberIterator iter = root.MemberBegin();
             iter != root.MemberEnd();
             ++iter) {
            string key         = iter->name.GetString();
            const Value &value = iter->value;

            if (key == "init") {
                // printf("%s", read.c_str());
                // std::flush(std::cout);
                // std::cout << key << std::endl;

                // #ifdef MULTILINE_JSON
                bool is_finish = false;
                if (value.IsString()) {
                    string str = value.GetString();
                    if (str.compare("finish") == 0) {
                        is_finish = true;
                    }
                }

                if (is_multiline) {

                    if (is_finish) {
                        ns->Init(init_nodes);

                        for (auto &&n : init_nodes) {
                            vector<int> neighbor;
                            neighbor = ns->GetNeighbor(n);

                            if (0 < neighbor.size()) {
                                ns->SendDeltaHQ(neighbor, n, key);
                            }
                        }
                        continue;
                    }

                    if (value.HasMember("node")) {

                        const Value &node = value["node"];

                        // node = init["node"].GetObject();
                        double x, y;
                        int radius;
                        x      = node["x"].GetDouble();
                        y      = node["y"].GetDouble();
                        radius = node["radius"].GetInt();
                        Node n{
                            id, array<float, 2>{FLOAT(x), FLOAT(y)}, radius};
                        init_nodes.push_back(n);
                        id++;
                    }
                    // #else

                } else {
                    if (is_finish) {
                        continue;
                    }

                    ns->Init(value);
                    for (auto &&n : value["node"].GetArray()) {

                        Value node(n, json.GetAllocator());
                        node.AddMember("id", id, json.GetAllocator());

                        vector<int> neighbor;
                        neighbor = ns->GetNeighbor(node);

                        sort(neighbor.begin(), neighbor.end());

                        if (0 < neighbor.size()) {
                            ns->SendDeltaHQ(neighbor, node, key);
                        } else {
                            neighbor.push_back(-1);
                            ns->SendDeltaHQ(neighbor, node, key);
                        }
                        id++;
                    }
                    // goto FINISH_HANDLER;
                    // #endif
                }
            }
            if (key == "update") {
                // std::cout << key << std::endl;
                if (!value.HasMember("node")) {
                    continue;
                }
                chrono::high_resolution_clock::time_point begin =
                    chrono::high_resolution_clock::now();

                ns->Update(value["node"]);

                chrono::high_resolution_clock::time_point end =
                    chrono::high_resolution_clock::now();

                chrono::microseconds update_elapsed =
                    chrono::duration_cast<chrono::microseconds>(end - begin);

                // fprintf(stderr, "update elapsed: %lld\n", elapsed.count());

                vector<int> neighbor;

                begin = chrono::high_resolution_clock::now();

                neighbor = ns->GetNeighbor(value["node"]);

                end = chrono::high_resolution_clock::now();

                chrono::microseconds search_elapsed =
                    chrono::duration_cast<chrono::microseconds>(end - begin);

                // fprintf(stderr,
                //         "elapsed=%lld\tupdate=%lld\tsearch=%lld\n",
                //         update_elapsed.count() + search_elapsed.count(),
                //         update_elapsed.count(),
                //         search_elapsed.count());

                if (neighbor.size() == 0) {
                    neighbor.push_back(-1);
                }
                // begin = chrono::high_resolution_clock::now();
                ns->SendDeltaHQ(neighbor, value["node"], key);
                // TODO send json
                // end     = chrono::high_resolution_clock::now();
                // elapsed = chrono::duration_cast<chrono::microseconds>(
                // end - begin);

                // fprintf(stderr, "send elapsed: %lld\n",
                // elapsed.count());
                // }
            }
            if (key == "finish") {
                // std::cout << key << std::endl;
                printf("{\"finish\":\"finish\"}\n");
                std::flush(std::cout);

                goto FINISH_HANDLER;
            }
        }
    }

FINISH_HANDLER:
    delete ns;

    return 0;
}
