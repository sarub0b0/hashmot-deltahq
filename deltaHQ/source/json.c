/*
 * Copyright (c) 2019 kosay  All rights reserved.
 *
 * See the file 'LICENSE' for licensing information.
 *
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
// #include <stdlib.h>
#include <jansson.h>
#include <math.h>
#include <assert.h>

#ifdef __APPLE__
#include <malloc/malloc.h>
#endif

#include <message.h>
#include <global.h>
#include <deltaQ.h>
#include <connection.h>
#include <xml_scenario.h>
#include <wlan.h>
#include <wimax.h>
#include <zigbee.h>
#include <node.h>
#include <active_tag.h>
#include <json.h>
#include <input.h>
#include <socket.h>
#include <measure.h>

#define SET_STRING_OBJECT(j, p, s)        \
    do {                                  \
        json_t *j_tmp;                    \
        j_tmp = json_object_get(j, s);    \
        if (j_tmp) {                      \
            const char *v;                \
            v = json_string_value(j_tmp); \
            strncpy(p, v, strlen(v));     \
            v = NULL;                     \
        }                                 \
    } while (0);

// #define SET_P_STRING_OBJECT(j, p, s)      \
//     do {                                  \
//         json_t *j_tmp;                    \
//         j_tmp = json_object_get(j, s);    \
//         if (j_tmp) {                      \
//             const char *v;                \
//             v = json_string_value(j_tmp); \
//             strncpy(p, v, strlen(v) + 1); \
//         }                                 \
//     } while (0);

#define SET_DOUBLE_OBJECT(j, p, s)        \
    do {                                  \
        json_t *j_tmp;                    \
        j_tmp = json_object_get(j, s);    \
        if (j_tmp) {                      \
            p = json_number_value(j_tmp); \
        }                                 \
    } while (0);

#define SET_INTEGER_OBJECT(j, p, s)        \
    do {                                   \
        json_t *j_tmp;                     \
        j_tmp = json_object_get(j, s);     \
        if (j_tmp) {                       \
            p = json_integer_value(j_tmp); \
        }                                  \
    } while (0);

#define SEND_JSON_MAXLEN 400
const char *key_init   = "init";
const char *key_update = "update";

char send_json_buf[SEND_JSON_MAXLEN];
char send_json_update_buf[SEND_JSON_MAXLEN] =
    "{\"meteor\":{\"update\":[{\"from_id\":";
char send_json_add_buf[SEND_JSON_MAXLEN] =
    "{\"meteor\":{\"add\":[{\"from_id\":";
char send_json_del_buf[SEND_JSON_MAXLEN] =
    "{\"meteor\":{\"delete\":[{\"from_id\":";

const int update_buf_write_pos = 32;
const int add_buf_write_pos    = 29;
const int delete_buf_write_pos = 32;

void print_json_aux(json_t *element, int indent);

void _node_print(struct node_class *node) {
    printf("id=%d\tposition=(%.2f, %.2f, %.2f)\n",
           node->id,
           node->position.c[0],
           node->position.c[1],
           node->position.c[2]);
}

const char *json_plural(int count) {
    return count == 1 ? "" : "s";
}

void print_json_indent(int indent) {
    int i;
    for (i = 0; i < indent; i++) {
        putchar(' ');
    }
}

void print_json_object(json_t *element, int indent) {
    size_t size;
    const char *key;
    json_t *value;

    print_json_indent(indent);
    size = json_object_size(element);

    printf("JSON Object of %ld pair%s:\n", size, json_plural(size));
    json_object_foreach(element, key, value) {
        print_json_indent(indent + 2);
        printf("JSON Key: \"%s\"\n", key);
        print_json_aux(value, indent + 2);
    }
}

void print_json_array(json_t *element, int indent) {
    size_t i;
    size_t size = json_array_size(element);
    print_json_indent(indent);

    printf("JSON Array of %ld element%s:\n", size, json_plural(size));
    for (i = 0; i < size; i++) {
        print_json_aux(json_array_get(element, i), indent + 2);
    }
}

void print_json_string(json_t *element, int indent) {
    print_json_indent(indent);
    printf("JSON String: \"%s\"\n", json_string_value(element));
}

void print_json_integer(json_t *element, int indent) {
    print_json_indent(indent);
    printf("JSON Integer: \"%" JSON_INTEGER_FORMAT "\"\n",
           json_integer_value(element));
}

void print_json_real(json_t *element, int indent) {
    print_json_indent(indent);
    printf("JSON Real: %f\n", json_real_value(element));
}

void print_json_true(json_t *element, int indent) {
    (void) element;
    print_json_indent(indent);
    printf("JSON True\n");
}

void print_json_false(json_t *element, int indent) {
    (void) element;
    print_json_indent(indent);
    printf("JSON False\n");
}

void print_json_null(json_t *element, int indent) {
    (void) element;
    print_json_indent(indent);
    printf("JSON Null\n");
}

void print_json_aux(json_t *element, int indent) {
    switch (json_typeof(element)) {
        case JSON_OBJECT:
            print_json_object(element, indent);
            break;
        case JSON_ARRAY:
            print_json_array(element, indent);
            break;
        case JSON_STRING:
            print_json_string(element, indent);
            break;
        case JSON_INTEGER:
            print_json_integer(element, indent);
            break;
        case JSON_REAL:
            print_json_real(element, indent);
            break;
        case JSON_TRUE:
            print_json_true(element, indent);
            break;
        case JSON_FALSE:
            print_json_false(element, indent);
            break;
        case JSON_NULL:
            print_json_null(element, indent);
            break;
        default:
            fprintf(
                stderr, "unrecognized JSON type %d\n", json_typeof(element));
    }
}

void print_json(json_t *root) {
    print_json_aux(root, 0);
}

struct msg *msg;

void init_msg_struct(void) {
    msg = malloc(9000);
}

struct msg *read_msg(input_buffer_t *ibuf) {

    int recv_len = 0;
    if (ibuf->is_listen_dgram) {

        recv_len = recv(ibuf->dgram.sock, msg, 9000, 0);
        if (recv_len < 1) {
            perror("recv");
            return NULL;
        }
        // msg[recv_len] = '\0';
    } else {
        while (fgets((char *) msg, 9000, stdin) == NULL) {
        }
        // line[strlen(line)] = '\0';
    }

    if (msg->type == -1) {
        // printf("finish\n");
        fprintf(stderr, "\n-- Received finish json\n");
        return 0;
        // continue;
        // exit(0);
    }
#ifdef DEBUG_PRINT
    printf("%d: %d(%.2f, %.2f) neighbor(%d): ",
           msg->type,
           msg->id,
           msg->x,
           msg->y,
           msg->neighbor_size);
    for (int32_t i = 0; i < msg->neighbor_size; i++) {
        // for (int16_t i = 0; i < 10; i++) {
        printf("%d ", msg->neighbor[i]);
    }
    puts("");
#endif
    // if (line[0] == '\0') {
    //     WARNING("buffer error");
    //     return 0;
    // }

    return msg;
}

json_t *read_json(input_buffer_t *ibuf) {

    char line[9000] = {0};
    // line = buffer_read(ibuf);

    int recv_len = 0;
    if (ibuf->is_listen_dgram) {

        recv_len = recv(ibuf->dgram.sock, line, 9000, 0);
        if (recv_len < 1) {
            perror("recv");
            return NULL;
        }
        line[recv_len] = '\0';
    } else {
        while (fgets(line, 9000, stdin) == NULL) {
        }
        line[strlen(line)] = '\0';
    }

    if (line[0] == '\0') {
        WARNING("buffer error");
        return (json_t *) 0;
    }

    json_error_t error;

    json_t *root         = NULL;
    json_t *first_object = NULL;

#if !defined(MEASURE) && !defined(MEASURE2)
    printf("read json: %s\n", line);
#endif
    TCHK_START(json_loads);

    root = json_loads(line, 0, &error);

    TCHK_END(json_loads);

    if (root) {
        first_object = json_object_get(root, "finish");
        if (first_object) {
            fprintf(stderr, "\n-- Received finish json\n");
            json_decref(root);
            first_object = NULL;
            root         = NULL;
            return NULL;
        } else {
            return root;
        }
    } else {
        fprintf(stderr,
                "json error on line %d (col %d): %s\n",
                error.line,
                error.column,
                error.text);
        fprintf(stderr, "%d\n%s\n", error.position, error.source);

        json_decref(root);
        first_object = NULL;
        root         = NULL;
    }

    return NULL;
}

int json_init_scenario(struct scenario_class *scenario, FILE *init_fd) {
    json_t *root = NULL;
    json_t *json = NULL;
    json_error_t error;

    root = json_loadf(init_fd, 0, &error);

    if (!root) {
        return -1;
    }

    json = json_object_get(root, key_init);
    if (!json) {
        WARNING("json_object_get");
        return -1;
    }

    json_t *j_nodes = NULL;
    json_t *j_envs  = NULL;
    json_t *j_conns = NULL;

    j_nodes = json_object_get(json, "node");
    if (!j_nodes) {
        WARNING("json_object_get node");
        return ERROR;
    }

    if (parse_node(scenario, j_nodes) == ERROR) {
        WARNING("parse_node");
        return ERROR;
    }

    j_envs = json_object_get(json, "environment");
    if (!j_envs) {
        WARNING("json_object_get environment");
        return ERROR;
    }
    if (parse_environment(scenario, j_envs) == ERROR) {
        WARNING("parse_connection");
        return ERROR;
    }

    j_conns = json_object_get(json, "connection");
    if (!j_conns) {
        WARNING("json_object_get connection");
        return ERROR;
    }
    if (parse_connection(scenario, j_conns) == ERROR) {
        WARNING("parse_connection");
        return ERROR;
    }

    j_nodes = NULL;
    j_envs  = NULL;
    j_conns = NULL;
    json    = NULL;

    json_decref(root);
    root = NULL;

    return SUCCESS;
}

int update_all_neighbors(struct scenario_class *scenario,
                         struct connection_class **neighbors,
                         int *center_id,
                         int **neighbor_ids,
                         input_buffer_t *ibuf,
                         int type) {

    // json_t *root = NULL;
    // json_t *json = NULL;

    struct msg *recv_msg = read_msg(ibuf);

    // root = read_json(ibuf);
    if (!recv_msg) {
        return -1;
    }

    // if (type == 0) {
    //     json = json_object_get(root, key_init);
    // } else {
    //     json = json_object_get(root, key_update);
    // }
    // if (!json) {
    //     WARNING("json_object_get");
    //     return -1;
    // }

    // json_t *node_object;
    // json_t *center;
    // json_t *neighbor;

    struct node_class *nodes;
    struct node_class *node;
    int own_id;

    int node_id;
    int neighbor_array_len;

    int neighbor_number;
    int nn;
    int conn_i;
    int nb_id;

    int ni;

    nodes  = scenario->nodes;
    own_id = scenario->own_id;

    // int same_neighbor = 0;
    // int neighbor_flag = 1;

    neighbor_number = 0;
    nn              = 0;
    ni              = 0;

    // json_t *update_json = NULL;

    // update_json = json_object_get(json, "neighbors");

    // if (!update_json) {
    //     WARNING("json_object_get");
    //     return -1;
    // }
    // nodes_array_len = json_array_size(update_json);

    // for (int i = 0; i < nodes_array_len; i++) {
    // node_object = json_object_get(update_json, i);

    // node_object = json_array_get(nodes_array, 0);
    // center = json_object_get(update_json, "center");

    // *center_id = json_integer_value(json_object_get(center, "id"));

    *center_id = recv_msg->id;
    // printf("center_id=%d\n", *center_id);

    // if (-1 < own_id && *center_id != own_id) {
    //     return -1;
    // } else {
    //     printf("match %d\n", *center_id);
    //     *center_id = 0;
    // }

    node = &nodes[*center_id];

    // node->position.c[0] = json_number_value(json_object_get(center, "x"));
    // node->position.c[1] = json_number_value(json_object_get(center, "y"));
    node->position.c[0] = recv_msg->x;
    node->position.c[1] = recv_msg->y;

    // json_decref(center);
    // center = NULL;
    // }

    // neighbor = json_object_get(update_json, "neighbor");

    // neighbor_array_len = json_array_size(neighbor);
    neighbor_array_len = recv_msg->neighbor_size;

    // json_t *nei;

    nn = 0;
    for (int j = 0; j < neighbor_array_len; j++) {
        // nei     = json_array_get(neighbor, j);
        // node_id = json_integer_value(nei);

        node_id = recv_msg->neighbor[j];
        // if (node_id < 0) {
        // neighbor_ids[*center_id][0] = -1;
        // break;
        // }

        neighbor_ids[*center_id][nn++] = node_id;

        // json_decref(nei);
        // nei = NULL;
    }

    // json_decref(neighbor);
    // neighbor = NULL;
    // json_decref(node_object);
    // node_object = NULL;

    neighbor_ids[*center_id][neighbor_array_len] = -1;

    if (neighbor_ids[*center_id][0] == -1) {
        return 0;
    }

    for (int j = 0; j < nn; j++) {
        // same_neighbor = 0;
        nb_id = neighbor_ids[*center_id][j];
        if (own_id != -1 && *center_id != own_id && nb_id != own_id) {
            continue;
        }

        if (*center_id < nb_id) {
            --nb_id;
        }
        conn_i = *center_id * (scenario->node_number - 1) + nb_id;

        neighbors[ni++] = &scenario->connections[conn_i];
        ++neighbor_number;

        // if (neighbor_ids[*center_id][j] == *center_id) {
        //     WARNING("nn?");
        //     same_neighbor = 1;
        //     break;
        // }
        // if (neighbor_flag && !same_neighbor) {
        // WARNING("nn?");

        nb_id = *center_id;
        if (neighbor_ids[*center_id][j] < nb_id) {
            --nb_id;
        }
        conn_i =
            neighbor_ids[*center_id][j] * (scenario->node_number - 1) + nb_id;
        neighbors[ni++] = &scenario->connections[conn_i];
        ++neighbor_number;

        // connection_print(&scenario->connections[conn_i]);
        // connection_print(neighbors[ni - 1]);
        // }
    }
    // }

    // *center_id = own_id;
    // json_decref(update_json);
    // json_decref(node_object);
    // json_decref(center);
    // json_decref(neighbor);
    // json_decref(nei);
    // json_decref(json);
    // printf("%lu\n", json->refcount);
    // printf("update->refcount=%zu\n", update_json->refcount);
    // printf("json->refcount=%zu\n", json->refcount);
    // printf("root->refcount=%zu\n", root->refcount);
    // json_decref(update_json);
    // json_decref(json);
    // json_decref(root);
    // printf("update->refcount=%zu\n", update_json->refcount);
    // printf("json->refcount=%zu\n", json->refcount);
    // printf("root->refcount=%zu\n", root->refcount);

    // update_json = NULL;
    // json        = NULL;
    // root        = NULL;

    INFO("neighbor_number=%d\n", neighbor_number);

    return neighbor_number;
}
int update_neighbors(struct scenario_class *scenario,
                     struct connection_class **neighbors,
                     int own_id,
                     int *neighbor_ids,
                     input_buffer_t *ibuf,
                     int *is_other_update,
                     int *is_other_delete,
                     // int *is_other,
                     // int *is_contain_other,
                     int *received_center_id,
                     int type) {

    // json_t *root = NULL;
    // json_t *json = NULL;

    struct msg *recv_msg = read_msg(ibuf);

    // root = read_json(ibuf);
    if (!recv_msg) {
        return -1;
    }

    // if (type == 0) {
    //     json = json_object_get(root, key_init);
    // } else {
    //     json = json_object_get(root, key_update);
    // }
    // if (!json) {
    //     WARNING("json_object_get");
    //     return -1;
    // }

    // json_t *center;
    // json_t *neighbor;

    struct node_class *nodes;
    struct node_class *node;

    int node_id   = -1;
    int center_id = -1;
    int neighbor_array_len;

    int neighbor_number;
    int nn;
    int conn_i;
    int nb_id;

    int ni;

    nodes  = scenario->nodes;
    own_id = scenario->own_id;

    neighbor_number = 0;
    nn              = 0;
    ni              = 0;

    // json_t *update_json = NULL;

    // update_json = json_object_get(json, "neighbors");

    // if (!update_json) {
    //     WARNING("json_object_get");
    //     return -1;
    // }
    // nodes_array_len = json_array_size(update_json);

    // for (int i = 0; i < nodes_array_len; i++) {
    // node_object = json_object_get(update_json, i);

    // node_object = json_array_get(nodes_array, 0);
    // center = json_object_get(update_json, "center");
    // center_id = json_integer_value(json_object_get(center, "id"));
    center_id = recv_msg->id;

    // fprintf(stderr, "center_id=%d\n", center_id);

    // else {
    //     printf("match %d\n", center_id);
    // }

    // =====================================================
    // ノードの位置更新
    // =====================================================
    node = &nodes[center_id];
    // node->position.c[0] = json_number_value(json_object_get(center, "x"));
    // node->position.c[1] = json_number_value(json_object_get(center, "y"));

    node->position.c[0] = recv_msg->x;
    node->position.c[1] = recv_msg->y;

    // json_decref(center);
    // center = NULL;
    // }

    // neighbor = json_object_get(update_json, "neighbor");

    // neighbor_array_len = json_array_size(neighbor);
    neighbor_array_len = recv_msg->neighbor_size;

    // json_t *nei;

    // =====================================================
    // neighbor_idsにneigborをセット
    // =====================================================
    nn              = 0;
    neighbor_ids[0] = -1;
    for (int j = 0; j < neighbor_array_len; j++) {
        // nei     = json_array_get(neighbor, j);
        // node_id = json_integer_value(nei);
        node_id = recv_msg->neighbor[j];

        // if (node_id < 0) {
        //     break;
        // }

        neighbor_ids[nn++] = node_id;

        // json_decref(nei);
        // nei = NULL;
    }
    neighbor_ids[neighbor_array_len] = -1;

    // json_decref(neighbor);
    // neighbor = NULL;
    // json_decref(node_object);
    // node_object = NULL;

    // =====================================================
    // center_idかneighbor_idsにown_idが含まれれば
    // neighborsに対象のconnectionポインタをセット
    // =====================================================

    // TODO 他のノードが追加・削除・した場合についての処理を考えること
    //
    // center_idがown_idならば
    // neighbor_idsのconnectionをセット
    // int *is_other         = 0;
    // int *is_contain_other = 0;
    *is_other_update = 0;
    *is_other_delete = 0;
    if (center_id == own_id) {
        *received_center_id = -1;
        // if (neighbor_ids[0] == -1) {
        //     *is_other_delete = 1;
        //     neighbor_ids[0]  = center_id;
        //     neighbor_ids[1]  = -1;
        //     return 0;
        // }
        if (neighbor_ids[0] != -1) {
            for (int i = 0; i < nn; i++) {
                nb_id = neighbor_ids[i];

                if (own_id < nb_id) {
                    --nb_id;
                }

                conn_i = nb_id;

                // printf("own: conn_i=%d\n", conn_i);

                if (conn_i < 0) {
                    WARNING("invalid neighbor");
                    break;
                }
                neighbors[ni++] = &scenario->connections[conn_i];
                // neighbors[ni++] = &scenario->connections[conn_i + 1];
                ++neighbor_number;
            }
        }
    } else {
        // *is_other = 1;
        // for (int i = 0; i < nn; i++) {
        //     nb_id = neighbor_ids[i];
        //     if (own_id == nb_id) {
        //         nb_id = center_id;
        //         if (own_id < nb_id) {
        //             --nb_id;
        //         }
        //         conn_i = nb_id;
        //         printf("conn_i=%d\n", conn_i);

        //         neighbors[ni++] = &scenario->connections[conn_i];
        //         // neighbors[ni++] = &scenario->connections[conn_i + 1];
        //         neighbor_number++;
        //         break;
        //     }
        // }
        for (int i = 0; i < nn; i++) {
            nb_id = neighbor_ids[i];
            if (own_id == nb_id) {
                // *is_contain_other   = 1;
                *is_other_update    = 1;
                *received_center_id = center_id;
                nb_id               = center_id;
                if (own_id < nb_id) {
                    --nb_id;
                }

                conn_i = nb_id;
                // printf("other: conn_i=%d\n", conn_i);

                neighbors[0] = &scenario->connections[conn_i];
                // neighbors[ni++] = &scenario->connections[conn_i + 1];
                neighbor_number = 1;
                // break;
                neighbor_ids[0] = center_id;
                neighbor_ids[1] = -1;
                break;
            }
        }

        if (*is_other_update == 0) {
            *is_other_delete    = 1;
            *received_center_id = center_id;
            // is_contain_other = 0;
            neighbor_ids[0] = center_id;
            neighbor_ids[1] = -1;
        }
    }
    // for (int j = 0; j < nn; j++) {
    //     same_neighbor = 0;
    //     nb_id         = neighbor_ids[j];
    //     if (nb_id != own_id) {
    //         continue;
    //     }

    //     if (own_id < nb_id) {
    //         nb_id--;
    //     }
    //     conn_i = own_id * (scenario->node_number - 1) + nb_id;

    //     neighbors[ni++] = &scenario->connections[conn_i];
    //     neighbor_number++;

    //     if (neighbor_ids[j] == own_id) {
    //         same_neighbor = 1;
    //         break;
    //     }
    //     if (neighbor_flag && !same_neighbor) {

    //         nb_id = own_id;
    //         if (neighbor_ids[j] < nb_id) {
    //             nb_id--;
    //         }
    //         conn_i = neighbor_ids[j] * (scenario->node_number - 1) + nb_id;
    //         neighbors[ni++] = &scenario->connections[conn_i];
    //         neighbor_number++;

    //         // connection_print(&scenario->connections[conn_i]);
    //         // connection_print(neighbors[ni - 1]);
    //     }
    // }
    // }

    // *center_id = own_id;
    // json_decref(update_json);
    // json_decref(node_object);
    // json_decref(center);
    // json_decref(neighbor);
    // json_decref(nei);
    // json_decref(json);
    // printf("%lu\n", json->refcount);
    // printf("update->refcount=%zu\n", update_json->refcount);
    // printf("json->refcount=%zu\n", json->refcount);
    // printf("root->refcount=%zu\n", root->refcount);
    // json_decref(update_json);
    // json_decref(json);
    // json_decref(root);
    // printf("update->refcount=%zu\n", update_json->refcount);
    // printf("json->refcount=%zu\n", json->refcount);
    // printf("root->refcount=%zu\n", root->refcount);

    // update_json = NULL;
    // json        = NULL;
    // root        = NULL;

    INFO("neighbor_number=%d\n", neighbor_number);

    return neighbor_number;
}

int search_all_connection(struct connection_class **conn_pair,
                          int from_id,
                          int to_id,
                          struct connection_class *connections,
                          int node_number) {
    int conn_i;
    int nb_id;
    nb_id = to_id;

    if (from_id < nb_id) {
        --nb_id;
    }
    conn_i = from_id * (node_number - 1) + nb_id;

    conn_pair[0] = &connections[conn_i];

    nb_id = from_id;
    if (to_id < nb_id) {
        --nb_id;
    }
    conn_i       = to_id * (node_number - 1) + nb_id;
    conn_pair[1] = &connections[conn_i];

    return 0;
}
int search_connection(struct connection_class **conn_pair,
                      int from_id,
                      int to_id,
                      struct connection_class *connections) {
    int conn_i;
    int nb_id;
    nb_id = to_id;

    // printf("from=%d to=%d\n", from_id, to_id);

    if (from_id < nb_id) {
        --nb_id;
    }
    conn_i = nb_id * 2;

    conn_pair[0] = &connections[conn_i];
    conn_pair[1] = &connections[conn_i + 1];

    return 0;
}
int send_all_update_json(int center_id,
                         meteor_param_t *mp,
                         int node_number,
                         struct connection_class *connections,
                         int is_broadcast,
                         bc_info_t *info) {

    struct connection_class *conn_pair[2];
    struct connection_class *rx;
    struct connection_class *tx;

    int from_id;
    int to_id;

    from_id = center_id;
    for (int i = 0; mp->update[i] != -1; i++) {
        // json_t *json;
        // json_t *update;
        // json_t *pair;

        // json_t *from, *to;

        // json   = json_object();
        // update = json_object();
        // pair   = json_array();

        // from = json_object();
        // to   = json_object();

        to_id = mp->update[i];
        search_all_connection(
            conn_pair, from_id, to_id, connections, node_number);

        tx = conn_pair[0];
        rx = conn_pair[1];

        // tx->frame_error_rate    = round(tx->frame_error_rate * 1e6) / 1e6;
        // tx->num_retransmissions = round(tx->num_retransmissions * 1e6) /
        // 1e6; tx->bandwidth           = round(tx->bandwidth * 1e6) / 1e6;
        // tx->loss_rate           = round(tx->loss_rate * 1e6) / 1e6;
        // tx->delay               = round(tx->delay * 1e6) / 1e6;

        // rx->frame_error_rate    = round(rx->frame_error_rate * 1e6) / 1e6;
        // rx->num_retransmissions = round(rx->num_retransmissions * 1e6) /
        // 1e6; rx->bandwidth           = round(rx->bandwidth * 1e6) / 1e6;
        // rx->loss_rate           = round(rx->loss_rate * 1e6) / 1e6;
        // rx->delay               = round(rx->delay * 1e6) / 1e6;

        // json_object_set(from, "from_id",
        // json_integer(tx->from_node_index)); json_object_set(from,
        // "to_id", json_integer(tx->to_node_index));
        // json_object_set(from, "fer", json_real(tx->frame_error_rate));
        // json_object_set(from, "num_retr",
        // json_real(tx->num_retransmissions)); json_object_set(from,
        // "standard", json_integer(tx->standard)); json_object_set(
        //     from, "op_rate",
        //     json_integer(connection_get_operating_rate(tx)));
        // json_object_set(from, "bandwidth", json_real(tx->bandwidth));
        // json_object_set(from, "loss_rate", json_real(tx->loss_rate));
        // json_object_set(from, "delay", json_real(tx->delay));

        // json_object_set(to, "from_id",
        // json_integer(rx->from_node_index)); json_object_set(to,
        // "to_id", json_integer(rx->to_node_index)); json_object_set(to,
        // "fer", json_real(rx->frame_error_rate)); json_object_set(to,
        // "num_retr", json_real(rx->num_retransmissions));
        // json_object_set(to, "standard", json_integer(rx->standard));
        // json_object_set(
        //     to, "op_rate",
        //     json_integer(connection_get_operating_rate(rx)));
        // json_object_set(to, "bandwidth", json_real(rx->bandwidth));
        // json_object_set(to, "loss_rate", json_real(rx->loss_rate));
        // json_object_set(to, "delay", json_real(rx->delay));

        // json_array_append(pair, from);
        // json_array_append(pair, to);

        // json_object_set(update, "update", pair);
        // json_object_set(json, "meteor", update);

        // info->msg = json_dumps(json, JSON_COMPACT |
        // JSON_REAL_PRECISION(10)); info->msg_len = strlen(info->msg);

        // if (broadcast_sendmsg(info) == ERROR) {
        //     WARNING("broadcast_sender");
        //     return ERROR;
        // }

        // printf("%s\n", info->msg);

        // free(info->msg);
        // info->msg = NULL;

        // while (0 < to->refcount) json_decref(to);
        // while (0 < from->refcount) json_decref(from);
        // while (0 < pair->refcount) json_decref(pair);
        // while (0 < update->refcount) json_decref(update);
        // while (0 < json->refcount) json_decref(json);

        TCHK_START(snprintf);
        info->msg_len = sprintf(
            &send_json_update_buf[update_buf_write_pos],
            // SEND_JSON_MAXLEN,
            // "{\"meteor\":{\"update\":[{\"from_id\":"
            "%d,\"to_id\":%d,"
            "\"fer\":%.6f,\"num_retr\":%.6f,\"standard\":%d,\"op_rate\":"
            "%.6f,\"bandwidth\":%.6f,\"loss_rate\":%.6f,\"delay\":%.6f},{"
            "\"from_id\":%d,\"to_id\":%d,\"fer\":%.6f,\"num_retr\":%.6f,"
            "\"standard\":%d,\"op_rate\":%.6f,\"bandwidth\":%.6f,\"loss_"
            "rate\":%.6f,\"delay\":%.6f}]}}",
            tx->from_node_index,
            tx->to_node_index,
            tx->frame_error_rate,
            tx->num_retransmissions,
            tx->standard,
            connection_get_operating_rate(tx),
            tx->bandwidth,
            tx->loss_rate,
            tx->delay,
            rx->from_node_index,
            rx->to_node_index,
            rx->frame_error_rate,
            rx->num_retransmissions,
            rx->standard,
            connection_get_operating_rate(rx),
            rx->bandwidth,
            rx->loss_rate,
            rx->delay);

        // info->msg = json_dumps(json, JSON_COMPACT |
        // JSON_REAL_PRECISION(10));
        info->msg = send_json_update_buf;
        // info->msg     = send_json_buf;
        // info->msg_len = strlen(info->msg);

        TCHK_END(snprintf);
#if !defined(MEASURE) && !defined(MEASURE2)
        printf("--len(%lu) %s\n", info->msg_len, info->msg);
#endif

        if (is_broadcast && broadcast_sendmsg(info) == ERROR) {
            WARNING("broadcast_sender");
            return ERROR;
        }
    }
    return SUCCESS;
}
int send_update_json(int own_id,
                     meteor_param_t *mp,
                     struct connection_class *connections,
                     int is_broadcast,
                     bc_info_t *info) {

    // struct connection_class *conn_pair[2];
    // struct connection_class *rx;
    struct connection_class *tx;

    int from_id;
    int to_id;
    int conn_i;

    from_id = own_id;
    conn_i  = from_id;
    for (int i = 0; mp->update[i] != -1; i++) {
        to_id  = mp->update[i];
        conn_i = to_id;
        // search_connection(conn_pair, from_id, to_id, connections);

        // tx = conn_pair[0];
        // rx = conn_pair[1];

        if (from_id < to_id) {
            --conn_i;
        }

        tx = &connections[conn_i];

        // tx->frame_error_rate    = round(tx->frame_error_rate * 1e6) / 1e6;
        // tx->num_retransmissions = round(tx->num_retransmissions * 1e6) /
        // 1e6; tx->bandwidth           = round(tx->bandwidth * 1e6) / 1e6;
        // tx->loss_rate           = round(tx->loss_rate * 1e6) / 1e6;
        // tx->delay               = round(tx->delay * 1e6) / 1e6;

        // rx->frame_error_rate    = round(rx->frame_error_rate * 1e6) / 1e6;
        // rx->num_retransmissions = round(rx->num_retransmissions * 1e6) /
        // 1e6; rx->bandwidth           = round(rx->bandwidth * 1e6) / 1e6;
        // rx->loss_rate           = round(rx->loss_rate * 1e6) / 1e6;
        // rx->delay               = round(rx->delay * 1e6) / 1e6;

        // snprintf(
        //     send_json_buf,
        //     SEND_JSON_MAXLEN,
        //     "{\"meteor\":{\"update\":[{\"from_id\":%d,\"to_id\":%d,"
        //     "\"fer\":%.6f,\"num_retr\":%.6f,\"standard\":%d,\"op_rate\":"
        //     "%.6f,\"bandwidth\":%.6f,\"loss_rate\":%.6f,\"delay\":%.6f},{"
        //     "\"from_id\":%d,\"to_id\":%d,\"fer\":%.6f,\"num_retr\":%.6f,"
        //     "\"standard\":%d,\"op_rate\":%.6f,\"bandwidth\":%.6f,\"loss_"
        //     "rate\":%.6f,\"delay\":%.6f}]}}",
        //     tx->from_node_index,
        //     tx->to_node_index,
        //     tx->frame_error_rate,
        //     tx->num_retransmissions,
        //     tx->standard,
        //     connection_get_operating_rate(tx),
        //     tx->bandwidth,
        //     tx->loss_rate,
        //     tx->delay,
        //     rx->from_node_index,
        //     rx->to_node_index,
        //     rx->frame_error_rate,
        //     rx->num_retransmissions,
        //     rx->standard,
        //     connection_get_operating_rate(rx),
        //     rx->bandwidth,
        //     rx->loss_rate,
        //     rx->delay);
        info->msg_len = sprintf(
            // send_json_buf,
            // SEND_JSON_MAXLEN,
            &send_json_update_buf[update_buf_write_pos],
            // "{\"meteor\":{\"update\":[{\"from_id\":"
            "%d,\"to_id\":%d,"
            "\"fer\":%.6f,\"num_retr\":%.6f,\"standard\":%d,\"op_rate\":"
            "%.6f,\"bandwidth\":%.6f,\"loss_rate\":%.6f,\"delay\":%.6f}]}}",
            tx->from_node_index,
            tx->to_node_index,
            tx->frame_error_rate,
            tx->num_retransmissions,
            tx->standard,
            connection_get_operating_rate(tx),
            tx->bandwidth,
            tx->loss_rate,
            tx->delay);

        info->msg = send_json_update_buf;
        // info->msg_len = strlen(info->msg);

#if !defined(MEASURE) && !defined(MEASURE2)
        printf("--len(%lu) %s\n", info->msg_len, info->msg);
#endif

        if (is_broadcast && broadcast_sendmsg(info) == ERROR) {
            WARNING("broadcast_sender");
            return ERROR;
        }
    }
    return SUCCESS;
}
int send_all_add_json(int center_id,
                      meteor_param_t *mp,
                      int node_number,
                      struct connection_class *connections,
                      int is_broadcast,
                      bc_info_t *info) {

    struct connection_class *conn_pair[2];
    struct connection_class *rx;
    struct connection_class *tx;

    int from_id;
    int to_id;

    from_id = center_id;
    for (int i = 0; mp->add[i] != -1; i++) {

        // json_t *json;
        // json_t *add;
        // json_t *pair;
        // json_t *from, *to;

        // json = json_object();
        // add  = json_object();
        // pair = json_array();

        // from = json_object();
        // to   = json_object();

        to_id = mp->add[i];
        search_all_connection(
            conn_pair, from_id, to_id, connections, node_number);

        tx = conn_pair[0];
        rx = conn_pair[1];

        // tx->frame_error_rate    = round(tx->frame_error_rate * 1e6) / 1e6;
        // tx->num_retransmissions = round(tx->num_retransmissions * 1e6) /
        // 1e6; tx->bandwidth           = round(tx->bandwidth * 1e6) / 1e6;
        // tx->loss_rate           = round(tx->loss_rate * 1e6) / 1e6;
        // tx->delay               = round(tx->delay * 1e6) / 1e6;

        // rx->frame_error_rate    = round(rx->frame_error_rate * 1e6) / 1e6;
        // rx->num_retransmissions = round(rx->num_retransmissions * 1e6) /
        // 1e6; rx->bandwidth           = round(rx->bandwidth * 1e6) / 1e6;
        // rx->loss_rate           = round(rx->loss_rate * 1e6) / 1e6;
        // rx->delay               = round(rx->delay * 1e6) / 1e6;

        // json_object_set(from, "from_id",
        // json_integer(tx->from_node_index)); json_object_set(from,
        // "to_id", json_integer(tx->to_node_index));
        // json_object_set(from, "fer", json_real(tx->frame_error_rate));
        // json_object_set(from, "num_retr",
        // json_real(tx->num_retransmissions)); json_object_set(from,
        // "standard", json_integer(tx->standard)); json_object_set(
        //     from, "op_rate",
        //     json_integer(connection_get_operating_rate(tx)));
        // json_object_set(from, "bandwidth", json_real(tx->bandwidth));
        // json_object_set(from, "loss_rate", json_real(tx->loss_rate));
        // json_object_set(from, "delay", json_real(tx->delay));

        // json_object_set(to, "from_id",
        // json_integer(rx->from_node_index)); json_object_set(to,
        // "to_id", json_integer(rx->to_node_index)); json_object_set(to,
        // "fer", json_real(rx->frame_error_rate)); json_object_set(to,
        // "num_retr", json_real(rx->num_retransmissions));
        // json_object_set(to, "standard", json_integer(rx->standard));
        // json_object_set(
        //     to, "op_rate",
        //     json_integer(connection_get_operating_rate(rx)));
        // json_object_set(to, "bandwidth", json_real(rx->bandwidth));
        // json_object_set(to, "loss_rate", json_real(rx->loss_rate));
        // json_object_set(to, "delay", json_real(rx->delay));

        // json_array_append(pair, from);
        // json_array_append(pair, to);

        // json_object_set(add, "add", pair);
        // json_object_set(json, "meteor", add);

        info->msg_len = sprintf(
            &send_json_add_buf[add_buf_write_pos],
            // send_json_buf,
            // SEND_JSON_MAXLEN,
            // "{\"meteor\":{\"add\":[{\"from_id\":"
            "%d,\"to_id\":%d,"
            "\"fer\":%.6f,\"num_retr\":%.6f,\"standard\":%d,\"op_rate\":"
            "%.6f,\"bandwidth\":%.6f,\"loss_rate\":%.6f,\"delay\":%.6f},{"
            "\"from_id\":%d,\"to_id\":%d,\"fer\":%.6f,\"num_retr\":%.6f,"
            "\"standard\":%d,\"op_rate\":%.6f,\"bandwidth\":%.6f,\"loss_"
            "rate\":%.6f,\"delay\":%.6f}]}}",
            tx->from_node_index,
            tx->to_node_index,
            tx->frame_error_rate,
            tx->num_retransmissions,
            tx->standard,
            connection_get_operating_rate(tx),
            tx->bandwidth,
            tx->loss_rate,
            tx->delay,
            rx->from_node_index,
            rx->to_node_index,
            rx->frame_error_rate,
            rx->num_retransmissions,
            rx->standard,
            connection_get_operating_rate(rx),
            rx->bandwidth,
            rx->loss_rate,
            rx->delay);

        // info->msg = json_dumps(json, JSON_COMPACT |
        // JSON_REAL_PRECISION(10));
        info->msg = send_json_add_buf;
        // info->msg_len = strlen(info->msg);

#if !defined(MEASURE) && !defined(MEASURE2)
        printf("--len(%lu) %s\n", info->msg_len, info->msg);
#endif
        if (is_broadcast && broadcast_sendmsg(info) == ERROR) {
            WARNING("broadcast_sender");
            return ERROR;
        }

        // while (0 < to->refcount) json_decref(to);
        // while (0 < from->refcount) json_decref(from);
        // while (0 < pair->refcount) json_decref(pair);
        // while (0 < add->refcount) json_decref(add);
        // while (0 < json->refcount) json_decref(json);
    }
    return SUCCESS;
}
int send_add_json(int own_id,
                  meteor_param_t *mp,
                  struct connection_class *connections,
                  int is_broadcast,
                  bc_info_t *info) {

    // struct connection_class *conn_pair[2];
    // struct connection_class *rx;
    struct connection_class *tx;

    int from_id;
    int to_id;
    int conn_i;

    from_id = own_id;
    for (int i = 0; mp->add[i] != -1; i++) {

        to_id  = mp->add[i];
        conn_i = to_id;
        // search_connection(conn_pair, from_id, to_id, connections);

        // tx = conn_pair[0];
        // rx = conn_pair[1];

        if (from_id < to_id) {
            --conn_i;
        }
        tx = &connections[conn_i];

        // tx->frame_error_rate    = tx->frame_error_rate;
        // tx->num_retransmissions = tx->num_retransmissions;
        // tx->bandwidth           = tx->bandwidth;
        // tx->loss_rate           = tx->loss_rate;
        // tx->delay               = tx->delay;

        // tx->frame_error_rate    = round(tx->frame_error_rate * 1e6) / 1e6;
        // tx->num_retransmissions = round(tx->num_retransmissions * 1e6) /
        // 1e6; tx->bandwidth           = round(tx->bandwidth * 1e6) / 1e6;
        // tx->loss_rate           = round(tx->loss_rate * 1e6) / 1e6;
        // tx->delay               = round(tx->delay * 1e6) / 1e6;

        // rx->frame_error_rate    = round(rx->frame_error_rate * 1e6) / 1e6;
        // rx->num_retransmissions = round(rx->num_retransmissions * 1e6) /
        // 1e6; rx->bandwidth           = round(rx->bandwidth * 1e6) / 1e6;
        // rx->loss_rate           = round(rx->loss_rate * 1e6) / 1e6;
        // rx->delay               = round(rx->delay * 1e6) / 1e6;

        info->msg_len = sprintf(
            &send_json_add_buf[add_buf_write_pos],
            // send_json_buf,
            // SEND_JSON_MAXLEN,
            // "{\"meteor\":{\"add\":[{\"from_id\":"
            "%d,\"to_id\":%d,"
            "\"fer\":%.6f,\"num_retr\":%.6f,\"standard\":%d,\"op_rate\":"
            "%.6f,\"bandwidth\":%.6f,\"loss_rate\":%.6f,\"delay\":%.6f}]}}",
            tx->from_node_index,
            tx->to_node_index,
            tx->frame_error_rate,
            tx->num_retransmissions,
            tx->standard,
            connection_get_operating_rate(tx),
            tx->bandwidth,
            tx->loss_rate,
            tx->delay);

        info->msg = send_json_add_buf;
        // info->msg_len = strlen(info->msg);

#if !defined(MEASURE) && !defined(MEASURE2)
        printf("--len(%lu) %s\n", info->msg_len, info->msg);
#endif
        if (is_broadcast && broadcast_sendmsg(info) == ERROR) {
            WARNING("broadcast_sender");
            return ERROR;
        }
    }
    return SUCCESS;
}
int send_delete_json(int center_id,
                     meteor_param_t *mp,
                     int is_broadcast,
                     bc_info_t *info) {

    int from_id;
    int to_id;

    from_id = center_id;

    for (int i = 0; mp->delete[i] != -1; i++) {
        // json_t *json;
        // json_t *delete;
        // json_t *pair;
        // json_t *p0, *p1;

        // json   = json_object();
        // delete = json_object();
        // pair   = json_array();

        to_id = mp->delete[i];

        // p0 = json_object();
        // p1 = json_object();

        // json_object_set(p0, "from_id", json_integer(from_id));
        // json_object_set(p0, "to_id", json_integer(to_id));

        // json_object_set(p1, "from_id", json_integer(to_id));
        // json_object_set(p1, "to_id", json_integer(from_id));

        // json_array_append(pair, p0);
        // json_array_append(pair, p1);

        // json_object_set(delete, "delete", pair);
        // json_object_set(json, "meteor", delete);

        // info->msg = json_dumps(json, JSON_COMPACT |
        // JSON_REAL_PRECISION(10));

        info->msg_len = sprintf(&send_json_del_buf[delete_buf_write_pos],
                                // SEND_JSON_MAXLEN,
                                // "{\"meteor\":{\"delete\":[{\"from_id\":"
                                // "%d}]}}",
                                "%d,\"to_id\":%d}]}}",
                                from_id,
                                to_id);
        // to_id);

        info->msg = send_json_del_buf;
        // info->msg_len = strlen(info->msg);

#if !defined(MEASURE) && !defined(MEASURE2)
        printf("--len(%lu) %s\n", info->msg_len, info->msg);
#endif

        if (is_broadcast && broadcast_sendmsg(info) == ERROR) {
            WARNING("broadcast_sender");
            return ERROR;
        }

        // free(info->msg);
        // info->msg = NULL;

        // while (0 < p0->refcount) json_decref(p0);
        // while (0 < p1->refcount) json_decref(p1);
        // while (0 < pair->refcount) json_decref(pair);
        // while (0 < delete->refcount) json_decref(delete);
        // while (0 < json->refcount) json_decref(json);
    }

    return SUCCESS;
}

int send_all_delete_json(int center_id,
                         meteor_param_t *mp,
                         int is_broadcast,
                         bc_info_t *info) {

    int from_id;
    int to_id;

    from_id = center_id;

    for (int i = 0; mp->delete[i] != -1; i++) {
        // json_t *json;
        // json_t *delete;
        // json_t *pair;
        // json_t *p0, *p1;

        // json   = json_object();
        // delete = json_object();
        // pair   = json_array();

        to_id = mp->delete[i];

        // p0 = json_object();
        // p1 = json_object();

        // json_object_set(p0, "from_id", json_integer(from_id));
        // json_object_set(p0, "to_id", json_integer(to_id));

        // json_object_set(p1, "from_id", json_integer(to_id));
        // json_object_set(p1, "to_id", json_integer(from_id));

        // json_array_append(pair, p0);
        // json_array_append(pair, p1);

        // json_object_set(delete, "delete", pair);
        // json_object_set(json, "meteor", delete);

        // info->msg = json_dumps(json, JSON_COMPACT |
        // JSON_REAL_PRECISION(10));

        // snprintf(send_json_buf,
        //          SEND_JSON_MAXLEN,
        info->msg_len = sprintf(&send_json_del_buf[delete_buf_write_pos],
                                // "{\"meteor\":{\"delete\":[{\"from_id\":"
                                "%d,\"to_id\":%d},{"
                                // "%d,\"to_id\":%d}]}}",
                                "\"from_id\":%d,\"to_id\":%d}]}}",
                                // "%d}]}}",
                                from_id,
                                to_id,
                                to_id,
                                from_id);

        info->msg = send_json_del_buf;
        // info->msg_len = strlen(info->msg);

#if !defined(MEASURE) && !defined(MEASURE2)
        printf("--len(%lu) %s\n", info->msg_len, info->msg);
#endif

        if (is_broadcast && broadcast_sendmsg(info) == ERROR) {
            WARNING("broadcast_sender");
            return ERROR;
        }

        // free(info->msg);
        // info->msg = NULL;

        // while (0 < p0->refcount) json_decref(p0);
        // while (0 < p1->refcount) json_decref(p1);
        // while (0 < pair->refcount) json_decref(pair);
        // while (0 < delete->refcount) json_decref(delete);
        // while (0 < json->refcount) json_decref(json);
    }

    return SUCCESS;
}
char *send_all_json_message(int center_id,
                            meteor_param_t *mp,
                            int node_number,
                            struct connection_class *connections,
                            int type,
                            int is_broadcast,
                            bc_info_t *info) {

    switch (type) {
        case 0:
            if (!mp->is_change_add) {
                return NULL;
            }
            if (send_all_add_json(center_id,
                                  mp,
                                  node_number,
                                  connections,
                                  is_broadcast,
                                  info) == ERROR)
                return NULL;
            break;
        case 1:
            if (!mp->is_change_update) {
                return NULL;
            }
            if (send_all_update_json(center_id,
                                     mp,
                                     node_number,
                                     connections,
                                     is_broadcast,
                                     info) == ERROR)
                return NULL;
            break;
        case 2:
            if (!mp->is_change_delete) {
                return NULL;
            }
            if (send_all_delete_json(center_id, mp, is_broadcast, info) ==
                ERROR)
                return NULL;
            break;
    }

    return SUCCESS;
}
char *send_json_message(int own_id,
                        meteor_param_t *mp,
                        struct connection_class *connections,
                        int type,
                        int is_broadcast,
                        bc_info_t *info) {

    switch (type) {
        case 0:
            if (!mp->is_change_add) {
                return NULL;
            }
            if (send_add_json(own_id, mp, connections, is_broadcast, info) ==
                ERROR)
                return NULL;
            break;
        case 1:
            if (!mp->is_change_update) {
                return NULL;
            }
            if (send_update_json(
                    own_id, mp, connections, is_broadcast, info) == ERROR)
                return NULL;
            break;
        case 2:
            if (!mp->is_change_delete) {
                return NULL;
            }
            if (send_delete_json(own_id, mp, is_broadcast, info) == ERROR)
                return NULL;
            break;
    }

    return SUCCESS;
}
int send_all_result_to_meteor(meteor_param_t *mp,
                              int center_id,
                              int node_number,
                              struct connection_class *connections,
                              int is_broadcast,
                              bc_info_t *info) {

    // fprintf(stdout, "%s", buf);
    // fflush(stdout);
    // strncpy(info->msg, buf, strlen(buf));

    if (mp->is_change_update || mp->is_change_add || mp->is_change_delete)

        for (int j = 0; j < 3; j++) {
            info->msg = send_all_json_message(center_id,
                                              mp,
                                              node_number,
                                              connections,
                                              j,
                                              is_broadcast,
                                              info);
        }

    return 0;
}

int send_result_to_meteor(meteor_param_t *mp,
                          int own_id,
                          struct connection_class *connections,
                          int is_broadcast,
                          bc_info_t *info) {

    // fprintf(stdout, "%s", buf);
    // fflush(stdout);
    // strncpy(info->msg, buf, strlen(buf));

    if (mp->is_change_update || mp->is_change_add || mp->is_change_delete)

        for (int j = 0; j < 3; j++) {
            info->msg = send_json_message(
                own_id, mp, connections, j, is_broadcast, info);
        }

    return 0;
}
int sort_neighbor_list(struct connection_class **neighbor,
                       int neighbor_number) {

    struct connection_class *temp_neighbor = NULL;
    int min_from_idx                       = 0;
    for (int i = 0; i < neighbor_number; i += 1) {
        min_from_idx = i;
        for (int j = i + 1; j < neighbor_number; j++) {
            if (neighbor[j]->from_node_index <
                neighbor[min_from_idx]->from_node_index) {
                min_from_idx = j;
            }
        }

        if (min_from_idx != i) {
            temp_neighbor          = neighbor[i];
            neighbor[i]            = neighbor[min_from_idx];
            neighbor[min_from_idx] = temp_neighbor;
        }

        for (int j = i + 1; j < neighbor_number; j++) {
            if (neighbor[i]->to_node_index == neighbor[j]->from_node_index &&
                neighbor[i]->from_node_index == neighbor[j]->to_node_index) {
                temp_neighbor   = neighbor[i + 1];
                neighbor[i + 1] = neighbor[j];
                neighbor[j]     = temp_neighbor;
            }
        }
    }

    // for (int i = 0; i < neighbor_number; i += 2) {
    //     printf("from=%d to=%d\n",
    //            neighbor[i]->from_node_index,
    //            neighbor[i]->to_node_index);
    //     printf("from=%d to=%d\n",
    //            neighbor[i + 1]->from_node_index,
    //            neighbor[i + 1]->to_node_index);
    //     puts("");
    // }
    return 0;
}

int parse_node(struct scenario_class *scenario, json_t *json) {
    json_t *jn;
    json_t *j;

    scenario->node_number = json_array_size(json);
    scenario->nodes = (struct node_class *) malloc(sizeof(struct node_class) *
                                                   scenario->node_number);
    memset(scenario->nodes,
           0,
           sizeof(struct node_class) * scenario->node_number);

    struct node_class *s_n;
    for (int i = 0; i < scenario->node_number; i++) {

        jn = json_array_get(json, i);

        if (!jn) {
            WARNING("json_array_get");
            return ERROR;
        }

        s_n = &scenario->nodes[i];
        node_init(s_n,
                  DEFAULT_NODE_NAME,
                  DEFAULT_NODE_TYPE,
                  DEFAULT_NODE_ID,
                  DEFAULT_NODE_SSID,
                  DEFAULT_NODE_CONNECTION,
                  DEFAULT_NODE_COORDINATE,
                  DEFAULT_NODE_COORDINATE,
                  DEFAULT_NODE_COORDINATE,
                  DEFAULT_NODE_INTERNAL_DELAY);

        interface_init(&s_n->interfaces[0],
                       DEFAULT_INTERFACE_NAME,
                       DEFAULT_INTERFACE_ADAPTER_TYPE,
                       DEFAULT_INTERFACE_ANTENNA_GAIN,
                       DEFAULT_INTERFACE_PT,
                       DEFAULT_INTERFACE_IP);

        s_n->interface_number = 1;

        // id
        s_n->id = i;

        // name
        SET_STRING_OBJECT(jn, s_n->name, NODE_NAME_STRING);

        // ssid
        SET_STRING_OBJECT(jn, s_n->ssid, NODE_SSID_STRING);

        // type
        const char *type = NULL;
        j                = json_object_get(jn, NODE_TYPE_STRING);
        if (j) {
            type = json_string_value(j);

            if (strcmp(type, NODE_TYPE_REGULAR_STRING) == 0)
                s_n->type = REGULAR_NODE;
            else if (strcmp(type, NODE_TYPE_ACCESS_POINT_STRING) == 0)
                s_n->type = ACCESS_POINT_NODE;
            else {
                WARNING(
                    "node attribute '%s' is assigned an invalid value "
                    "('%s')",
                    NODE_TYPE_STRING,
                    type);
                return ERROR;
            }
            // json_decref(j);
            j = NULL;
        }

        // connection
        const char *connection = NULL;
        j                      = json_object_get(jn, NODE_CONNECTION_STRING);
        if (j) {
            connection = json_string_value(j);

            if (strcmp(connection, NODE_CONNECTION_AD_HOC_STRING) == 0)
                s_n->connection = AD_HOC_CONNECTION;
            else if (strcmp(connection,
                            NODE_CONNECTION_INFRASTRUCTURE_STRING) == 0)
                s_n->connection = INFRASTRUCTURE_CONNECTION;
            else if (strcmp(connection, NODE_CONNECTION_ANY_STRING) == 0)
                s_n->connection = ANY_CONNECTION;
            else {
                WARNING(
                    "Node attribute '%s' is assigned an invalid value "
                    "('%s')",
                    NODE_CONNECTION_STRING,
                    connection);
                return ERROR;
            }
            // json_decref(j);
            j = NULL;
        }

        // adapter type
        const char *adapter_type = NULL;
        j = json_object_get(jn, INTERFACE_ADAPTER_STRING);
        if (j) {
            adapter_type = json_string_value(j);
            s_n->interface_number =
                1; // interface attribute was set => define interface

            if (strcmp(adapter_type, INTERFACE_ADAPTER_ORINOCO_STRING) == 0)
                s_n->interfaces[0].adapter_type = ORINOCO;
            else if (strcmp(adapter_type,
                            INTERFACE_ADAPTER_DEI80211MR_STRING) == 0)
                s_n->interfaces[0].adapter_type = DEI80211MR;
            else if (strcmp(adapter_type,
                            INTERFACE_ADAPTER_CISCO_340_STRING) == 0)
                s_n->interfaces[0].adapter_type = CISCO_340;
            else if (strcmp(adapter_type,
                            INTERFACE_ADAPTER_CISCO_ABG_STRING) == 0)
                s_n->interfaces[0].adapter_type = CISCO_ABG;
            else if (strcmp(adapter_type, INTERFACE_ADAPTER_JENNIC_STRING) ==
                     0)
                s_n->interfaces[0].adapter_type = JENNIC;
            else if (strcmp(adapter_type, INTERFACE_ADAPTER_S_NODE_STRING) ==
                     0)
                s_n->interfaces[0].adapter_type = S_NODE;
            else if (strcmp(adapter_type,
                            INTERFACE_ADAPTER_NS3_WIMAX_STRING) == 0) {
                s_n->interfaces[0].adapter_type = NS3_WIMAX;
                // special initialization for WiMAX interfaces:
                // * init built-in capacity structure
                // * init adapter properties
                capacity_update_all(&(s_n->interfaces[0].wimax_params),
                                    WIMAX_DEFAULT_SYS_BW,
                                    WIMAX_DEFAULT_MCS,
                                    MIMO_TYPE_SISO,
                                    1,
                                    1);

                if (wimax_init_ns3_adapter(&ns3_wimax) == ERROR) {
                    WARNING("Could not initialize the Ns-3 WiMAX adapter.");
                    return ERROR;
                }
            } else {
                WARNING(
                    "Node attribute '%s' is assigned an invalid value "
                    "('%s')",
                    INTERFACE_ADAPTER_STRING,
                    adapter_type);
                return ERROR;
            }

            // x, y, z
            SET_DOUBLE_OBJECT(jn, s_n->position.c[0], NODE_X_STRING);
            SET_DOUBLE_OBJECT(jn, s_n->position.c[1], NODE_Y_STRING);
            SET_DOUBLE_OBJECT(jn, s_n->position.c[2], NODE_Z_STRING);

            // internal_delay
            SET_DOUBLE_OBJECT(
                jn, s_n->internal_delay, NODE_INTERNAL_DELAY_STRING);

            // interface Pt
            SET_INTEGER_OBJECT(
                jn, s_n->interfaces[0].Pt, INTERFACE_PT_STRING);
            // json_decref(j);
            j = NULL;
        }
    }
    // json_decref(jn);
    jn = NULL;

    return SUCCESS;
}

int parse_environment(struct scenario_class *scenario, json_t *json) {
    json_t *je;
    json_t *j;

    scenario->environment_number = json_array_size(json);
    // printf("env number=%d\n", scenario->environment_number);

    scenario->environments = (struct environment_class *) malloc(
        sizeof(struct environment_class) * scenario->environment_number);

    memset(scenario->environments,
           0,
           sizeof(struct environment_class) * scenario->environment_number);

    int name_provided = FALSE;

    struct environment_class *e;
    for (int i = 0; i < scenario->environment_number; i++) {
        e = &scenario->environments[i];
        environment_init(e,
                         DEFAULT_ENVIRONMENT_NAME,
                         DEFAULT_ENVIRONMENT_TYPE,
                         DEFAULT_ENVIRONMENT_ALPHA,
                         DEFAULT_ENVIRONMENT_SIGMA,
                         DEFAULT_ENVIRONMENT_WALL_ATTENUATION,
                         DEFAULT_ENVIRONMENT_NOISE_POWER);

        je = json_array_get(json, i);
        if (!je) {
            WARNING("json_array_get");
            return ERROR;
        }

        j = json_object_get(je, ENVIRONMENT_NAME_STRING);
        if (j) {
            const char *name;
            name = json_string_value(j);
            strncpy(e->name, name, strlen(name));
            e->name_hash  = string_hash(e->name, strlen(e->name));
            name_provided = TRUE;
            // json_decref(j);
            j = NULL;
        }

        SET_STRING_OBJECT(je, e->type, ENVIRONMENT_TYPE_STRING);

        j = json_object_get(je, ENVIRONMENT_IS_DYNAMIC_STRING);
        if (j) {
            if (j == json_true()) {
                e->is_dynamic = TRUE;
            }
            if (j == json_false()) {
                e->is_dynamic = FALSE;
            }
            // const char *type;
            // type = json_string_value(j);
            // if (strncmp(type, TRUE_STRING, strlen(type)) == 0) {
            //     e->is_dynamic = TRUE;
            // }
            // if (strncmp(type, FALSE_STRING, strlen(type)) == 0) {
            //     e->is_dynamic = TRUE;
            // }
            // json_decref(j);
            j = NULL;
        }

        SET_DOUBLE_OBJECT(je, e->alpha[0], ENVIRONMENT_ALPHA_STRING);
        if (e->alpha[0] < 0) {
            e->alpha[0] = 0;
        }
        SET_DOUBLE_OBJECT(je, e->sigma[0], ENVIRONMENT_SIGMA_STRING);
        if (e->sigma[0] < 0) {
            e->sigma[0] = 0;
        }
        SET_DOUBLE_OBJECT(je, e->W[0], ENVIRONMENT_W_STRING);
        if (e->W[0] < 0) {
            e->W[0] = 0;
        }
        SET_DOUBLE_OBJECT(
            je, e->noise_power[0], ENVIRONMENT_NOISE_POWER_STRING);

        j = json_object_get(je, ENVIRONMENT_FADING_STRING);
        if (j) {
            const char *fading;
            fading = json_string_value(j);
            if (strncmp(fading,
                        ENVIRONMENT_FADING_AWGN_STRING,
                        strlen(ENVIRONMENT_FADING_AWGN_STRING)) == 0) {
                e->fading = AWGN_FADING;
            }
            if (strncmp(fading,
                        ENVIRONMENT_FADING_RAYLEIGH_STRING,
                        strlen(ENVIRONMENT_FADING_RAYLEIGH_STRING)) == 0) {
                e->fading = RAYLEIGH_FADING;
            }
            // json_decref(j);
            j = NULL;
        }

        if (name_provided == FALSE) {
            return ERROR;
        }
    }
    je = NULL;

    // printf("Creating the following environment:\n");
    // environment_print(&scenario->environments[0]);
    return SUCCESS;
}

int parse_connection(struct scenario_class *scenario, json_t *json) {

    json_t *jc;
    json_t *j;

    int json_connection_number;

    json_connection_number = json_array_size(json);

    if (0 <= scenario->own_id) {
        scenario->connection_number = scenario->node_number - 1;
    } else {
        scenario->connection_number =
            scenario->node_number * (scenario->node_number - 1);
    }

    scenario->connections = (struct connection_class *) malloc(
        sizeof(struct connection_class) * scenario->connection_number);
    scenario->connection_number = 0;

    memset(scenario->connections,
           0,
           sizeof(struct connection_class) * scenario->connection_number);

    int from_node_provided           = FALSE;
    int to_node_provided             = FALSE;
    int through_environment_provided = FALSE;
    struct connection_class c;
    for (int i = 0; i < json_connection_number; i++) {

        if (connection_init(
                &c,
                DEFAULT_CONNECTION_PARAMETER,
                DEFAULT_CONNECTION_PARAMETER,
                DEFAULT_CONNECTION_PARAMETER,
                DEFAULT_CONNECTION_PACKET_SIZE,
                DEFAULT_CONNECTION_STANDARD,
                UNDEFINED_VALUE, // channel value is set later because
                                 // it depends on standard!!!
                DEFAULT_CONNECTION_RTS_CTS_THRESHOLD,
                DEFAULT_CONNECTION_CONSIDER_INTERFERENCE) == ERROR)
            return ERROR;

        jc = json_array_get(json, i);
        if (!jc) {
            WARNING("json_array_get");
            return ERROR;
        }

        // from_node
        j = json_object_get(jc, CONNECTION_FROM_NODE_STRING);
        if (j) {
            const char *from;
            from = json_string_value(j);
            strncpy(c.from_node, from, strlen(from));
            from_node_provided = TRUE;
            // json_decref(j);
            j = NULL;
        }

        if (0 <= scenario->own_id &&
            strcmp(scenario->nodes[scenario->own_id].name, c.from_node) !=
                0) {
            continue;
        }

        // from_interface
        SET_STRING_OBJECT(
            jc, c.from_interface, CONNECTION_FROM_INTERFACE_STRING);

        // to_node
        j = json_object_get(jc, CONNECTION_TO_NODE_STRING);
        if (j) {
            const char *to;
            to = json_string_value(j);
            strncpy(c.to_node, to, strlen(to));
            to_node_provided = TRUE;
            // json_decref(j);
            j = NULL;
        }

        // to_interface
        SET_STRING_OBJECT(jc, c.to_interface, CONNECTION_TO_INTERFACE_STRING);
        // through_environment
        j = json_object_get(jc, CONNECTION_THROUGH_ENVIRONMENT_STRING);
        if (j) {
            const char *through_environment;
            through_environment = json_string_value(j);
            strncpy(c.through_environment,
                    through_environment,
                    strlen(through_environment));
            through_environment_provided = TRUE;
            // json_decref(j);
            j = NULL;
        }

        // packet_size
        SET_INTEGER_OBJECT(jc, c.packet_size, CONNECTION_PACKET_SIZE_STRING);
        if (c.packet_size < 0) {
            c.packet_size = DEFAULT_CONNECTION_PACKET_SIZE;
        }

        // standard
        const char *standard = NULL;
        j = json_object_get(jc, CONNECTION_STANDARD_STRING);
        if (j) {
            standard = json_string_value(j);
            if (strcmp(standard, CONNECTION_STANDARD_802_11B_STRING) == 0) {
                if (connection_init_standard(&c, WLAN_802_11B) == ERROR)
                    return ERROR;
            } else if (strcmp(standard, CONNECTION_STANDARD_802_11G_STRING) ==
                       0) {
                if (connection_init_standard(&c, WLAN_802_11G) == ERROR)
                    return ERROR;
            } else if (strcmp(standard, CONNECTION_STANDARD_802_11A_STRING) ==
                       0) {
                if (connection_init_standard(&c, WLAN_802_11A) == ERROR)
                    return ERROR;
            } else if (strcmp(standard, CONNECTION_STANDARD_ETH_10_STRING) ==
                       0) {
                if (connection_init_standard(&c, ETHERNET_10) == ERROR)
                    return ERROR;
            } else if (strcmp(standard, CONNECTION_STANDARD_ETH_100_STRING) ==
                       0) {
                if (connection_init_standard(&c, ETHERNET_100) == ERROR)
                    return ERROR;
            } else if (strcmp(standard,
                              CONNECTION_STANDARD_ETH_1000_STRING) == 0) {
                if (connection_init_standard(&c, ETHERNET_1000) == ERROR)
                    return ERROR;
            } else if (strcmp(standard,
                              CONNECTION_STANDARD_ACTIVE_TAG_STRING) == 0) {
                if (connection_init_standard(&c, ACTIVE_TAG) == ERROR)
                    return ERROR;
            } else if (strcmp(standard, CONNECTION_STANDARD_ZIGBEE_STRING) ==
                       0) {
                if (connection_init_standard(&c, ZIGBEE) == ERROR)
                    return ERROR;
            } else if (strcmp(standard, CONNECTION_STANDARD_802_16_STRING) ==
                       0) {
                if (connection_init_standard(&c, WIMAX_802_16) == ERROR)
                    return ERROR;
            } else {
                WARNING(
                    "Connection attribute '%s' is assigned an invalid value \
('%s')",
                    CONNECTION_STANDARD_STRING,
                    standard);
                return ERROR;
            }
            // json_decref(j);
            j = NULL;
        }

        // rate
        const char *rate = NULL;
        j                = json_object_get(jc, CONNECTION_RATE_STRING);
        if (j) {
            rate = json_string_value(j);

            if ((c.standard != WLAN_802_11B) &&
                (c.standard != WLAN_802_11G) &&
                (c.standard != WLAN_802_11A)) {
                WARNING(
                    "Connection attribute '%s' is not supported for this \
standard",
                    CONNECTION_RATE_STRING);
                return ERROR;
            } else if (strcmp(rate, CONNECTION_RATE_ADAPTIVE_STRING) == 0) {
                c.adaptive_operating_rate = TRUE;
            } else if (strcmp(rate, CONNECTION_RATE_1MBPS_STRING) == 0) {
                c.adaptive_operating_rate = FALSE;

                if (c.standard == WLAN_802_11B)
                    connection_init_operating_rate(&c, B_RATE_1MBPS);
                else if (c.standard == WLAN_802_11G)
                    connection_init_operating_rate(&c, G_RATE_1MBPS);
                else if (c.standard == WLAN_802_11A) {
                    WARNING(
                        "Connection attribute '%s' is assigned a value ('%s') \
not supported for this standard",
                        CONNECTION_RATE_STRING,
                        rate);
                    return ERROR;
                }
            } else if (strcmp(rate, CONNECTION_RATE_2MBPS_STRING) == 0) {
                c.adaptive_operating_rate = FALSE;

                if (c.standard == WLAN_802_11B)
                    connection_init_operating_rate(&c, B_RATE_2MBPS);
                else if (c.standard == WLAN_802_11G)
                    connection_init_operating_rate(&c, G_RATE_2MBPS);
                else if (c.standard == WLAN_802_11A) {
                    WARNING(
                        "Connection attribute '%s' is assigned a value ('%s') \
not supported for this standard",
                        CONNECTION_RATE_STRING,
                        rate);
                    return ERROR;
                }
            } else if (strcmp(rate, CONNECTION_RATE_5MBPS_STRING) == 0) {
                c.adaptive_operating_rate = FALSE;

                if (c.standard == WLAN_802_11B)
                    connection_init_operating_rate(&c, B_RATE_5MBPS);
                else if (c.standard == WLAN_802_11G)
                    connection_init_operating_rate(&c, G_RATE_5MBPS);
                else if (c.standard == WLAN_802_11A) {
                    WARNING(
                        "Connection attribute '%s' is assigned a value ('%s') \
not supported for this standard",
                        CONNECTION_RATE_STRING,
                        rate);
                    return ERROR;
                }
            } else if (strcmp(rate, CONNECTION_RATE_6MBPS_STRING) == 0) {
                c.adaptive_operating_rate = FALSE;

                if (c.standard == WLAN_802_11A)
                    connection_init_operating_rate(&c, A_RATE_6MBPS);
                else if (c.standard == WLAN_802_11G)
                    connection_init_operating_rate(&c, G_RATE_6MBPS);
                else if (c.standard == WLAN_802_11B) {
                    WARNING(
                        "Connection attribute '%s' is assigned a value ('%s') \
not supported for this standard",
                        CONNECTION_RATE_STRING,
                        rate);
                    return ERROR;
                }
            } else if (strcmp(rate, CONNECTION_RATE_9MBPS_STRING) == 0) {
                c.adaptive_operating_rate = FALSE;

                if (c.standard == WLAN_802_11A)
                    connection_init_operating_rate(&c, A_RATE_9MBPS);
                else if (c.standard == WLAN_802_11G)
                    connection_init_operating_rate(&c, G_RATE_9MBPS);
                else if (c.standard == WLAN_802_11B) {
                    WARNING(
                        "Connection attribute '%s' is assigned a value ('%s') \
not supported for this standard",
                        CONNECTION_RATE_STRING,
                        rate);
                    return ERROR;
                }
            } else if (strcmp(rate, CONNECTION_RATE_11MBPS_STRING) == 0) {
                c.adaptive_operating_rate = FALSE;

                if (c.standard == WLAN_802_11B)
                    connection_init_operating_rate(&c, B_RATE_11MBPS);
                else if (c.standard == WLAN_802_11G)
                    connection_init_operating_rate(&c, G_RATE_11MBPS);
                else if (c.standard == WLAN_802_11A) {
                    WARNING(
                        "Connection attribute '%s' is assigned a value ('%s') \
not supported for this standard",
                        CONNECTION_RATE_STRING,
                        rate);
                    return ERROR;
                }
            } else if (strcmp(rate, CONNECTION_RATE_12MBPS_STRING) == 0) {
                c.adaptive_operating_rate = FALSE;

                if (c.standard == WLAN_802_11A)
                    connection_init_operating_rate(&c, A_RATE_12MBPS);
                else if (c.standard == WLAN_802_11G)
                    connection_init_operating_rate(&c, G_RATE_12MBPS);
                else if (c.standard == WLAN_802_11B) {
                    WARNING(
                        "Connection attribute '%s' is assigned a value ('%s') \
not supported for this standard",
                        CONNECTION_RATE_STRING,
                        rate);
                    return ERROR;
                }
            } else if (strcmp(rate, CONNECTION_RATE_18MBPS_STRING) == 0) {
                c.adaptive_operating_rate = FALSE;

                if (c.standard == WLAN_802_11A)
                    connection_init_operating_rate(&c, A_RATE_18MBPS);
                else if (c.standard == WLAN_802_11G)
                    connection_init_operating_rate(&c, G_RATE_18MBPS);
                else if (c.standard == WLAN_802_11B) {
                    WARNING(
                        "Connection attribute '%s' is assigned a value ('%s') \
not supported for this standard",
                        CONNECTION_RATE_STRING,
                        rate);
                    return ERROR;
                }
            } else if (strcmp(rate, CONNECTION_RATE_24MBPS_STRING) == 0) {
                c.adaptive_operating_rate = FALSE;

                if (c.standard == WLAN_802_11A)
                    connection_init_operating_rate(&c, A_RATE_24MBPS);
                else if (c.standard == WLAN_802_11G)
                    connection_init_operating_rate(&c, G_RATE_24MBPS);
                else if (c.standard == WLAN_802_11B) {
                    WARNING(
                        "Connection attribute '%s' is assigned a value ('%s') \
not supported for this standard",
                        CONNECTION_RATE_STRING,
                        rate);
                    return ERROR;
                }
            } else if (strcmp(rate, CONNECTION_RATE_36MBPS_STRING) == 0) {
                c.adaptive_operating_rate = FALSE;

                if (c.standard == WLAN_802_11A)
                    connection_init_operating_rate(&c, A_RATE_36MBPS);
                else if (c.standard == WLAN_802_11G)
                    connection_init_operating_rate(&c, G_RATE_36MBPS);
                else if (c.standard == WLAN_802_11B) {
                    WARNING(
                        "Connection attribute '%s' is assigned a value ('%s') \
not supported for this standard",
                        CONNECTION_RATE_STRING,
                        rate);
                    return ERROR;
                }
            } else if (strcmp(rate, CONNECTION_RATE_48MBPS_STRING) == 0) {
                c.adaptive_operating_rate = FALSE;

                if (c.standard == WLAN_802_11A)
                    connection_init_operating_rate(&c, A_RATE_48MBPS);
                else if (c.standard == WLAN_802_11G)
                    connection_init_operating_rate(&c, G_RATE_48MBPS);
                else if (c.standard == WLAN_802_11B) {
                    WARNING(
                        "Connection attribute '%s' is assigned a value ('%s') \
not supported for this standard",
                        CONNECTION_RATE_STRING,
                        rate);
                    return ERROR;
                }
            } else if (strcmp(rate, CONNECTION_RATE_54MBPS_STRING) == 0) {
                c.adaptive_operating_rate = FALSE;

                if (c.standard == WLAN_802_11A)
                    connection_init_operating_rate(&c, A_RATE_54MBPS);
                else if (c.standard == WLAN_802_11G)
                    connection_init_operating_rate(&c, G_RATE_54MBPS);
                else if (c.standard == WLAN_802_11B) {
                    WARNING(
                        "Connection attribute '%s' is assigned a value ('%s') \
not supported for this standard",
                        CONNECTION_RATE_STRING,
                        rate);
                    return ERROR;
                }
            } else {
                WARNING(
                    "Connection attribute '%s' is assigned an invalid value \
('%s')",
                    CONNECTION_RATE_STRING,
                    rate);
                return ERROR;
            }
            // json_decref(j);
            j = NULL;
        }

        // channel
        j = json_object_get(jc, CONNECTION_CHANNEL_STRING);
        if (j) {
            long int long_int_result = json_integer_value(j);

            if (long_int_result == LONG_MIN)
                return ERROR;
            else if ((long_int_result >= INT_MIN) &&
                     (long_int_result <= INT_MAX))
                c.channel = (int) long_int_result;
            else {
                WARNING(
                    "Connection attribute '%s' has a value outside the \
'int' range ('%ld')",
                    CONNECTION_CHANNEL_STRING,
                    long_int_result);
                return ERROR;
            }

            // check the value is correct
            // (channel values should depend on connection standard
            // !!!!!!!!)
            if (c.channel < MIN_CHANNEL || c.channel > MAX_CHANNEL) {
                WARNING(
                    "Connection attribute '%s' must be between %d and %d. \
Ignored invalid value ('%lu')",
                    CONNECTION_CHANNEL_STRING,
                    MIN_CHANNEL,
                    MAX_CHANNEL,
                    long_int_result);
                c.channel = MIN_CHANNEL;
                return ERROR;
            }
            // json_decref(j);
            j = NULL;
        }

        // RTS CTS threshold

        // bandwidth

        // loss rate

        // delay

        // jitter

        // consider_interference
        j = json_object_get(jc, CONNECTION_CONSIDER_INTERFERENCE_STRING);
        if (j) {
            int val;
            val = json_boolean_value(j);

            if (val) {
                c.consider_interference = TRUE;
            } else {
                c.consider_interference = FALSE;
            }
            // json_decref(j);
            j = NULL;
        }

        if (from_node_provided == FALSE) {
            WARNING("Connection attribute 'from_node' is mandatory!");
            return ERROR;
        }

        if (to_node_provided == FALSE) {
            WARNING("Connection attribute 'to_node' is mandatory!");
            return ERROR;
        }

        if (through_environment_provided == FALSE) {
            WARNING(
                "Connection attribute 'through_environment' is "
                "mandatory!");
            return ERROR;
        }

        if (scenario_add_connection(scenario, &c) == NULL) {
            return ERROR;
        }
    }
    jc = NULL;

    return SUCCESS;
}

int set_all_neighbor_bmp(int center_id,
                         int **neighbor_ids,
                         int **neighbor_ids_bmp,
                         int node_number) {

    int ni = 0;

    if (neighbor_ids[center_id][0] == -1) {
        memset(neighbor_ids_bmp[center_id], 0, sizeof(int) * node_number);
        // for (int i = 0; i < node_number; i++) {
        //     if (i == center_id) {
        //         continue;
        //     }
        //     neighbor_ids_bmp[i][center_id] = 1;
        // }
        return SUCCESS;
    }

    for (int i = 0; neighbor_ids[center_id][i] != -1; i++) {
        ni = neighbor_ids[center_id][i];

        neighbor_ids_bmp[center_id][ni] = 1;
        neighbor_ids_bmp[ni][center_id] = 1;
    }
    return SUCCESS;
}

int set_neighbor_bmp(int *neighbor_ids,
                     int *neighbor_ids_bmp,
                     int is_other_update,
                     int is_other_delete,
                     int node_number) {

#ifdef DEBUG_PRINT
    printf("is_other_delete=%d\nis_other_update=%d\n",
           is_other_delete,
           is_other_update);
#endif
    int ni = 0;
    if (neighbor_ids[0] == -1) {
        memset(neighbor_ids_bmp, 0, sizeof(int) * node_number);
        // for (int i = 0; i < node_number; i++) {
        //     neighbor_ids_bmp[i] = 0;
        // }

        return SUCCESS;
    }
    if (is_other_delete) {
        ni                   = neighbor_ids[0];
        neighbor_ids_bmp[ni] = 0;
        return SUCCESS;
    }

    if (is_other_update) {
        ni                   = neighbor_ids[0];
        neighbor_ids_bmp[ni] = 1;

        return SUCCESS;
    }

    for (int i = 0; neighbor_ids[i] != -1; i++) {
        ni                   = neighbor_ids[i];
        neighbor_ids_bmp[ni] = 1;
    }
    return SUCCESS;
}

int set_all_prev_neighbor_bmp(int center_id,
                              int **neighbor_ids_bmp,
                              int **prev_neighbor_ids_bmp,
                              int node_number) {
    memcpy(prev_neighbor_ids_bmp[center_id],
           neighbor_ids_bmp[center_id],
           sizeof(int) * node_number);
    memset(neighbor_ids_bmp[center_id], 0, sizeof(int) * node_number);
    // for (int i = 0; i < node_number; i++) {
    //     prev_neighbor_ids_bmp[center_id][i] =
    //     neighbor_ids_bmp[center_id][i]; neighbor_ids_bmp[center_id][i] = 0;
    // }

    return 0;
}
int set_prev_neighbor_bmp(int *neighbor_ids_bmp,
                          int *prev_neighbor_ids_bmp,
                          int node_number,
                          int is_other_update,
                          int is_other_delete) {

    memcpy(
        prev_neighbor_ids_bmp, neighbor_ids_bmp, sizeof(int) * node_number);

    if (is_other_update == 0 && is_other_delete == 0) {
        memset(neighbor_ids_bmp, 0, sizeof(int) * node_number);
    }
    // for (int i = 0; i < node_number; ++i) {
    //     prev_neighbor_ids_bmp[i] = neighbor_ids_bmp[i];
    //     if (is_other_update == 0 && is_other_delete == 0) {
    //         neighbor_ids_bmp[i] = 0;
    //     }
    // }

    return 0;
}

int clear_neighbor_bmp(int center_id,
                       int **neighbor_ids_bmp,
                       int node_number) {
    for (int i = 0; i < node_number; i++) {
        neighbor_ids_bmp[center_id][i] = 0;
    }
    return 0;
}

void print_all_neighbor_bmp(int **neighbor_ids_bmp, int number) {
    for (int i = 0; i < number; i++) {
        printf("bmp center=%d\t", i);
        for (int j = 0; j < number; j++) {
            if (neighbor_ids_bmp[i][j] == 1) {
                printf("%2d ", j);
            } else {
                printf("   ");
            }
        }
        printf("\n");
    }
}
void print_neighbor_bmp(int *neighbor_ids_bmp, int loop_number, int own_id) {
    printf("bmp center=%d\t", own_id);
    for (int i = 0; i < loop_number; i++) {
        if (neighbor_ids_bmp[i] == 1) {
            printf("%2d ", i);
        } else {
            printf("   ");
        }
    }
    printf("\n");
}

void print_all_meteor_param(int center_id, meteor_param_t *m) {
    // if (!m->is_change_update && !m->is_change_add &&
    // !m->is_change_delete)

    printf("-----------------------------------\n");
    printf("meteor center=%d\n", center_id);
    // printf("\tupdate=%d\t", m->is_change_update);
    printf("\tupdate\t");
    for (int j = 0; m->update[j] != -1; j++) {
        printf("%d ", m->update[j]);
    }
    puts("");

    // printf("\tadd=%d\t", m->is_change_add);
    printf("\tadd\t");
    for (int j = 0; m->add[j] != -1; j++) {
        printf("%d ", m->add[j]);
    }
    puts("");
    // printf("\tdelete=%d\t", m->is_change_delete);
    printf("\tdelete\t");
    for (int j = 0; m->delete[j] != -1; j++) {
        printf("%d ", m->delete[j]);
    }
    puts("");
}

void print_meteor_param(int center_id, meteor_param_t *m) {
    // if (!m->is_change_update && !m->is_change_add &&
    // !m->is_change_delete)

    printf("-----------------------------------\n");
    printf("meteor center=%d\n", center_id);
    // printf("\tupdate=%d\t", m->is_change_update);
    printf("\tupdate\t");
    for (int j = 0; m->update[j] != -1; j++) {
        printf("%d ", m->update[j]);
    }
    puts("");

    // printf("\tadd=%d\t", m->is_change_add);
    printf("\tadd\t");
    for (int j = 0; m->add[j] != -1; j++) {
        printf("%d ", m->add[j]);
    }
    puts("");
    // printf("\tdelete=%d\t", m->is_change_delete);
    printf("\tdelete\t");
    for (int j = 0; m->delete[j] != -1; j++) {
        printf("%d ", m->delete[j]);
    }
    puts("");
}

int clear_meteor_param(meteor_param_t *mp) {

    mp->add[0]           = -1;
    mp->delete[0]        = -1;
    mp->update[0]        = -1;
    mp->add_last_idx     = 0;
    mp->delete_last_idx  = 0;
    mp->update_last_idx  = 0;
    mp->is_change_update = 0;
    mp->is_change_add    = 0;
    mp->is_change_delete = 0;
    return 0;
}

int is_same_id(int *arr, int val) {
    for (int i = 0; arr[i] != -1; i++) {
        if (arr[i] == val) {
            return 1;
        }
    }
    if (arr[0] == -1) {
        return 0;
    }

    return 0;
}
int set_all_meteor_param(meteor_param_t *mp,
                         int **current_map,
                         int **prev_map,
                         int center_id,
                         int node_number) {

    clear_meteor_param(mp);

    for (int j = 0; j < node_number; j++) {
        if (j == center_id) {
            continue;
        }
        if (prev_map[center_id][j] == current_map[center_id][j] &&
            current_map[center_id][j] == 1) {
            if (!is_same_id(mp->update, j)) {
                mp->update[mp->update_last_idx++] = j;
                mp->update[mp->update_last_idx]   = -1;
            }
            mp->is_change_update = 1;
            continue;
        }
        if (prev_map[center_id][j] < current_map[center_id][j]) {
            if (!is_same_id(mp->add, j)) {
                mp->add[mp->add_last_idx++] = j;
                mp->add[mp->add_last_idx]   = -1;
            }
            mp->is_change_add = 1;
            continue;
        }
        if (current_map[center_id][j] < prev_map[center_id][j]) {
            if (!is_same_id(mp->delete, j)) {
                mp->delete[mp->delete_last_idx++] = j;
                mp->delete[mp->delete_last_idx]   = -1;
            }
            mp->is_change_delete = 1;
            continue;
        }
    }

    return 0;
}

int set_meteor_param(meteor_param_t *mp,
                     int *current_map,
                     int *prev_map,
                     int own_id,
                     int node_number,
                     int is_other_update,
                     int is_other_delete,
                     int received_center_id) {

    clear_meteor_param(mp);

    for (int i = 0; i < node_number; i++) {
        if (is_other_update || is_other_delete) {
            // printf("received_center_id=%d\n", received_center_id);
            i           = received_center_id;
            node_number = i;
        }
        if (i == own_id) {
            continue;
        }
        if (prev_map[i] == current_map[i] && current_map[i] == 1) {
            if (!is_same_id(mp->update, i)) {
                mp->update[mp->update_last_idx++] = i;
                mp->update[mp->update_last_idx]   = -1;
            }
            mp->is_change_update = 1;
            continue;
        }
        if (prev_map[i] < current_map[i]) {
            if (!is_same_id(mp->add, i)) {
                mp->add[mp->add_last_idx++] = i;
                mp->add[mp->add_last_idx]   = -1;
            }
            mp->is_change_add = 1;
            continue;
        }
        if (current_map[i] < prev_map[i]) {
            if (!is_same_id(mp->delete, i)) {
                mp->delete[mp->delete_last_idx++] = i;
                mp->delete[mp->delete_last_idx]   = -1;
            }
            mp->is_change_delete = 1;
            continue;
        }
    }

    return 0;
}
