#include <iostream>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <cfloat>

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

typedef GenericDocument<UTF8<>, MemoryPoolAllocator<>, MemoryPoolAllocator<>>
    DocumentType;

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
    int L = 32;

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

    float density = 0;

    int node_number   = 0;
    const Value &root = init_json;
    for (Value::ConstMemberIterator iter = root.MemberBegin();
         iter != root.MemberEnd();
         ++iter) {
        string key         = iter->name.GetString();
        const Value &value = iter->value;

        chrono::high_resolution_clock::time_point begin, end;
        chrono::nanoseconds elapsed;

        begin = chrono::high_resolution_clock::now();
        ns->Init(value);
        end = chrono::high_resolution_clock::now();

        elapsed = chrono::duration_cast<chrono::nanoseconds>(end - begin);

        fprintf(stderr,
                "Init elapsed %lld.%09lld\n",
                elapsed.count() / 1000000000,
                elapsed.count() % 1000000000);

        float min_x, min_y, max_x, max_y;
        min_x = FLT_MAX;
        min_y = FLT_MAX;
        max_x = 0;
        max_y = 0;
        int x, y;
        int node_size = 0;
        for (auto &&node : value["node"].GetArray()) {
            x = FLOAT(node["x"].GetDouble());
            y = FLOAT(node["y"].GetDouble());
            if (x < min_x) {
                min_x = x;
            }
            if (max_x < x) {
                max_x = x;
            }
            if (y < min_y) {
                min_y = y;
            }
            if (max_y < y) {
                max_y = y;
            }
            node_size++;
        }

        density = node_size / ((max_x - min_x) * (max_y - min_y) * 0.000001);
        fprintf(stderr,
                "-- Init density(%.2f) top-left(%.2f, %.2f), "
                "bottom-right(%.2f, "
                "%.2f)\n",
                density,
                min_x,
                min_y,
                max_x,
                max_y);

        for (auto &&n : value["node"].GetArray()) {
            Value node(n, init_json.GetAllocator());
            node.AddMember("id", node_number, init_json.GetAllocator());

            const vector<int> &neighbor = ns->GetNeighbor(node_number);
            // sort(neighbor.begin(), neighbor.end());

            ns->SendDeltaHQ(neighbor, node_number, key);
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
    // vector<int> neighbor;
    // string read;
    char read[9000];

    char valueBuffer[20000];
    char parseBuffer[9000];

    MemoryPoolAllocator<> valueAllocator(valueBuffer, sizeof(valueBuffer));
    MemoryPoolAllocator<> parseAllocator(parseBuffer, sizeof(parseBuffer));
    DocumentType json(&valueAllocator, sizeof(parseBuffer), &parseAllocator);

    int neighbor_count = 0;
    int loop_count     = 0;
    int update_id;
    while (1) {

        ///////////////////////////////////////
        // Read stream
        ///////////////////////////////////////
        // getline(cin, read);
        while (fgets(read, 9000, stdin) == NULL) {
        }
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
        // Json json;
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

        // const Value &root = json;
        for (Value::ConstMemberIterator iter = json.MemberBegin();
             iter != json.MemberEnd();
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

                // neighbor.shrink_to_fit();
                const vector<int> &neighbor = ns->GetNeighbor(update_id);

#ifdef MEASURE
                search_end = chrono::high_resolution_clock::now();
                search_elapsed += chrono::duration_cast<chrono::nanoseconds>(
                    search_end - search_begin);
#endif

                if (neighbor[0] != -1) {
                    neighbor_count += neighbor.size();
                }
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
                valueAllocator.Clear();
                parseAllocator.Clear();
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

                chrono::nanoseconds avg, measure_avg, parse_avg, update_avg,
                    search_avg, send_avg;

                measure_avg = measure_elapsed / loop_count;
                parse_avg   = parse_elapsed / loop_count;
                update_avg  = update_elapsed / loop_count;
                search_avg  = search_elapsed / loop_count;
                send_avg    = send_elapsed / loop_count;

                fprintf(stderr,
                        "node_number neighbor_avg density update_count "
                        "all_elapsed parse_elapsed update_elapsed "
                        "search_elapsed send_elapsed all_avg_elapsed "
                        "parse_avg_elapsed update_avg_elapsed "
                        "search_avg_elapsed send_avg_elapsed\n");

                fprintf(stdout,
                        "%d %d %.2f %d %lld.%09lld %lld.%09lld %lld.%09lld "
                        "%lld.%09lld %lld.%09lld %lld.%09lld %lld.%09lld "
                        "%lld.%09lld %lld.%09lld %lld.%09lld \n",
                        node_number,
                        loop_count,
                        density,
                        neighbor_count / loop_count,

                        measure_elapsed.count() / 1000000000,
                        measure_elapsed.count() % 1000000000,

                        parse_elapsed.count() / 1000000000,
                        parse_elapsed.count() % 1000000000,

                        update_elapsed.count() / 1000000000,
                        update_elapsed.count() % 1000000000,

                        search_elapsed.count() / 1000000000,
                        search_elapsed.count() % 1000000000,

                        send_elapsed.count() / 1000000000,
                        send_elapsed.count() % 1000000000,

                        measure_avg.count() / 1000000000,
                        measure_avg.count() % 1000000000,

                        parse_avg.count() / 1000000000,
                        parse_avg.count() % 1000000000,

                        update_avg.count() / 1000000000,
                        update_avg.count() % 1000000000,

                        search_avg.count() / 1000000000,
                        search_avg.count() % 1000000000,

                        send_avg.count() / 1000000000,
                        send_avg.count() % 1000000000

                );

                fprintf(stderr,
                        "neighbor avg: %d\n",
                        neighbor_count / loop_count);

                avg = measure_elapsed / loop_count;
                fprintf(stderr,
                        "all:%lld.%09lld avg:%lld.%09lld\n",
                        measure_elapsed.count() / 1000000000,
                        measure_elapsed.count() % 1000000000,
                        avg.count() / 1000000000,
                        avg.count() % 1000000000);
                avg = parse_elapsed / loop_count;
                fprintf(stderr,
                        "\tparse: %lld.%09lld\n\t\tavg:%lld.%09lld\n",
                        parse_elapsed.count() / 1000000000,
                        parse_elapsed.count() % 1000000000,
                        avg.count() / 1000000000,
                        avg.count() % 1000000000);
                avg = update_elapsed / loop_count;
                fprintf(stderr,
                        "\tupdate:%lld.%09lld\n\t\tavg:%lld.%09lld\n",
                        update_elapsed.count() / 1000000000,
                        update_elapsed.count() % 1000000000,
                        avg.count() / 1000000000,
                        avg.count() % 1000000000);
                avg = search_elapsed / loop_count;
                fprintf(stderr,
                        "\tsearch:%lld.%09lld\n\t\tavg:%lld.%09lld\n",
                        search_elapsed.count() / 1000000000,
                        search_elapsed.count() % 1000000000,
                        avg.count() / 1000000000,
                        avg.count() % 1000000000);
                avg = send_elapsed / loop_count;
                fprintf(stderr,
                        "\tsend:  %lld.%09lld\n\t\tavg:%lld.%09lld\n",
                        send_elapsed.count() / 1000000000,
                        send_elapsed.count() % 1000000000,
                        avg.count() / 1000000000,
                        avg.count() % 1000000000);
#endif

                valueAllocator.Clear();
                parseAllocator.Clear();
                goto FINISH_HANDLER;
            }
        }
        ++loop_count;
    }

FINISH_HANDLER:
    delete ns;

    return 0;
}
