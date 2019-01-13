#ifndef JSON_H
#define JSON_H

#include "socket.h"
#include "connection.h"
#include "input.h"

typedef struct meteor {
    int *add;
    int *delete;
    int *update;

    int add_last_idx;
    int delete_last_idx;
    int update_last_idx;

    int is_change_update;
    int is_change_add;
    int is_change_delete;

} meteor_param_t;

struct msg {
    int8_t type;
    int32_t id;
    float x;
    float y;
    int16_t neighbor_size;
    int32_t neighbor[1];
};

void init_msg_struct();
int json_init_scenario(struct scenario_class *scenario, FILE *init_fd);
// =====================================================
int update_neighbors(struct scenario_class *scenario,
                     struct connection_class **neighbor,
                     int own_id,
                     int *neighbor_ids,
                     input_buffer_t *ibuf,
                     int *is_other_update,
                     int *is_other_delete,
                     // int *is_other,
                     // int *is_contain_other,
                     int *received_center_id,
                     int type);

int set_neighbor_bmp(int *neighbor_ids,
                     int *neighbor_ids_bmp,
                     int is_other_update,
                     int is_other_delete,
                     int node_number);

int search_connection(struct connection_class **conn_pair,
                      int from_id,
                      int to_id,
                      struct connection_class *connections);
int send_update_json(int own_id,
                     meteor_param_t *mp,
                     struct connection_class *connections,
                     int is_broadcast,
                     bc_info_t *info);
int send_add_json(int own_id,
                  meteor_param_t *mp,
                  struct connection_class *connections,
                  int is_broadcast,
                  bc_info_t *info);
int send_delete_json(int center_id,
                     meteor_param_t *mp,
                     int is_broadcast,
                     bc_info_t *info);

char *send_json_message(int own_id,
                        meteor_param_t *mp,
                        struct connection_class *connections,
                        int type,
                        int is_broadcast,
                        bc_info_t *info);
int send_result_to_meteor(meteor_param_t *mp,
                          int own_id,
                          struct connection_class *connections,
                          int is_broadcast,
                          bc_info_t *info);

int set_prev_neighbor_bmp(int *neighbor_ids_bmp,
                          int *prev_neighbor_ids_bmp,
                          int node_number,
                          int is_other_update,
                          int is_other_delete);

int clear_neighbor_bmp();
int copy_neighbor_bmp();

int set_meteor_param(meteor_param_t *meteor_param,
                     int *neighbor_ids_bmp,
                     int *prev_neighbor_ids_bmp,
                     int own_id,
                     int node_number,
                     int is_other_update,
                     int is_other_delete,
                     int received_center_id);

void print_neighbor_bmp(int *neighbor_ids_bmp, int loop_number, int own_id);
void print_meteor_param(int own_id, meteor_param_t *m);

// =====================================================
int update_all_neighbors(struct scenario_class *scenario,
                         struct connection_class **neighbor,
                         int *center_id,
                         int **neighbor_ids,
                         input_buffer_t *ibuf,
                         int type);
int send_all_result_to_meteor(meteor_param_t *mp,
                              int center_id,
                              int node_number,
                              struct connection_class *connections,
                              int is_broadcast,
                              bc_info_t *info);

int set_all_neighbor_bmp(int center_id,
                         int **neighbor_ids,
                         int **neighbor_ids_bmp,
                         int node_number);

int set_all_prev_neighbor_bmp(int center_id,
                              int **neighbor_ids_bmp,
                              int **prev_neighbor_ids_bmp,
                              int node_number);
int search_all_connection(struct connection_class **conn_pair,
                          int from_id,
                          int to_id,
                          struct connection_class *connections,
                          int node_number);
int send_all_update_json(int center_id,
                         meteor_param_t *mp,
                         int node_number,
                         struct connection_class *connections,
                         int is_broadcast,
                         bc_info_t *info);
int send_all_add_json(int center_id,
                      meteor_param_t *mp,
                      int node_number,
                      struct connection_class *connections,
                      int is_broadcast,

                      bc_info_t *info);

int send_all_delete_json(int center_id,
                         meteor_param_t *mp,
                         int is_broadcast,
                         bc_info_t *info);

char *send_all_json_message(int center_id,
                            meteor_param_t *mp,
                            int node_number,
                            struct connection_class *connections,
                            int type,
                            int is_broadcast,
                            bc_info_t *info);
int sort_all_neighbor_list();

int clear_all_neighbor_bmp();
int copy_all_neighbor_bmp();

int set_all_meteor_param(meteor_param_t *meteor_param,
                         int **neighbor_ids_bmp,
                         int **prev_neighbor_ids_bmp,
                         int center_id,
                         int node_number);

void print_all_neighbor_bmp(int **neighbor_ids_bmp, int number);
void print_all_meteor_param(int center_id, meteor_param_t *m);

int parse_node();
int parse_environment();
int parse_connection();

#endif
