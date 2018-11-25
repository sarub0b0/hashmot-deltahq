/*
 * Copyright (c) 2006-2013 The StarBED Project  All rights reserved.
 *
 * See the file 'LICENSE' for licensing information.
 *
 */

/************************************************************************
 *
 * QOMET Emulator Implementation
 *
 * File name: deltaQ.c
 * Function: Main source file of the deltaQ program that uses the deltaQ
 *           library to compute the communication conditions between
 *           emulated nodes
 *
 * Author: Razvan Beuran
 * Changes: Kunio AKASHI
 *
 * $Id: deltaQ.c 161 2014-02-04 00:32:16Z razvan $
 *
 ***********************************************************************/
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include <deltaQ.h> // include file of deltaQ library
#include <message.h>
#include <thread.h>
#include <output.h>
#include <json.h>
#include <input.h>
#include <socket.h>
#include <jansson.h>

//#define DISABLE_EMPTY_TIME_RECORDS

///////////////////////////////////////////////////////////
// Various constants
///////////////////////////////////////////////////////////

// type of motion output
//#define MOTION_OUTPUT_NAM 0
//#define MOTION_OUTPUT_NS2 1

///////////////////////////////////////////////////////////
// Constants used to add artificial noise;
// currently only used for specific experimental purposes;
// may become configurable at user level in future versions
///////////////////////////////////////////////////////////

// Uncomment the line to enable noise addition
//#define ADD_NOISE

#ifdef ADD_NOISE
#define NOISE_DEFAULT -95 // default noise level
#define NOISE_START1 30   // start time for first noise source
#define NOISE_STOP1 90    // stop time for first noise source
#define NOISE_LEVEL1 -50  // level of first noise source

#define NOISE_START2 150 // start time for second noise source
#define NOISE_STOP2 210  // stop time for second noise source
#define NOISE_LEVEL2 -60 // level of first second source
#endif

///////////////////////////////////////////////////////////
// Constant used to activate the auto-connect feature
// (currently only used for active tags)
// PROBABLY SHOULD BE REMOVED??????????
///////////////////////////////////////////////////////////

//#define AUTO_CONNECT_ACTIVE_TAGS

///////////////////////////////////
// Generic variables and functions
///////////////////////////////////

// print license info
static void license(FILE *f) {
    fprintf(f,
            "Copyright (c) 2006-2013 The StarBED Project  All rights "
            "reserved.\n");
    fprintf(f, "\n");
    fprintf(f,
            "Redistribution and use in source and binary forms, with or "
            "without\n");
    fprintf(f,
            "modification, are permitted provided that the following "
            "conditions\n");
    fprintf(f, "are met:\n");
    fprintf(f,
            "1. Redistributions of source code must retain the above "
            "copyright\n");
    fprintf(
        f,
        "   notice, this list of conditions and the following disclaimer.\n");
    fprintf(f,
            "2. Redistributions in binary form must reproduce the above "
            "copyright\n");
    fprintf(f,
            "   notice, this list of conditions and the following disclaimer "
            "in the\n");
    fprintf(f,
            "   documentation and/or other materials provided with the "
            "distribution.\n");
    fprintf(f,
            "3. Neither the name of the project nor the names of its "
            "contributors\n");
    fprintf(f,
            "   may be used to endorse or promote products derived from this "
            "software\n");
    fprintf(f, "   without specific prior written permission.\n");
    fprintf(f, "\n");
    fprintf(f,
            "THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS \"AS "
            "IS\" AND\n");
    fprintf(
        f,
        "ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, "
        "THE\n");
    fprintf(f,
            "IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A "
            "PARTICULAR PURPOSE\n");
    fprintf(
        f,
        "ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE "
        "LIABLE\n");
    fprintf(f,
            "FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR "
            "CONSEQUENTIAL\n");
    fprintf(
        f,
        "DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE "
        "GOODS\n");
    fprintf(f,
            "OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS "
            "INTERRUPTION)\n");
    fprintf(f,
            "HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN "
            "CONTRACT, STRICT\n");
    fprintf(
        f,
        "LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN "
        "ANY WAY\n");
    fprintf(f,
            "OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE "
            "POSSIBILITY OF\n");
    fprintf(f, "SUCH DAMAGE.\n\n");
}

// print usage info
static void usage(FILE *f) {
    fprintf(f, "\nUsage: deltaQ [options] <scenario_file.xml>\n");
    fprintf(f, "General options:\n");
    fprintf(f,
            " -h, --help             - print this help message and exit\n");
    fprintf(f,
            " -v, --version          - print version information and exit\n");
    fprintf(f,
            " -l, --license          - print license information and exit\n");
    fprintf(f, "Output control:\n");
    // fprintf(f,
    //         " -t, --text-only        - enable ONLY text deltaQ output (no "
    //         "binary output)\n");
    // fprintf(f,
    //         " -b, --binary-only      - enable ONLY binary deltaQ output (no
    //         " "text output)\n");
    // fprintf(f,
    //         " -n, --no-deltaQ        - NO text NOR binary deltaQ output
    //         will " "be written\n");
    // fprintf(f,
    //         " -m, --motion-nam       - enable output of motion data in NAM
    //         " "format\n");
    // fprintf(f,
    //         " -s, --motion-ns        - enable output of motion data in NS-2
    //         " "format\n");
    // fprintf(f, " -j, --object           - enable output of object data\n");
    // fprintf(f,
    //         " -o, --output <base>    - use <base> as base for generating "
    //         "output files,\n");
    // fprintf(f, "                          instead of the input file
    // name\n");
    fprintf(f, "Computation control:\n");
    fprintf(f, " -i --id                - own node id\n");
    fprintf(f,
            " -a --ipaddr            - send message to ip address: port\n");
    fprintf(f,
            " -p --port              - send message to ip address: port\n");
    fprintf(f, " -L --listen            - listen port\n");
    fprintf(f,
            " -t, --threads <number> - number of thread. default number is "
            "number of cpu cores\n");
    // fprintf(f, " -D, --disable-motion   - disable motion computation\n");
    fprintf(f, "\n");
    fprintf(f, "See the documentation for more usage details.\n");
    fprintf(
        f,
        "Please send any comments or bug reports to 'info@starbed.org'.\n\n");
}

