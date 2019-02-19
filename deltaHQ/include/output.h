/*
 * Copyright (c) 2019 kosay  All rights reserved.
 *
 * See the file 'LICENSE' for licensing information.
 *
 */
#ifndef _OUTPUT_H
#define _OUTPUT_H

#include <stdio.h>

struct connection_class;
struct node_class;
struct io_connection_state_class;

void *write_connection(void *arg);
int write_connection_status_text(
    struct connection_class **neighbor,
    int neighbor_number,
    struct node_class *nodes,
    struct io_connection_state_class *io_connection_state,
    double current_time,
    double start_time,
    int own_id,
    int text_output_enabled,
    FILE *text_output_file,
    int binary_output_enabled,
    int cartesian_coord_syst);

int write_connection_status_binary(
    struct connection_class **neighbor,
    int neighbor_number,
    struct io_connection_state_class *io_connection_state,
    struct node_class *nodes,
    FILE *binary_output_file,
    double current_timef);

void *write_node(void *arg);

#endif
