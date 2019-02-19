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


#include <time.h>

#include <iostream>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <cfloat>
#include <thread>

#include <rapidjson/document.h>
// #include <rapidjson/pointer.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/encodedstream.h>

#include <getopt_long.hh>

#include <using.hh>
#include <neighbor_search.hh>
// #include <lsh/lsh.hh>
// #include <lsh/lsh_index.hh>
#include <kdtree/kdtree.hh>
// #include <linear/linear.hh>

#define FLOAT(x) static_cast<float>(x)

using namespace neighbor_search;

typedef GenericDocument<UTF8<>, MemoryPoolAllocator<>, MemoryPoolAllocator<>>
    DocumentType;

void usage(FILE *f) {
    fprintf(f, "\nUsage: location_info_base [options] <init_file.json>\n");
    fprintf(f, "General options:\n");
    fprintf(f,
            "  -h, --help             - print this help message and exit\n");
    // fprintf(f,
    //         " -v, --version          - print version information and
    //         exit\n");
    // fprintf(f,
    //         " -l, --license          - print license information and
    //         exit\n");
    fprintf(f, "\nOutput control: (default stdout)\n");
    fprintf(f,
            "  -A --ipaddr     <xxx.xxx.xxx.xxx> - ip address for sending "
            "message\n");
    fprintf(f,
            "  -p --port       <number>          - port number for sending "
            "message\n");
    fprintf(f, "  -L --listen     <number>          - listen port\n");

    fprintf(f, "\nComputation control:\n");
    fprintf(f, "  -N --process    <number>          - number of process\n");
    fprintf(f, "  -j --job-id     <number>          - job id\n");
    fprintf(f,
            "  -a --algorithm <type>             - lsh, kdtree, linear | h, "
            "t, l\n");
    fprintf(f, "\n");
    // fprintf(stderr, "arguments $1 $2\n");
    // fprintf(stderr, "arguments $1 $2 $3 $4\n");
    // fprintf(stderr, "$1: Set init file ! \n");
    // fprintf(stderr, "$2: Set neighbor search algrothm ! \n");
    // fprintf(stderr, "\th: LSH\n");
    // fprintf(stderr, "\tt: kd-tree\n");
    // fprintf(stderr, "\tl: linear\n");
    // fprintf(stderr, "$3: Set ip address\n");
    // fprintf(stderr, "$4: Set port\n");
    // fprintf(stderr, "$2: Set single or multi line json\n");
    // fprintf(stderr, "\ts: single line\n");
    // fprintf(stderr, "\tm: multi line\n");
}

const struct option long_options[] = {
    {"help", 0, 0, 'h'},
    {"process", 1, 0, 'N'},
    {"ipaddr", 1, 0, 'A'},
    {"port", 1, 0, 'p'},
    {"listen", 1, 0, 'L'},
    {"algorithm", 1, 0, 'a'},
    {"job-id", 1, 0, 'i'},
    // {"version",   0,   0,   'v'},
    // {"license",   0,   0,   'l'},
    // {"process",   1,   0,   't'},
    {0, 0, 0, 0},
};

// structure holding name of short options;
// should match the 'long_options' structure above
const char short_options[] = "hp:a:L:N:A:j:";