long int parse_svn_revision(char *svn_revision_str) {
    /*
    According to svnversion version 1.4.2 (r22196)
    this is the expected output:

    The version number will be a single number if the working
    copy is single revision, unmodified, not switched and with
    an URL that matches the TRAIL_URL argument.  If the working
    copy is unusual the version number will be more complex:

    4123:4168     mixed revision working copy  [=> use most recent (highest)
    number]
    4168M         modified working copy        [=> use number part]
    4123S         switched working copy        [=> use number part]
    4123:4168MS   mixed revision, modified, switched working copy [=> use most
    recent (highest) number part]

     */

    char *end_ptr, *next_ptr;
    long int retval, backup_value;

    errno  = 0;
    retval = strtol(svn_revision_str, &end_ptr, 10);

    if ((errno == ERANGE && (retval == LONG_MAX || retval == LONG_MIN)) ||
        (errno != 0 && retval == 0)) {
        WARNING("Could not parse integer.");
        perror("strtol");
        return ERROR;
    }

    if (end_ptr == svn_revision_str) {
        DEBUG("No digits were found");
        return ERROR;
    }

    // if we got here, strtol() successfully parsed a number
    DEBUG("Function strtol() returned %ld.", retval);
    // not necessarily an error...
    if (*end_ptr != '\0') {
        DEBUG("Further characters after number: '%s'", end_ptr);

        // if ':' appeared, it means we must parse the next number,
        // otherwise we ignore what follows
        if (*end_ptr == ':') {
            next_ptr     = end_ptr + 1;
            backup_value = retval;

            errno  = 0;
            retval = strtol(next_ptr, &end_ptr, 10);

            if ((errno == ERANGE &&
                 (retval == LONG_MAX || retval == LONG_MIN)) ||
                (errno != 0 && retval == 0)) {
                perror("strtol");
                return ERROR;
            }

            if (end_ptr == next_ptr) {
                WARNING("No digits were found");
                retval = backup_value;
                printf("restoring %ld\n", retval);
            } else {
                DEBUG("Function strtol() returned %ld", retval);
            }

            // not necessarily an error...
            if (*end_ptr != '\0') {
                ;
            }
        }
    }

    return retval;
}

// structure holding the names of long options and their
// short version; when updating also change the 'short_options'
// structure below
static struct option long_options[] = {{"help", 0, 0, 'h'},
                                       {"id", 1, 0, 'i'},
                                       {"ipaddr", 1, 0, 'a'},
                                       {"port", 1, 0, 'p'},
                                       {"listen", 1, 0, 'L'},
                                       {"version", 0, 0, 'v'},
                                       {"license", 0, 0, 'l'},
                                       {"process", 1, 0, 't'},
                                       {0, 0, 0, 0}};

// structure holding name of short options;
// should match the 'long_options' structure above
static char short_options[15] = "hi:vl:p:t:a:L:";

///////////////////////////////////////////////////
// main function
///////////////////////////////////////////////////

