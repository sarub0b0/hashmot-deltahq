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

    int node_number   = 0;
    const Value &root = init_json;
    for (Value::ConstMemberIterator iter = root.MemberBegin();
         iter != root.MemberEnd();
         ++iter) {
        string key         = iter->name.GetString();
        const Value &value = iter->value;

        ns->Init(value);
        for (auto &&n : value["node"].GetArray()) {

            Value node(n, init_json.GetAllocator());
            node.AddMember("id", node_number, init_json.GetAllocator());

            vector<int> neighbor;
            neighbor = ns->GetNeighbor(node_number);

            sort(neighbor.begin(), neighbor.end());

            if (0 < neighbor.size()) {
                ns->SendDeltaHQ(neighbor, node_number, key);
            } else {
                neighbor.push_back(-1);
                ns->SendDeltaHQ(neighbor, node_number, key);
            }
            node_number++;
        }
    }
    fprintf(stderr, "\n-- Update wait --\n");

#ifdef MEASURE
    chrono::high_resolution_clock::time_point begin, end;
    chrono::high_resolution_clock::time_point parse_begin, parse_end;
    chrono::high_resolution_clock::time_point update_begin, update_end;
    chrono::high_resolution_clock::time_point search_begin, search_end;
    chrono::high_resolution_clock::time_point send_begin, send_end;
    chrono::nanoseconds measure_elapsed;
    chrono::nanoseconds parse_elapsed;
    chrono::nanoseconds update_elapsed;
    chrono::nanoseconds search_elapsed;
    chrono::nanoseconds send_elapsed;

    measure_elapsed =
        chrono::duration_cast<chrono::nanoseconds>(begin - begin);
    update_elapsed =
        chrono::duration_cast<chrono::nanoseconds>(begin - begin);
    parse_elapsed = chrono::duration_cast<chrono::nanoseconds>(begin - begin);
    search_elapsed =
        chrono::duration_cast<chrono::nanoseconds>(begin - begin);
    send_elapsed = chrono::duration_cast<chrono::nanoseconds>(begin - begin);

