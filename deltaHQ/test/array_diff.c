#include <stdio.h>

#define NODES 10

int main(void) {
    int neighbor[] = {2, 3, 1, 4, 5, 6, 1, 9};

    int prev_map[NODES]    = {0, 1, 0, 0, 0, 1, 1, 1, 1, 0};
    int current_map[NODES] = {0};

    int add[NODES]    = {0};
    int del[NODES]    = {0};
    int update[NODES] = {0};

    int nei_num = 8;

    for (int i = 0; i < nei_num; i++) {
        current_map[neighbor[i]] = 1;
    }

    int update_idx = 0;
    int add_idx    = 0;
    int del_idx    = 0;
    for (int i = 0; i < NODES; i++) {
        if (prev_map[i] == current_map[i]) {
            update[update_idx++] = i;
            continue;
        }
        if (prev_map[i] < current_map[i]) {
            add[add_idx++] = i;
            continue;
        }
        if (current_map[i] < prev_map[i]) {
            del[del_idx++] = i;
            continue;
        }
    }

    puts("update");
    for (int i = 0; i < update_idx; i++) {
        printf("%d ", update[i]);
    }
    puts("");
    puts("add");
    for (int i = 0; i < add_idx; i++) {
        printf("%d ", add[i]);
    }
    puts("");
    puts("delete");
    for (int i = 0; i < del_idx; i++) {
        printf("%d ", del[i]);
    }
    return 0;
}
