#include <iostream>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <rapidjson/document.h>
// #include <rapidjson/pointer.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/encodedstream.h>

#include <using.hh>
#include <neighbor_search.hh>
#include <lsh/lsh.hh>
#include <lsh/lsh_index.hh>
#include <kdtree/kdtree.hh>
#include <linear/linear.hh>

#define FLOAT(x) static_cast<float>(x)

using namespace neighbor_search;

void usage(void) {
    // fprintf(stderr, "arguments $1 $2\n");
    fprintf(stderr, "arguments $1 $2 $3 $4\n");
    fprintf(stderr, "$1: Set init file ! \n");
    fprintf(stderr, "$2: Set neighbor search algrothm ! \n");
    fprintf(stderr, "\th: LSH\n");
    fprintf(stderr, "\tt: kd-tree\n");
    fprintf(stderr, "\tl: linear\n");
    fprintf(stderr, "$3: Set ip address\n");
    fprintf(stderr, "$4: Set port\n");
    // fprintf(stderr, "$2: Set single or multi line json\n");
    // fprintf(stderr, "\ts: single line\n");
    // fprintf(stderr, "\tm: multi line\n");
}

int main(int argc, char const *argv[]) {
    cin.tie(0);
    ios::sync_with_stdio(false);

    int d = 2;
    int k = 3;
    int L = 64;

    string read;

    string init_file;

    NeighborSearch *ns;

    if (argc <= 2) {
        usage();
        exit(1);
    }

    init_file = argv[1];

    switch (argv[2][0]) {
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

    string host, port;
    if (5 == argc) {
        host = argv[3];
        port = argv[4];
        ns->InitDGram(host, port);
        printf("host=%s port=%s\n", host.c_str(), port.c_str());
    }

    // bool is_multiline = false;
    // switch (argv[1][0]) {
    //     case 's':
    //         break;
    //     case 'm':
    //         is_multiline = true;
    //         break;
    //     default:
    //         usage();
    //         exit(1);
    // }

    vector<Node> init_nodes;

    ifstream ifs(init_file);
    IStreamWrapper isw(ifs);

    Json init_json;
    init_json.ParseStream(isw);

    bool error = init_json.HasParseError();
    if (error) {
        size_t offset       = init_json.GetErrorOffset();
        ParseErrorCode code = init_json.GetParseError();
        const char *msg     = GetParseError_En(code);

        printf("%lu:%d(%s)\n", offset, code, msg);
        exit(1);
    }

    const Value &root = init_json;
    for (Value::ConstMemberIterator iter = root.MemberBegin();
         iter != root.MemberEnd();
         ++iter) {
        string key         = iter->name.GetString();
        const Value &value = iter->value;

        ns->Init(value);
        int id = 0;
        for (auto &&n : value["node"].GetArray()) {

            Value node(n, init_json.GetAllocator());
            node.AddMember("id", id, init_json.GetAllocator());

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
    }

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

                // fprintf(stderr, "update elapsed: %lld\n",
                // elapsed.count());

                vector<int> neighbor;

                begin = chrono::high_resolution_clock::now();

                neighbor = ns->GetNeighbor(value["node"]);

                end = chrono::high_resolution_clock::now();

                chrono::microseconds search_elapsed =
                    chrono::duration_cast<chrono::microseconds>(end - begin);

                // fprintf(stderr,
                //         "elapsed=%lld\tupdate=%lld\tsearch=%lld\n",
                //         update_elapsed.count() +
                //         search_elapsed.count(), update_elapsed.count(),
                //         search_elapsed.count());

                if (neighbor.size() == 0) {
                    neighbor.push_back(-1);
                }
                sort(neighbor.begin(), neighbor.end());
                // begin = chrono::high_resolution_clock::now();
                ns->SendDeltaHQ(neighbor, value["node"], key);
                // end     = chrono::high_resolution_clock::now();
                // elapsed = chrono::duration_cast<chrono::microseconds>(
                // end - begin);

                // fprintf(stderr, "send elapsed: %lld\n",
                // elapsed.count());
                // }
            }
            if (key == "finish") {
                ns->SendDeltaHQ();

                goto FINISH_HANDLER;
            }
        }
    }

FINISH_HANDLER:
    delete ns;

    return 0;
}
