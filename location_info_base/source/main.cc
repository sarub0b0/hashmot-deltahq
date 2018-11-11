#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/error/en.h>

#include <using.hh>
#include <neighbor_search.hh>
#include <lsh/lsh.hh>
#include <lsh/lsh_index.hh>
#include <kdtree/kdtree.hh>

#define FLOAT(x) static_cast<float>(x)

using namespace neighbor_search;

void usage(void) {
    fprintf(stderr, "arg: Set neighbor search algrothm ! \n");
    fprintf(stderr, "\tl: LSH\n");
    fprintf(stderr, "\tk: kd-tree\n");
}

int main(int argc, char const *argv[]) {
    cin.tie(0);
    ios::sync_with_stdio(false);

    int d = 2;
    int k = 3;
    int L = 64;

    string read;

    NeighborSearch *ns;

    if (argc < 2 || 2 < argc) {
        usage();
        exit(1);
    }

    // TODO 引数で近傍探索のアルゴリズムを切り替える
    switch (argv[1][0]) {
        case 'l':
            ns = new LSH(d, k, L);
            break;
        case 'k':
            ns = new KdTree();
            break;
        default:
            usage();
            exit(1);
    }

    vector<NodeParam> init_nodes;

    int id = 0;
    while (1) {

        ///////////////////////////////////////
        // Read stream
        ///////////////////////////////////////
        getline(cin, read);
        // printf("%s\n", read.c_str());

        Json json;
        json.Parse(read.c_str());

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
                printf("%s\n", read.c_str());
                std::flush(std::cout);
                // std::cout << key << std::endl;

                bool is_finish = false;
                if (value.IsString()) {
                    string str = value.GetString();
                    if (str.compare("finish") == 0) {
                        is_finish = true;
                    }
                }

                if (is_finish) {
                    ns->Init(init_nodes);

                    for (auto &&n : init_nodes) {
                        vector<int> neighbor;
                        neighbor = ns->GetNeighbor(n);

                        if (0 < neighbor.size()) {
                            ns->SendDeltaHQ(neighbor, n, key);
                        }
                    }
                    break;
                }

                if (value.HasMember("node")) {

                    const Value &node = value["node"];

                    // node = init["node"].GetObject();
                    double x, y;
                    int radius;
                    x      = node["x"].GetDouble();
                    y      = node["y"].GetDouble();
                    radius = node["radius"].GetInt();
                    NodeParam n{id, FLOAT(x), FLOAT(y), radius};
                    init_nodes.push_back(n);
                    id++;
                }

                // for (auto &&n : value["node"].GetArray()) {

                // }

                // for (auto &&n : value["node"].GetArray()) {

                //     Value node(n, json.GetAllocator());
                //     node.AddMember("id", id, json.GetAllocator());

                //     vector<int> neighbor;
                //     neighbor = ns->GetNeighbor(node);

                //     if (0 < neighbor.size()) {
                //         ns->SendDeltaHQ(neighbor, node, key);
                //     }
                //     id++;
                // }
            }
            if (key == "update") {
                // std::cout << key << std::endl;
                ns->Update(value);

                vector<int> neighbor;

                neighbor = ns->GetNeighbor(value);
                if (0 < neighbor.size()) {
                    ns->SendDeltaHQ(neighbor, value, key);
                    // TODO send json
                }
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