int main(int argc, char **argv) {

    json_set_alloc_funcs(malloc, free);

    TCHK_START(all);

    int error_status = SUCCESS;

    int own_id = -1;
    // xml scenario object
    xml_cls *xml_scenario = NULL;

    // helper scenario object used temporarily
    struct scenario_class *scenario = NULL;

    // file pointers
    FILE *scenario_file = NULL; // scenario file pointer
    FILE *init_file     = NULL; // scenario file pointer

    // file name related strings
    char scenario_filename[MAX_STRING];

    // revision related variables
    // long int svn_revision;
    char svn_revision_str[MAX_STRING];
    char qomet_name[MAX_STRING];
    char c;

    int thread_number          = 0;
    unsigned short port        = 0;
    unsigned short listen_port = 0;
    char ipaddr[16];

    ////////////////////////////////////////////////////////////
    // initialization

    // #ifndef SVN_REVISION
    //     DEBUG("SVN_REVISION not defined.");
    //     svn_revision = ERROR;
    // #else
    //     if ((strcmp(SVN_REVISION, "exported") == 0) ||
    //         (strlen(SVN_REVISION) == 0)) {
    //         DEBUG("SVN_REVISION defined but equal to 'exported' or ''.");
    //         svn_revision = ERROR;
    //     } else {
    //         DEBUG("SVN_REVISION defined.");
    //         svn_revision = parse_svn_revision(SVN_REVISION);
    //     }
    // #endif

    //     if (svn_revision == ERROR) {
    //         WARNING("Could not identify revision number.");
    //         sprintf(svn_revision_str, "N/A");
    //     } else {
    //         sprintf(svn_revision_str, "%ld", svn_revision);
    //     }

    snprintf(qomet_name,
             MAX_STRING,
             "QOMET v%d.%d.%d %s- deltaQ (revision %s)",
             MAJOR_VERSION,
             MINOR_VERSION,
             SUBMINOR_VERSION,
             (IS_BETA == TRUE) ? "beta " : "",
             svn_revision_str);

    printf("JANSSON VERSION %s\n", JANSSON_VERSION);

    memset(ipaddr, 0, sizeof(char) * 16);
    ////////////////////////////////////////////////////////////
    // option parsing

    // uncomment the following line to disable
    // option parsing error printing
    // opterr = 0;

    if (argc == 1) {
        usage(stdout);
        exit(1);
    }

    // parse options
    while ((c = getopt_long(argc, argv, short_options, long_options, NULL)) !=
           -1) {
        switch (c) {
            // generic options
            case 'h':
                printf("\n%s: A versatile wireless network emulator.\n",
                       qomet_name);
                usage(stdout);
                exit(0);
            case 'i':
                own_id = atoi(optarg);
                break;
            case 'v':
                printf(
                    "\n%s.  Copyright (c) 2006-2013 The StarBED Project.\n\n",
                    qomet_name);
                exit(0);
            case 'l':
                printf("\n%s.\n", qomet_name);
                license(stdout);
                exit(0);
            case 'p':
                port = (unsigned short) atoi(optarg);
                break;
            case 'L':
                listen_port = (unsigned short) atoi(optarg);
                break;
            case 't':
                thread_number = atol(optarg);
                break;
            case 'a':
                strncpy(ipaddr, optarg, 16);
                break;
            // unknown options
            case '?':
                printf("Try --help for more info\n");
                exit(1);
            default:
                WARNING("Unimplemented option -- %c", c);
                WARNING("Try --help for more info");
                exit(1);
        }
    }

    if (thread_number <= 0) {
        thread_number = thread_get_cpu_number();
    }
    if (thread_number <= 0) {
        WARNING("Not get core number error");
        goto ERROR_HANDLE;
    }

    INFO("%d threads", thread_number);

    int is_broadcast = 0;
    if (0 < port && ipaddr[0] != '\0') {
        is_broadcast = 1;
    }

    int is_listen_dgram = 0;
    if (0 < listen_port) {
        is_listen_dgram = 1;
    }
    // socket initialize
    bc_info_t info;
    if (is_broadcast) {
        socket_init_state(ipaddr, port, &info);
    }
    dgram_info_t dgram;
    if (is_listen_dgram) {
        dgram.port = listen_port;

        dgram_listen(&dgram);
    }

    // optind represents the index where option parsing stopped
    // and where non-option arguments parsing can start;
    // check whether non-option arguments are present
    if (argc == optind) {
        WARNING("No scenario configuration file was provided.");
        printf("\n%s: A versatile wireless network emulator.\n", qomet_name);
        usage(stdout);
        exit(1);
    }

    ////////////////////////////////////////////////////////////
    // more initialization
    ////////////////////////////////////////////////////////////

    INFO("\n-- Wireless network emulator %s --", qomet_name);
    INFO("Starting...");

#ifdef ADD_NOISE
    fprintf(stderr, "Noise will be added to your experiment!\n");
    fprintf(stderr, "Press <Return> to continue...\n");
    getchar();
#endif

    // try to allocate the xml_scenario object
    xml_scenario = (xml_cls *) malloc(sizeof(xml_cls));

    if (xml_scenario == NULL) {
        WARNING("Cannot allocate memory (tried %zd bytes);", sizeof(xml_cls));
        WARNING("you may need to reduce maximum scenario size");
        WARNING(
            "by editing the maximum data sizes constants in the file "
            "'deltaQ/scenario.h'");
        WARNING(
            "(MAX_NODES, MAX_OBJECTS, MAX_ENVIRONMENTS, MAX_MOTIONS and "
            "MAX_CONNECTIONS)");
        goto ERROR_HANDLE;
    } else {
        printf(
            "MEMORY: Allocated %.2f MB (%zd bytes) for internal scenario "
            "storage.\n",
            sizeof(xml_cls) / 1048576.0,
            sizeof(xml_cls));
    }

    // initialize the scenario object
    scenario_init(&(xml_scenario->scenario));

    ////////////////////////////////////////////////////////////
    // scenario parsing phase
    ////////////////////////////////////////////////////////////
    // INFO("\n-- QOMET Emulator: Scenario Parsing --\n");
    // if (strlen(argv[optind]) > MAX_STRING) {
    //     WARNING("Input file name '%s' longer than %d characters!",
    //             argv[optind],
    //             MAX_STRING);
    //     goto ERROR_HANDLE;
    // } else {
    //     strncpy(scenario_filename, argv[optind], MAX_STRING - 1);
    // }
    INFO("\n-- QOMET Emulator: Init Parsing --\n");
    if (strlen(argv[optind]) > MAX_STRING) {
        WARNING("Input file name '%s' longer than %d characters!",
                argv[optind],
                MAX_STRING);
        goto ERROR_HANDLE;
    } else {
        strncpy(scenario_filename, argv[optind], MAX_STRING - 1);
    }
    init_file = fopen(scenario_filename, "r");
    if (init_file == NULL) {
        WARNING("Cannot open scenario file '%s'!", scenario_filename);
        goto ERROR_HANDLE;
    }

    // open scenario file
    // scenario_file = fopen(scenario_filename, "r");
    // if (scenario_file == NULL) {
    //     WARNING("Cannot open scenario file '%s'!", scenario_filename);
    //     goto ERROR_HANDLE;
    // }

    // parse scenario file
    // if (xml_scenario_parse(scenario_file, xml_scenario) == ERROR) {
    //     WARNING("Cannot parse scenario file '%s'!", scenario_filename);
    //     goto ERROR_HANDLE;
    // }

    // print parse summary
    // INFO("Scenario file '%s' parsed.", scenario_filename);
#ifdef MESSAGE_DEBUG
    DEBUG("Loaded scenario file summary:");
    xml_scenario_print(xml_scenario);
#endif

    // save a pointer to the scenario data structure
    scenario         = &(xml_scenario->scenario);
    scenario->own_id = own_id;

    pthread_mutex_t ibuf_mutex_value;
    pthread_mutex_t ibuf_mutex_queue;
    pthread_cond_t ibuf_cond;

    pthread_mutex_t ibuf_mut_write;
    pthread_cond_t ibuf_cond_write;

    pthread_mutex_init(&ibuf_mutex_value, NULL);
    pthread_mutex_init(&ibuf_mutex_queue, NULL);
    pthread_cond_init(&ibuf_cond, NULL);

    pthread_mutex_init(&ibuf_mut_write, NULL);
    pthread_cond_init(&ibuf_cond_write, NULL);

    input_buffer_t ibuf;
    ibuf.bufs_size       = 10000;
    ibuf.buf_size        = 9000;
    ibuf.read_pos        = 0;
    ibuf.write_pos       = 0;
    ibuf.buf_count       = 0;
    ibuf.dgram           = dgram;
    ibuf.is_listen_dgram = is_listen_dgram;

    // ibuf.is_writable        = 1;
    ibuf.mutex_value = &ibuf_mutex_value;
    ibuf.mutex_queue = &ibuf_mutex_queue;
    ibuf.cond        = &ibuf_cond;
    // ibuf.mutex_write_buffer = &ibuf_mut_write;
    // ibuf.cond_write_buffer  = &ibuf_cond_write;
    // ibuf.read_idx           = 0;
    // ibuf.write_idx          = 0;

    meteor_param_t *meteor_param = NULL;

    struct connection_class **neighbor = NULL;
    int neighbor_number                = 0;

    int center_id          = -1;
    int received_center_id = -1;
    int is_other_update    = 0;
    int is_other_delete    = 0;

    // =====================================================
    // all node calculation mode
    // =====================================================
    int **all_neighbor_ids          = NULL;
    int **all_neighbor_ids_bmp      = NULL;
    int **all_prev_neighbor_ids_bmp = NULL;

    // =====================================================
    // 1node calculation mode
    // =====================================================
    int *neighbor_ids          = NULL;
    int *neighbor_ids_bmp      = NULL;
    int *prev_neighbor_ids_bmp = NULL;

    ////////////////////////////////////////////////////////////
    // open test json file
    ////////////////////////////////////////////////////////////
    // ibuf.test_json = fopen("input_deltaHQ.json", "r");
    // if (!ibuf.test_json) {
    //     WARNING("fopen error");
    //     goto ERROR_HANDLE;
    // }
    ibuf.buffers = (char **) malloc(sizeof(char *) * ibuf.bufs_size);
    for (int i = 0; i < ibuf.bufs_size; i++) {
        ibuf.buffers[i] = (char *) malloc(sizeof(char) * ibuf.buf_size);
    }

    pthread_t pt_input_buf;

    pthread_create(&pt_input_buf, NULL, thread_buffer_write, &ibuf);

    // =====================================================
    // read json and init scenario
    // =====================================================
    // WARNING("json_init_scenario start");
    if (json_init_scenario(scenario, init_file) == ERROR) {
        WARNING("json_init_scenario");
        goto ERROR_HANDLE;
    }
    // WARNING("json_init_scenario done");
    // WARNING("malloc start");
    meteor_param = (meteor_param_t *) malloc(sizeof(meteor_param_t));

    meteor_param->add =
        (int *) malloc(sizeof(int) * (scenario->node_number + 1));
    meteor_param->delete =
        (int *) malloc(sizeof(int) * (scenario->node_number + 1));
    meteor_param->update =
        (int *) malloc(sizeof(int) * (scenario->node_number + 1));

    neighbor = (struct connection_class **) malloc(
        sizeof(struct connection_class *) * scenario->connection_number);
    if (0 <= own_id) {
        // =====================================================
        // one node
        // =====================================================
        neighbor_ids =
            (int *) malloc(sizeof(int) * (scenario->node_number + 1));
        neighbor_ids_bmp =
            (int *) malloc(sizeof(int) * scenario->node_number);
        prev_neighbor_ids_bmp =
            (int *) malloc(sizeof(int) * scenario->node_number);

        // WARNING("malloc loop start");

        memset(neighbor_ids, -1, sizeof(int) * (scenario->node_number + 1));
        memset(neighbor_ids_bmp, 0, sizeof(int) * scenario->node_number);
        memset(prev_neighbor_ids_bmp, 0, sizeof(int) * scenario->node_number);

        // printf("\nconnection_number=%d\n",
        // scenario->connection_number); for (int connection_i = 0;
        // connection_i < scenario->connection_number;
        //      connection_i++) {
        //     connection_print(&(scenario->connections[connection_i]));
        // }

        fprintf(stderr, "\n-- Initial Neighbors Update:\n");
        // center_id == own_id ならset_neighbor_bmp
        // center_id != own_id ならneighbor_bmpを触らない
        for (int i = 0; i < scenario->node_number; ++i) {
            // fprintf(stderr, "init number=%d\n", i);
            neighbor_number = update_neighbors(scenario,
                                               neighbor,
                                               own_id,
                                               neighbor_ids,
                                               &ibuf,
                                               &is_other_update,
                                               &is_other_delete,
                                               &received_center_id,
                                               0);

            if (i != own_id) {
                continue;
            }
#ifdef DEBUG_PRINT
            printf("neighbor_ids(%d)=", own_id);
            for (int j = 0; neighbor_ids[j] != -1; ++j) {
                printf("%d ", neighbor_ids[j]);
            }
            puts("");
#endif

            set_neighbor_bmp(neighbor_ids,
                             neighbor_ids_bmp,
                             is_other_update,
                             is_other_delete,
                             scenario->node_number);
        }
        is_other_update = 0;
    } else {
        // =====================================================
        // all node
        // =====================================================

        all_neighbor_ids =
            (int **) malloc(sizeof(int *) * (scenario->node_number + 1));
        all_neighbor_ids_bmp =
            (int **) malloc(sizeof(int *) * scenario->node_number);
        all_prev_neighbor_ids_bmp =
            (int **) malloc(sizeof(int *) * scenario->node_number);

        // WARNING("malloc loop start");

        for (int i = 0; i < scenario->node_number; i++) {

            all_neighbor_ids[i] =
                (int *) malloc(sizeof(int) * (scenario->node_number + 1));
            memset(all_neighbor_ids[i],
                   -1,
                   sizeof(int) * (scenario->node_number + 1));

            all_neighbor_ids_bmp[i] =
                (int *) malloc(sizeof(int) * scenario->node_number);
            all_prev_neighbor_ids_bmp[i] =
                (int *) malloc(sizeof(int) * scenario->node_number);

            memset(all_neighbor_ids_bmp[i],
                   0,
                   sizeof(int) * scenario->node_number);
            memset(all_prev_neighbor_ids_bmp[i],
                   0,
                   sizeof(int) * scenario->node_number);
        }
        // WARNING("malloc done");

        // printf("\nconnection_number=%d\n",
        // scenario->connection_number); for (int connection_i = 0;
        // connection_i < scenario->connection_number;
        //      connection_i++) {
        //     connection_print(&(scenario->connections[connection_i]));
        // }
        WARNING("update_neighbors");

        for (int i = 0; i < scenario->node_number; ++i) {
            neighbor_number = update_all_neighbors(
                scenario, neighbor, &center_id, all_neighbor_ids, &ibuf, 0);

#ifdef DEBUG_PRINT
            printf("all_neighbor_ids(%d)=", i);
            for (int j = 0; all_neighbor_ids[i][j] != -1; j++) {
                printf("%d ", all_neighbor_ids[i][j]);
            }
            puts("");
#endif
            if (neighbor_number < 0) {
                goto ERROR_HANDLE;
            }

            set_all_neighbor_bmp(
                center_id, all_neighbor_ids, all_neighbor_ids_bmp);
        }
    }

    // exit(1);
    // set_meteor_param(meteor_param,
    //                  neighbor_ids_bmp,
    //                  prev_neighbor_ids_bmp,
    //                  center_id,
    //                  scenario->node_number);

    // scenario_print(scenario);

    ////////////////////////////////////////////////////////////
    // computation phase
    ////////////////////////////////////////////////////////////

    // Latest version of expat (expat-2.0.1-11.el6_2.i686) fixed a bug
    // related to hash functions by initializing internally the random
    // number generator seed to values obtained from the clock. This
    // causes random values computed by us to change at each run. To fix
    // this we initialize the random number generator seed to its default
    // value (1).
    // References:
    // [1] http://cmeerw.org/blog/759.html
    // [2] https://bugzilla.redhat.com/show_bug.cgi?id=786617
    // [3] https://rhn.redhat.com/errata/RHSA-2012-0731.html
    DEBUG(
        "Initialize random seed because it was changed by expat during "
        "parsing.\n");
    srand(1);

    INFO("\n-- QOMET Emulator: Scenario Computation --");

#ifdef MESSAGE_DEBUG
    int node_i, env_i, obj_i, motion_i, connection_i;

    // print the initial condition of the scenario
    // printf("\n-- Initial condition:\n");
    fprintf(stderr, "\n-- Initial condition:\n");
    printf("\nnode_number=%d\n", scenario->node_number);
    for (node_i = 0; node_i < scenario->node_number; node_i++) {
        node_print(&(scenario->nodes[node_i]));
    }
    printf("\nobject_number=%d\n", scenario->object_number);
    for (obj_i = 0; obj_i < scenario->object_number; obj_i++) {
        object_print(&(scenario->objects[obj_i]));
    }
    printf("\nenvironment_number=%d\n", scenario->environment_number);
    for (env_i = 0; env_i < scenario->environment_number; env_i++) {
        environment_print(&(scenario->environments[env_i]));
    }
    printf("\nmotion_number=%d\n", scenario->motion_number);
    for (motion_i = 0; motion_i < scenario->motion_number; motion_i++) {
        motion_print(&(scenario->motions[motion_i]));
    }

    printf("\nconnection_number=%d\n", scenario->connection_number);
    for (connection_i = 0; connection_i < scenario->connection_number;
         connection_i++) {
        connection_print(&(scenario->connections[connection_i]));
    }

    printf(
        "node_number=%d\nobject_number=%d\nenvironment_number=%d\nmotion_"
        "number=%d\nconnection_number=%d\ninterface_number=%d\n",
        scenario->node_number,
        scenario->object_number,
        scenario->environment_number,
        scenario->motion_number,
        scenario->connection_number,
        scenario->interface_number);
#endif

    // initialize scenario state
    INFO("\n-- Scenario initialization:");

    fprintf(stderr, "\n-- Scenario initialization:\n");

    TCHK_START(scenario_init_state);
    if (scenario_init_state(scenario,
                            xml_scenario->jpgis_filename_provided,
                            xml_scenario->jpgis_filename,
                            thread_number) == ERROR) {
        fflush(stdout);
        WARNING("Error during scenario initialization. Aborting...");
        goto ERROR_HANDLE;
    }
    TCHK_END(scenario_init_state);

    // scenario_sort_connections(
    // scenario->connections, scenario->connection_number, own_id);
    // printf("\nconnection_number=%d\n", scenario->connection_number);
    // for (int connection_i = 0; connection_i < scenario->connection_number;
    //      connection_i++) {
    //     connection_print(&(scenario->connections[connection_i]));
    // }

#ifdef MESSAGE_DEBUG

    // print the initial condition of the scenario
    printf("\n-- Initial condition:\n");
    fprintf(stderr, "\n-- Initial condition:\n");
    printf("\nnode_number=%d\n", scenario->node_number);
    for (node_i = 0; node_i < scenario->node_number; node_i++) {
        node_print(&(scenario->nodes[node_i]));
    }
    printf("\nobject_number=%d\n", scenario->object_number);
    for (obj_i = 0; obj_i < scenario->object_number; obj_i++) {
        object_print(&(scenario->objects[obj_i]));
    }
    printf("\nenvironment_number=%d\n", scenario->environment_number);
    for (env_i = 0; env_i < scenario->environment_number; env_i++) {
        environment_print(&(scenario->environments[env_i]));
    }
    printf("\nmotion_number=%d\n", scenario->motion_number);
    for (motion_i = 0; motion_i < scenario->motion_number; motion_i++) {
        motion_print(&(scenario->motions[motion_i]));
    }

    printf("\nconnection_number=%d\n", scenario->connection_number);
    for (connection_i = 0; connection_i < scenario->connection_number;
         connection_i++) {
        connection_print(&(scenario->connections[connection_i]));
    }

    printf(
        "node_number=%d\nobject_number=%d\nenvironment_number=%d\nmotion_"
        "number=%d\nconnection_number=%d\ninterface_number=%d\n",
        scenario->node_number,
        scenario->object_number,
        scenario->environment_number,
        scenario->motion_number,
        scenario->connection_number,
        scenario->interface_number);
#endif
    ////////////////////////////////////////////////////////////
    // processing phase
    ////////////////////////////////////////////////////////////

    // start processing the scenario
    TCHK_START(deltaQ);

    INFO("\n-- Scenario processing:");
    fprintf(stderr, "\n-- Scenario processing:\n");

    // init value
    pthread_mutex_t mut_assign;
    pthread_cond_t cond_assign;

    pthread_mutex_t mut_calc;
    pthread_cond_t cond_calc;

    pthread_barrier_t br_assign;
    pthread_barrierattr_t br_assign_attr;

    pthread_barrier_t br_calc;
    pthread_barrierattr_t br_calc_attr;

    pthread_mutex_init(&mut_assign, NULL);
    pthread_mutex_init(&mut_calc, NULL);

    pthread_cond_init(&cond_assign, NULL);
    pthread_cond_init(&cond_calc, NULL);

    pthread_barrier_init(&br_assign, &br_assign_attr, thread_number + 1);
    pthread_barrier_init(&br_calc, &br_calc_attr, thread_number + 1);
#ifdef __APPLE__
    pthread_mutex_t mut_set_index;
    pthread_mutex_init(&mut_set_index, NULL);
#endif

    pthread_t *pt_scenario;

    pt_scenario = (pthread_t *) malloc(sizeof(pthread_t) * thread_number);

    struct scenario_deltaQ_class *deltaQ_class =
        (struct scenario_deltaQ_class *) malloc(
            sizeof(struct scenario_deltaQ_class));

    deltaQ_class->neighbor = neighbor;

    deltaQ_class->start_conn = (int *) malloc(sizeof(int) * thread_number);
    deltaQ_class->end_conn   = (int *) malloc(sizeof(int) * thread_number);

    deltaQ_class->scenario = scenario;

    deltaQ_class->br_assign = &br_assign;
    deltaQ_class->br_calc   = &br_calc;
    deltaQ_class->calc_done = FALSE;

#ifdef __APPLE__
    deltaQ_class->mut_set_index = &mut_set_index;
#endif

#ifndef __APPLE__
    cpu_set_t cpu;
#endif
    // struct sched_param sched;
    // int priority = 99;
    // int policy   = SCHED_FIFO;

    char name[NAME_SIZE];

    TCHK_START(thread_create);

    struct array_range ar_conn;

    TCHK_START(assign_range_calc);

    TCHK_END(assign_range_calc);

    ar_conn.loop = neighbor_number;
    ar_conn.step = neighbor_number / thread_number;
    ar_conn.mod  = neighbor_number % thread_number;

    for (int i = 0; i < thread_number; i++) {
        pt_scenario[i] = pthread_self();
#ifndef __APPLE__
        CPU_ZERO(&cpu);
        CPU_SET(i, &cpu);
        if (pthread_setaffinity_np(pt_scenario[i], sizeof(cpu_set_t), &cpu) !=
            0) {
            WARNING("pthread_setaffinity_np error");
            goto ERROR_HANDLE;
        }
#endif
        //#ifdef SET_SCHED_PRIORITY
        //        param.sched_priority = priority;
        //        if (pthread_setschedparam(pt_scenario[i], policy,
        //        &param) != 0) {
        //            WARNING("pthread_setschedparam error");
        //            goto ERROR_HANDLE;
        //        }
        //#endif
        TCHK_START(assign_connection_range);

        thread_get_index(&ar_conn);
        deltaQ_class->start_conn[i] = ar_conn.start;
        deltaQ_class->end_conn[i]   = ar_conn.end;

        TCHK_END(assign_connection_range);

        sprintf(name, "%d", i);
#ifndef __APPLE__
        pthread_setname_np(pt_scenario[i], name);
#else
        pthread_setname_np(name);
#endif
        if (pthread_create(
                &pt_scenario[i], NULL, scenario_deltaQ, deltaQ_class) != 0) {
            WARNING("pthread_create error");
            goto ERROR_HANDLE;
        }
    }

    //
    // send init json
    //
    if (0 <= own_id) {
        is_other_update = 0;
        is_other_delete = 0;
        set_meteor_param(meteor_param,
                         neighbor_ids_bmp,
                         prev_neighbor_ids_bmp,
                         own_id,
                         scenario->node_number,
                         is_other_update,
                         is_other_delete,
                         received_center_id);
#ifdef DEBUG_PRINT
        print_meteor_param(own_id, meteor_param);
#endif

        if (send_result_to_meteor(meteor_param,
                                  own_id,
                                  scenario->connections,
                                  is_broadcast,
                                  &info) == ERROR) {
            WARNING("send_result_to_meteor");
        }

    } else {
        for (int i = 0; i < scenario->node_number; i++) {

            set_all_meteor_param(meteor_param,
                                 all_neighbor_ids_bmp,
                                 all_prev_neighbor_ids_bmp,
                                 i,
                                 scenario->node_number);
#ifdef DEBUG_PRINT
            print_all_meteor_param(i, meteor_param);
#endif
            if (send_all_result_to_meteor(meteor_param,
                                          i,
                                          scenario->node_number,
                                          scenario->connections,
                                          is_broadcast,
                                          &info) == ERROR) {
                WARNING("send_result_to_meteor");
            }
        }
    }

    // puts("loop start");
    int loop_exit = FALSE;

    // neighbor_number = update_all_neighbors(scenario,
    //                                        deltaQ_class->neighbor,
    //                                        &center_id,
    //                                        all_neighbor_ids,
    //                                        &ibuf,
    //                                        0);

    if (neighbor_number < 0) {
        WARNING("init finish");
    }

    fprintf(stderr, "\n-- Scenario Loop Start. Update Wait:\n");
    while (loop_exit == FALSE) {

        // sleep(1);

        TCHK_START(scenario_deltaQ);
#ifdef ADD_NOISE
        if (current_time >= NOISE_START1 && current_time < NOISE_STOP1) {
            scenario->Environments[0].noise_power[0] = NOISE_LEVEL1;
        } else if (current_time >= NOISE_START2 &&
                   current_time < NOISE_STOP2) {
            scenario->Environments[0].noise_power[0] = NOISE_LEVEL2;
        } else {
            scenario->Environments[0].noise_power[0] = NOISE_DEFAULT;
        }
#endif

#ifdef AUTO_CONNETCT_ACTIVE_TAGS
        INFO("Auto-connectiong active tag nodes...");
        if (scenario_auto_connect_nodes_at(scenario) == ERROR) {
            WARNING("Error auto-connecting active tag nodes");
        }
#endif

        // ------------------------------------
        // get neighbor node id from stdin
        // ------------------------------------
        if (0 <= own_id) {

            neighbor_number = update_neighbors(scenario,
                                               deltaQ_class->neighbor,
                                               own_id,
                                               neighbor_ids,
                                               &ibuf,
                                               &is_other_update,
                                               &is_other_delete,
                                               &received_center_id,
                                               1);

            if (neighbor_number < 0) {
                WARNING("finish message or read error");
                deltaQ_class->calc_done = TRUE;
                neighbor_number         = 0;
                loop_exit               = TRUE;
                break;
            }

            ar_conn.loop = neighbor_number;
            ar_conn.step = neighbor_number / thread_number;
            ar_conn.mod  = neighbor_number % thread_number;

            for (int i = 0; i < thread_number; i++) {

                if (0 < neighbor_number) {
                    thread_get_index(&ar_conn);
                    deltaQ_class->start_conn[i] = ar_conn.start;
                    deltaQ_class->end_conn[i]   = ar_conn.end;
                } else {
                    deltaQ_class->start_conn[i] = 0;
                    deltaQ_class->end_conn[i]   = 0;
                }
            }

            DEBUG2("br_assign");
            pthread_barrier_wait(&br_assign);

            DEBUG2("br_calc");
            pthread_barrier_wait(&br_calc);

            TCHK_END(scenario_deltaQ);
            // if (is_other_update != 0 || is_other_delete != 0 ||
            //     neighbor_number != 0) {
            if (1) {

                // ------------------------------------
                // set bitmap
                // ------------------------------------
                set_prev_neighbor_bmp(neighbor_ids_bmp,
                                      prev_neighbor_ids_bmp,
                                      scenario->node_number,
                                      is_other_update,
                                      is_other_delete);

                set_neighbor_bmp(neighbor_ids,
                                 neighbor_ids_bmp,
                                 is_other_update,
                                 is_other_delete,
                                 scenario->node_number);

#ifdef DEBUG_PRINT
                printf("-------------- prev --------------\n");
                print_neighbor_bmp(
                    prev_neighbor_ids_bmp, scenario->node_number, own_id);

                printf("\n-------------- current --------------\n");
                print_neighbor_bmp(
                    neighbor_ids_bmp, scenario->node_number, own_id);
#endif
                //
                // send json message
                //
                set_meteor_param(meteor_param,
                                 neighbor_ids_bmp,
                                 prev_neighbor_ids_bmp,
                                 own_id,
                                 scenario->node_number,
                                 is_other_update,
                                 is_other_delete,
                                 received_center_id);

#ifdef DEBUG_PRINT
                print_meteor_param(own_id, meteor_param);
#endif

                if (send_result_to_meteor(meteor_param,
                                          own_id,
                                          scenario->connections,
                                          is_broadcast,
                                          &info) == ERROR) {
                    WARNING("send_result_to_meteor");
                }
            }
        } else {
            neighbor_number = update_all_neighbors(scenario,
                                                   deltaQ_class->neighbor,
                                                   &center_id,
                                                   all_neighbor_ids,
                                                   &ibuf,
                                                   1);

            if (neighbor_number < 0) {
                WARNING("finish message or read error");
                deltaQ_class->calc_done = TRUE;
                neighbor_number         = 0;
                loop_exit               = TRUE;
                break;
            }

            ar_conn.loop = neighbor_number;
            ar_conn.step = neighbor_number / thread_number;
            ar_conn.mod  = neighbor_number % thread_number;

            for (int i = 0; i < thread_number; i++) {

                if (0 < neighbor_number) {
                    thread_get_index(&ar_conn);
                    deltaQ_class->start_conn[i] = ar_conn.start;
                    deltaQ_class->end_conn[i]   = ar_conn.end;
                } else {
                    deltaQ_class->start_conn[i] = 0;
                    deltaQ_class->end_conn[i]   = 0;
                }
            }

            DEBUG2("br_assign");
            pthread_barrier_wait(&br_assign);

            DEBUG2("br_calc");
            pthread_barrier_wait(&br_calc);

            // ------------------------------------
            // set bitmap
            // ------------------------------------
            set_all_prev_neighbor_bmp(center_id,
                                      all_neighbor_ids_bmp,
                                      all_prev_neighbor_ids_bmp,
                                      scenario->node_number);

            set_all_neighbor_bmp(
                center_id, all_neighbor_ids, all_neighbor_ids_bmp);

#ifdef DEBUG_PRINT
            printf("-------------- prev --------------\n");
            print_all_neighbor_bmp(all_prev_neighbor_ids_bmp,
                                   scenario->node_number);

            printf("\n-------------- current --------------\n");
            print_all_neighbor_bmp(all_neighbor_ids_bmp,
                                   scenario->node_number);
#endif

            //
            // send json message
            //
            set_all_meteor_param(meteor_param,
                                 all_neighbor_ids_bmp,
                                 all_prev_neighbor_ids_bmp,
                                 center_id,
                                 scenario->node_number);

#ifdef DEBUG_PRINT
            print_all_meteor_param(center_id, meteor_param);
#endif

            if (send_all_result_to_meteor(meteor_param,
                                          center_id,
                                          scenario->node_number,
                                          scenario->connections,
                                          is_broadcast,
                                          &info) == ERROR) {
                WARNING("send_result_to_meteor");
            }
            TCHK_END(scenario_deltaQ);
        }
    }

    DEBUG2("br_assign");
    pthread_barrier_wait(&br_assign);

    for (int i = 0; i < thread_number; i++) {
        pthread_join(pt_scenario[i], NULL);
    }

    pthread_join(pt_input_buf, NULL);

    // FINISH:
    // if (send_result_to_meteor(meteor_param,
    //                           center_id,
    //                           scenario->node_number,
    //                           scenario->connections,
    //                           is_broadcast,
    //                           &info) == ERROR) {
    //     WARNING("send_result_to_meteor");
    // }

    TCHK_END(deltaQ);

    // if no errors occurred, skip next instruction
    goto FINAL_HANDLE;

ERROR_HANDLE:
    ////////////////////////////////////////////////////////////
    // finalizing phase
    // print this in case of unsuccessful processing
    INFO(
        "\n-- Scenario processing completed with errors (see the "
        "WARNING "
        "messages above)\n\n");
    fprintf(stderr,
            "\n\n-- Scenario processing completed with errors (see the "
            "WARNING messages above)\n\n");
    exit(1);

FINAL_HANDLE:

    // print this in case of successful processing
    INFO("\n-- Scenario processing completed successfully\n\n");
    fprintf(stderr, "\n\n-- Scenario processing completed successfully\n\n");

    if (!scenario) {
        exit(1);
    }

    if (is_broadcast) {
        socket_finalize(&info);
    }
    if (is_listen_dgram) {
        if (dgram.sock != 0) {
            close(dgram.sock);
        }
    }

    if (meteor_param->add) free(meteor_param->add);
    if (meteor_param->delete) free(meteor_param->delete);
    if (meteor_param->update) free(meteor_param->update);

    meteor_param->add    = NULL;
    meteor_param->delete = NULL;
    meteor_param->update = NULL;

    if (0 <= own_id) {
        if (neighbor_ids) free(neighbor_ids);
        neighbor_ids = NULL;

        if (neighbor_ids_bmp) free(neighbor_ids_bmp);
        neighbor_ids_bmp = NULL;

        if (prev_neighbor_ids_bmp) free(prev_neighbor_ids_bmp);
        prev_neighbor_ids_bmp = NULL;

    } else {
        for (int i = 0; i < scenario->node_number; i++) {
            if (all_neighbor_ids[i]) free(all_neighbor_ids[i]);
            if (all_neighbor_ids_bmp[i]) free(all_neighbor_ids_bmp[i]);
            if (all_prev_neighbor_ids_bmp[i])
                free(all_prev_neighbor_ids_bmp[i]);

            all_neighbor_ids[i]          = NULL;
            all_neighbor_ids_bmp[i]      = NULL;
            all_prev_neighbor_ids_bmp[i] = NULL;
        }
    }

    for (int i = 0; i < ibuf.bufs_size; i++) {
        if (ibuf.buffers[i]) free(ibuf.buffers[i]);
        ibuf.buffers[i] = NULL;
    }

    for (int i = 0; i < scenario->connection_number; i++) {
        if (neighbor[i]) neighbor[i] = NULL;
    }

    if (ibuf.buffers) free(ibuf.buffers);
    ibuf.buffers = NULL;

    if (deltaQ_class->start_conn) free(deltaQ_class->start_conn);
    deltaQ_class->start_conn = NULL;

    if (deltaQ_class->end_conn) free(deltaQ_class->end_conn);
    deltaQ_class->end_conn = NULL;

    if (deltaQ_class) free(deltaQ_class);
    deltaQ_class = NULL;

    if (pt_scenario) free(pt_scenario);
    pt_scenario = NULL;

    if (meteor_param) free(meteor_param);
    meteor_param = NULL;

    if (neighbor) free(neighbor);
    neighbor = NULL;

    if (all_neighbor_ids) free(all_neighbor_ids);
    all_neighbor_ids = NULL;

    if (all_neighbor_ids_bmp) free(all_neighbor_ids_bmp);
    all_neighbor_ids_bmp = NULL;

    if (all_prev_neighbor_ids_bmp) free(all_prev_neighbor_ids_bmp);
    all_prev_neighbor_ids_bmp = NULL;

    if (scenario->connections) free(scenario->connections);
    scenario->connections = NULL;

    if (scenario->environments) free(scenario->environments);
    scenario->environments = NULL;

    if (scenario->nodes) free(scenario->nodes);
    scenario->nodes = NULL;

    // pthread_mutex_destroy(&ibuf_mutex_queue);
    // pthread_cond_destroy(&ibuf_cond);

    // pthread_mutex_destroy(&mut_assign);
    // pthread_mutex_destroy(&mut_calc);
    // pthread_cond_destroy(&cond_assign);
    // pthread_cond_destroy(&cond_calc);

    // close output files
    if (scenario_file != NULL) {
        fclose(scenario_file);
        scenario_file = NULL;
    }
    if (xml_scenario != NULL) {
        free(xml_scenario);
        xml_scenario = NULL;
    }
    if (init_file != NULL) {
        fclose(init_file);
        init_file = NULL;
    }
    TCHK_END(all);

    return error_status;
}