int main(int argc, char *const argv[]) {
    cin.tie(0);
    ios::sync_with_stdio(false);

    //* lsh.hh define
    // int d = DIMENSION;
    // int k = K_FUNCS;
    // int L = HASHES;

    string init_file;

    NeighborSearch *ns;

    struct send_data *send_data =
        static_cast<struct send_data *>(malloc(9000));

    send_data                = static_cast<struct send_data *>(malloc(9000));
    send_data->type          = -1;
    send_data->id            = -1;
    send_data->x             = -1;
    send_data->y             = -1;
    send_data->neighbor_size = 0;
    send_data->neighbor[0]   = -1;

    DGram dg;

    if (argc == 1) {
        usage(stdout);
        exit(1);
    }

    int process_number = 1;
    string ipaddr, port, listen_port;
    string algorithm;
    int job_id = -1;

    bool is_listen = false;

    int opt;
    class getopt gopt;
    while ((opt = gopt.getopt_long(
                argc, argv, short_options, long_options, nullptr)) != -1) {
        switch (opt) {
            case 'N':
                process_number = atoi(gopt.opt_arg);
                break;
            case 'p':
                port = gopt.opt_arg;
                break;
            case 'A':
                ipaddr = gopt.opt_arg;
                break;
            case 'L':
                listen_port = gopt.opt_arg;
                break;
            case 'a':
                algorithm = gopt.opt_arg;
                break;
            case 'j':
                job_id = atoi(gopt.opt_arg);
                break;
            case 'h':
                usage(stdout);
                exit(0);
                break;
            case '?':
                printf("Try --help for more info\n");
                exit(1);
            default:
                usage(stdout);
                exit(1);
        }
    }

    // fprintf(
    //     stderr,
    //     "argc=%d\nalgorithm=%s\nprocces_number=%d\nlisten_port=%s\nipaddr=%"
    //     "s\nport=%s\n",
    //     argc,
    //     algorithm.c_str(),
    //     process_number,
    //     listen_port.c_str(),
    //     ipaddr.c_str(),
    //     port.c_str());

    // fprintf(stderr, "optind=%d\n", gopt.opt_ind);
    if (gopt.opt_ind == argc) {
        fprintf(stderr, "ERROR unset filename\n");
        exit(1);
    }

    init_file = argv[gopt.opt_ind];
    fprintf(stderr, "-- Read file %s\n", init_file.c_str());

    if (algorithm == "h" || algorithm == "lsh") {
        // ns = new LSH(d, k, L);
    } else if (algorithm == "t" || algorithm == "kdtree") {
        ns = new KdTree();
    } else if (algorithm == "l" || algorithm == "linear") {
        // ns = new Linear();
    } else {
        fprintf(stderr, "ERROR unsupport algorithm\n");
        exit(1);
    }

    // string host, port;
    if (ipaddr != "" && port != "") {
        ns->InitDGram(ipaddr, port);
        fprintf(stderr, "host=%s port=%s\n", ipaddr.c_str(), port.c_str());
    }
    if (listen_port != "") {
        if (dg.Open("AF_INET", false) == -1) {
            exit(1);
        }
        if (!dg.Bind("", listen_port)) {
            fprintf(stderr,
                    "ERROR bind %s: %s\n",
                    ipaddr.c_str(),
                    listen_port.c_str());
            exit(1);
        }
        is_listen = true;
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
    // vector<int32_t> neighbor;

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

    float field_size[2] = {0};
    float density       = 0;
    float area          = 0;

    int node_number   = 0;
    const Value &root = init_json;
    for (Value::ConstMemberIterator iter = root.MemberBegin();
         iter != root.MemberEnd();
         ++iter) {
        string key         = iter->name.GetString();
        const Value &value = iter->value;

        float min_x, min_y, max_x, max_y;
        if (key == "init") {
            string global_key = key;

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

            send_data->type = 0;

            for (auto &&v : value.GetObject()) {
                string key         = v.name.GetString();
                const Value &value = v.value;

                if (key == "node") {
                    // min_x = FLT_MAX;
                    // min_y = FLT_MAX;
                    // max_x = 0;
                    // max_y = 0;
                    // int x, y;
                    // for (auto &&node : value["node"].GetArray()) {
                    //     x = FLOAT(node["x"].GetDouble());
                    //     y = FLOAT(node["y"].GetDouble());
                    //     if (x < min_x) {
                    //         min_x = x;
                    //     }
                    //     if (max_x < x) {
                    //         max_x = x;
                    //     }
                    //     if (y < min_y) {
                    //         min_y = y;
                    //     }
                    //     if (max_y < y) {
                    //         max_y = y;
                    //     }
                    //     node_size++;
                    // }

                    // field_size[0] = max_x - min_x;
                    // field_size[1] = max_y - min_y;
                    node_number = value.Size();
                    // neighbor.reserve(node_number + 1);
                    // neighbor.resize(node_number);
                    // neighbor.clear();

#ifndef MEASURE
                    int id = 0;
                    if (job_id == 1 || job_id == -1) {
                        for (int i = 0; i < node_number; i++) {

                            // }
                            // for (auto &&n : value.GetArray()) {
                            // Value node(n, init_json.GetAllocator());
                            // node.AddMember("id", node_number,
                            // init_json.GetAllocator());

                            // neighbor.clear();
                            send_data->id            = id;
                            send_data->neighbor_size = 0;

                            ns->GetNeighbor(id, *send_data);
                            // const vector<int> &neighbor =
                            // ns->GetNeighbor(node_number);
                            // sort(neighbor.begin(), neighbor.end());

                            ns->SendDeltaHQ(*send_data);
                            ++id;

                            // std::this_thread::sleep_for(std::chrono::microseconds(5000));
                        }
                    }
#endif
                }
                if (key == "field") {
                    field_size[0] = value["width"].GetDouble();
                    field_size[1] = value["height"].GetDouble();
                    area          = value["area"].GetDouble();

                    min_x = 0;
                    min_y = 0;
                    max_x = field_size[0];
                    max_y = field_size[1];
                }
            }
            density = node_number / area;
            fprintf(stderr,
                    "-- Init density(%.2f) top-left(%.2f, %.2f), "
                    "bottom-right(%.2f, "
                    "%.2f)\n",
                    density,
                    min_x,
                    min_y,
                    max_x,
                    max_y);
        }
    }

    vector<int> assign_map;
    bool is_assign = false;
    if (1 < process_number && job_id != -1) {
        if (job_id == 0) {
            fprintf(stderr, "\n-- ERROR too small job_id\n");
            exit(1);
        }
        if (process_number < job_id) {
            fprintf(stderr, "\n-- ERROR too big job_id\n");
            exit(1);
        }
        if (node_number < process_number) {
            fprintf(stderr, "\n-- ERROR too big number of process\n");
            exit(1);
        }
        fprintf(stderr, "\n-- Set assign id --\n");
        assign_map.reserve(node_number + 1);
        assign_map.resize(node_number, 0);
        is_assign = true;

        // fprintf(stderr,
        //         "process_number=%d node_number=%d\n",
        //         process_number,
        //         node_number);

        int div = node_number / process_number;
        if (node_number % 2 == 1 && job_id % 2 == 1) {
            div += 1;
        }
        // fprintf(stderr, "assign_map: ");
        // fprintf(stderr, "div=%d\n", div);
        int assign_idx         = job_id - 1;
        assign_map[assign_idx] = 1;
        for (int i = 0; i < div; i++) {
            assign_idx += process_number;

            if (node_number < assign_idx) {
                break;
            }
            assign_map[assign_idx] = 1;
        }

        // for (auto &&a : assign_map) {
        //     fprintf(stderr, "%d ", a);
        // }
        // fprintf(stderr, "\n");
    }

    fprintf(stderr, "\n-- Update Wait --\n");

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
#ifdef MEASURE2
    struct timespec start_ts;
#endif
    // #ifdef TCHK_ELAPSED
    //     chrono::high_resolution_clock::time_point begin, end;
    //     std::chrono::nanoseconds parse_elapsed;
    // #endif
    bool loop_start = false;
    // vector<int> neighbor;
    // string read;
    char read[9000] = {0};

    char valueBuffer[20000] = {0};
    char parseBuffer[9000]  = {0};

    MemoryPoolAllocator<> valueAllocator(valueBuffer, sizeof(valueBuffer));
    MemoryPoolAllocator<> parseAllocator(parseBuffer, sizeof(parseBuffer));
    DocumentType json(&valueAllocator, sizeof(parseBuffer), &parseAllocator);

    int neighbor_count    = 0;
    int loop_count        = 0;
    int search_loop_count = 0;
    int update_id;
    while (1) {

        ///////////////////////////////////////
        // Read stream
        ///////////////////////////////////////
        // getline(cin, read);
        if (loop_start == false) {
#ifdef MEASURE
            begin      = chrono::high_resolution_clock::now();
            loop_start = true;
#endif
        }

        if (is_listen) {
            int recv_length;
            if ((recv_length = dg.RecvFrom(read, 9000)) == -1) {
                fprintf(stderr, "ERROR recvfrom\n");
                exit(1);
            }
            read[recv_length] = '\0';

            // fprintf(stderr, "read: %s\n", read);
        } else {
            while (fgets(read, 9000, stdin) == NULL) {
            }
            // fprintf(stderr, "read: %s\n", read);
        }

#ifdef MEASURE2
        if (loop_start == false) {
            loop_start = true;
            clock_gettime(CLOCK_REALTIME, &start_ts);
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

                update_id       = ns->Update(value["node"], *send_data);
                send_data->type = 1;

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
                if (is_assign) {
                    if (assign_map[update_id] == 1) {

#ifdef MEASURE
                        search_begin = chrono::high_resolution_clock::now();
#endif

                        // neighbor.shrink_to_fit();
                        // const vector<int> &neighbor =
                        // ns->GetNeighbor(update_id);
                        // neighbor.clear();
                        send_data->neighbor_size = 0;
                        ns->GetNeighbor(update_id, *send_data);

#ifdef MEASURE
                        search_end = chrono::high_resolution_clock::now();
                        search_elapsed +=
                            chrono::duration_cast<chrono::nanoseconds>(
                                search_end - search_begin);
#endif

                        if (send_data->neighbor[0] != -1) {
                            neighbor_count += send_data->neighbor_size;
                        }

#ifdef MEASURE
                        send_begin = chrono::high_resolution_clock::now();
#endif
                        ns->SendDeltaHQ(*send_data);

#ifdef MEASURE
                        send_end = chrono::high_resolution_clock::now();
                        send_elapsed +=
                            chrono::duration_cast<chrono::nanoseconds>(
                                send_end - send_begin);
#endif
                        search_loop_count++;
                    }
                } else {
#ifdef MEASURE
                    search_begin = chrono::high_resolution_clock::now();
#endif

                    // neighbor.shrink_to_fit();
                    // const vector<int> &neighbor =
                    // ns->GetNeighbor(update_id);
                    // neighbor.clear();
                    send_data->neighbor_size = 0;
                    ns->GetNeighbor(update_id, *send_data);

#ifdef MEASURE
                    search_end = chrono::high_resolution_clock::now();
                    search_elapsed +=
                        chrono::duration_cast<chrono::nanoseconds>(
                            search_end - search_begin);
#endif

                    if (send_data->neighbor[0] != -1) {
                        neighbor_count += send_data->neighbor_size;
                    }
                    // if (neighbor[0] != -1) {
                    //     neighbor_count += neighbor.size();
                    // }

#ifdef MEASURE
                    send_begin = chrono::high_resolution_clock::now();
#endif
                    ns->SendDeltaHQ(*send_data);

#ifdef MEASURE
                    send_end = chrono::high_resolution_clock::now();
                    send_elapsed +=
                        chrono::duration_cast<chrono::nanoseconds>(
                            send_end - send_begin);
#endif
                    search_loop_count++;
                }
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
                search_avg  = search_elapsed / search_loop_count;
                send_avg    = send_elapsed / search_loop_count;

                fprintf(stderr,
                        "node_number loot_count search_count area width "
                        "height density "
                        "neighbor_avg "
                        "all_elapsed parse_elapsed update_elapsed "
                        "search_elapsed send_elapsed all_avg_elapsed "
                        "parse_avg_elapsed update_avg_elapsed "
                        "search_avg_elapsed send_avg_elapsed\n");

                fprintf(stdout,
                        "%d %d %d %.4f %.4f %.4f %.4f %d %lld.%09lld "
                        "%lld.%09lld "
                        "%lld.%09lld "
                        "%lld.%09lld %lld.%09lld %lld.%09lld %lld.%09lld "
                        "%lld.%09lld %lld.%09lld %lld.%09lld \n",
                        node_number,
                        loop_count,
                        search_loop_count,
                        area,
                        field_size[0],
                        field_size[1],
                        density,
                        neighbor_count / search_loop_count,

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
                        neighbor_count / search_loop_count);

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
                avg = search_elapsed / search_loop_count;
                fprintf(stderr,
                        "\tsearch:%lld.%09lld\n\t\tavg:%lld.%09lld\n",
                        search_elapsed.count() / 1000000000,
                        search_elapsed.count() % 1000000000,
                        avg.count() / 1000000000,
                        avg.count() % 1000000000);
                avg = send_elapsed / search_loop_count;
                fprintf(stderr,
                        "\tsend:  %lld.%09lld\n\t\tavg:%lld.%09lld\n",
                        send_elapsed.count() / 1000000000,
                        send_elapsed.count() % 1000000000,
                        avg.count() / 1000000000,
                        avg.count() % 1000000000);
#endif

#ifdef MEASURE2

                fprintf(stderr,
                        "hashmot: %ld.%09ld\n",
                        start_ts.tv_sec,
                        start_ts.tv_nsec);

                // struct tm start_tm;
                // localtime_r(&start_ts.tv_sec, &start_tm);
                // fprintf(stderr,
                //         "hashmot: %02d:%02d:%02d.%09ld\n",
                //         start_tm.tm_hour,
                //         start_tm.tm_min,
                //         start_tm.tm_sec,
                //         start_ts.tv_nsec);
#endif

                for (int i = 0; i < 10; i++) {
                    ns->SendDeltaHQ();
                }
                valueAllocator.Clear();
                parseAllocator.Clear();
                goto FINISH_HANDLER;
            }
        }
        ++loop_count;
    }

FINISH_HANDLER:
    fprintf(stderr, "\n-- Success --\n\n");
    delete ns;
    delete send_data;

    return 0;
}
