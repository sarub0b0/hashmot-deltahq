#include <iostream>
#include <chrono>

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
                        }
                        id++;
                    }
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

                fprintf(stderr,
                        "elapsed=%lld\tupdate=%lld\tsearch=%lld\n",
                        update_elapsed.count() + search_elapsed.count(),
                        update_elapsed.count(),
                        search_elapsed.count());

                if (0 < neighbor.size()) {
                    // begin = chrono::high_resolution_clock::now();
                    ns->SendDeltaHQ(neighbor, value["node"], key);
                    // TODO send json
                    // end     = chrono::high_resolution_clock::now();
                    // elapsed = chrono::duration_cast<chrono::microseconds>(
                    // end - begin);

                    // fprintf(stderr, "send elapsed: %lld\n",
                    // elapsed.count());
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
