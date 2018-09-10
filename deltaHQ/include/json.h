#ifndef JSON_H
#define JSON_H

#include <socket.h>
#include <connection.h>
#include <input.h>

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

int update_neighbors(struct scenario_class *scenario,
                     struct connection_class **neighbor,
                     int *center_id,
                     int **neighbor_ids,
                     input_buffer_t *ibuf,
                     int type);

int send_result_to_meteor(meteor_param_t *mp,
                          int center_id,
                          int node_number,
                          struct connection_class *connections,
                          int is_broadcast,
                          bc_info_t *info);

int json_init_scenario(struct scenario_class *scenario, input_buffer_t *ibuf);

int sort_neighbor_list();

int set_neighbor_bmp(int center_id,
                     int **neighbor_ids,
                     int **neighbor_ids_bmp);

int copy_neighbor_bmp();

int set_prev_neighbor_bmp(int center_id,
                          int **neighbor_ids_bmp,
                          int **prev_neighbor_ids_bmp,
                          int node_number);
int clear_neighbor_bmp();

void print_neighbor_bmp();

void print_meteor_param(int center_id, meteor_param_t *m);

int set_meteor_param(meteor_param_t *meteor_param,
                     int **neighbor_ids_bmp,
                     int **prev_neighbor_ids_bmp,
                     int center_id,
                     int node_number);
#endif
