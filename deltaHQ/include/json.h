#ifndef JSON_H
#define JSON_H

typedef struct broadcast_info bc_info_t;
struct connection_class;

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

int init_neighbors();
int update_neighbors();
int send_result_to_meteor(meteor_param_t *mp,
                          int *center_ids,
                          int node_number,
                          struct connection_class *connections,
                          int is_broadcast,
                          bc_info_t *info);
int json_init_scenario();
int sort_neighbor_list();
int set_neighbor_bmp();
int copy_neighbor_bmp();
int set_prev_neighbor_bmp();
int clear_neighbor_bmp();
void print_neighbor_bmp();
void print_meteor_param();
int set_meteor_param();
#endif