#endif
    // #ifdef TCHK_ELAPSED
    //     chrono::high_resolution_clock::time_point begin, end;
    //     std::chrono::nanoseconds parse_elapsed;
    // #endif
    bool loop_start = false;
    vector<int> neighbor;
    // string read;
    char read[9000];

    Json json;
    int update_id;
    while (1) {

        ///////////////////////////////////////
        // Read stream
        ///////////////////////////////////////
        // getline(cin, read);
        fgets(read, 9000, stdin);
#ifdef MEASURE
        if (loop_start == false) {
            begin      = chrono::high_resolution_clock::now();
            loop_start = true;
        }
#endif

        // #ifdef TCHK_ELAPSED
        //         begin = chrono::high_resolution_clock::now();

        // #endif
#ifdef MEASURE
        parse_begin = chrono::high_resolution_clock::now();
#endif
        json.Parse(read);
        // json.Parse(read.c_str());
        // json.Parse(init.c_str());

#ifdef MEASURE
        parse_end = chrono::high_resolution_clock::now();
        parse_elapsed += chrono::duration_cast<chrono::nanoseconds>(
            parse_end - parse_begin);
#endif
        // #ifdef TCHK_ELAPSED
        //         end = chrono::high_resolution_clock::now();
        //         parse_elapsed =
        //             std::chrono::duration_cast<std::chrono::nanoseconds>(end
        //             - begin);
        //         printf("parse:%lld.%09lld\n",
        //                parse_elapsed.count() / 1000000000,
        //                parse_elapsed.count() % 1000000000);
        // #endif
        ///////////////////////////////////////
        // Check parse error
        ///////////////////////////////////////
        // bool error = json.HasParseError();
        // if (error) {
        //     size_t offset       = json.GetErrorOffset();
        //     ParseErrorCode code = json.GetParseError();
        //     const char *msg     = GetParseError_En(code);

        //     printf("%lu:%d(%s)\n", offset, code, msg);
        //     exit(1);
        // }

        const Value &root = json;
        for (Value::ConstMemberIterator iter = root.MemberBegin();
             iter != root.MemberEnd();
             ++iter) {
            string key         = iter->name.GetString();
            const Value &value = iter->value;

            if (key == "update") {
                // std::cout << key << std::endl;
                // if (!value.HasMember("node")) {
                //     continue;
                // }
                // #ifdef TCHK_ELAPSED
                //                 chrono::high_resolution_clock::time_point
                //                 begin =
                //                     chrono::high_resolution_clock::now();
                // #endif

#ifdef MEASURE
                update_begin = chrono::high_resolution_clock::now();
#endif

                update_id = ns->Update(value["node"]);

#ifdef MEASURE
                update_end = chrono::high_resolution_clock::now();
                update_elapsed += chrono::duration_cast<chrono::nanoseconds>(
                    update_end - update_begin);
#endif
                // #ifdef TCHK_ELAPSED
                //                 chrono::high_resolution_clock::time_point
                //                 end =
                //                     chrono::high_resolution_clock::now();

                //                 chrono::nanoseconds update_elapsed =
                //                     chrono::duration_cast<chrono::nanoseconds>(end
                //                     - begin);

                //                 // fprintf(stderr, "update elapsed:
                //                 %lld\n",
                //                 // elapsed.count());
                // #endif

                // #ifdef TCHK_ELAPSED
                //                 begin =
                //                 chrono::high_resolution_clock::now();
                // #endif

#ifdef MEASURE
                search_begin = chrono::high_resolution_clock::now();
#endif

                neighbor.clear();
                neighbor = ns->GetNeighbor(update_id);

#ifdef MEASURE
                search_end = chrono::high_resolution_clock::now();
                search_elapsed += chrono::duration_cast<chrono::nanoseconds>(
                    search_end - search_begin);
#endif
                // #ifdef TCHK_ELAPSED
                //                 end = chrono::high_resolution_clock::now();

                //                 chrono::nanoseconds search_elapsed =
                //                     chrono::duration_cast<chrono::nanoseconds>(end
                //                     - begin);

                //                 fprintf(stderr,
                //                         "elapsed=%lld.%09lld\n"
                //                         "\tupdate=%lld.%09lld\n"
                //                         "\tsearch=%lld.%09lld\n",
                //                         (update_elapsed.count() +
                //                         search_elapsed.count()) /
                //                             1000000000,
                //                         (update_elapsed.count() +
                //                         search_elapsed.count()) %
                //                             1000000000,
                //                         update_elapsed.count() /
                //                         1000000000, update_elapsed.count()
                //                         % 1000000000,
                //                         search_elapsed.count() /
                //                         1000000000, search_elapsed.count()
                //                         % 1000000000);
                // #endif

                if (neighbor.size() == 0) {
                    neighbor.push_back(-1);
                }

                // sort(neighbor.begin(), neighbor.end());

                // begin = chrono::high_resolution_clock::now();

#ifdef MEASURE
                send_begin = chrono::high_resolution_clock::now();
#endif
                ns->SendDeltaHQ(neighbor, update_id, key);

#ifdef MEASURE
                send_end = chrono::high_resolution_clock::now();
                send_elapsed += chrono::duration_cast<chrono::nanoseconds>(
                    send_end - send_begin);
#endif
                // end     = chrono::high_resolution_clock::now();
                // elapsed = chrono::duration_cast<chrono::microseconds>(
                // end - begin);

                // fprintf(stderr, "send elapsed: %lld\n",
                // elapsed.count());
                // }
            }
            if (key == "finish") {
#ifdef MEASURE
                send_begin = chrono::high_resolution_clock::now();
#endif
                ns->SendDeltaHQ();
#ifdef MEASURE
                send_end = chrono::high_resolution_clock::now();
                send_elapsed += chrono::duration_cast<chrono::nanoseconds>(
                    send_end - send_begin);
#endif

#ifdef MEASURE
                end = chrono::high_resolution_clock::now();
                measure_elapsed =
                    chrono::duration_cast<chrono::nanoseconds>(end - begin);

                chrono::nanoseconds avg;

                avg = measure_elapsed / node_number;
                printf("all:%lld.%09lld avg:%lld.%09lld\n",
                       measure_elapsed.count() / 1000000000,
                       measure_elapsed.count() % 1000000000,
                       avg.count() / 1000000000,
                       avg.count() % 1000000000);
                avg = parse_elapsed / node_number;
                printf("\tparse: %lld.%09lld\n\t\tavg:%lld.%09lld\n",
                       parse_elapsed.count() / 1000000000,
                       parse_elapsed.count() % 1000000000,
                       avg.count() / 1000000000,
                       avg.count() % 1000000000);
                avg = update_elapsed / node_number;
                printf("\tupdate:%lld.%09lld\n\t\tavg:%lld.%09lld\n",
                       update_elapsed.count() / 1000000000,
                       update_elapsed.count() % 1000000000,
                       avg.count() / 1000000000,
                       avg.count() % 1000000000);
                avg = search_elapsed / node_number;
                printf("\tsearch:%lld.%09lld\n\t\tavg:%lld.%09lld\n",
                       search_elapsed.count() / 1000000000,
                       search_elapsed.count() % 1000000000,
                       avg.count() / 1000000000,
                       avg.count() % 1000000000);
                avg = send_elapsed / node_number;
                printf("\tsend:  %lld.%09lld\n\t\tavg:%lld.%09lld\n",
                       send_elapsed.count() / 1000000000,
                       send_elapsed.count() % 1000000000,
                       avg.count() / 1000000000,
                       avg.count() % 1000000000);
#endif

                goto FINISH_HANDLER;
            }
        }
    }

FINISH_HANDLER:
    delete ns;

    return 0;
}
