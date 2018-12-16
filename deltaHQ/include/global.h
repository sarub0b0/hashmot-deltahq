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
 * File name: global.h
 * Function:  Header file with global defines
 *
 * Author: Razvan Beuran
 *
 * $Id: global.h 145 2013-06-07 01:11:09Z razvan $
 *
 ***********************************************************************/

#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <stdio.h>
#include <time.h>
/////////////////////////////////////////////
// Basic constants
/////////////////////////////////////////////

// BOOLEAN
#define TRUE 1
#define FALSE 0

#define SUCCESS 0
#define ERROR -1

//#ifndef MAX_STRING
#define MAX_STRING 16
//#endif
#define MAX_STRING_STR "16"

// this is long enough to hold IPv4 addresses
// (4x3 digits + 3x1 dots + '\0')?
// it should be increased to 40(?!) if IPv6 addresses are used
#define IP_ADDR_SIZE 16

// this is long enough to hold Ethernet MAC addresses
// (2x6 digits + 5x1 colon + '\0')?
#define MAC_ADDR_SIZE 18

#ifndef ETH_SIZE
#define ETH_SIZE 6
#endif

#define FIRST_NODE_ID 0

// maximum data sizes constants
#define MAX_NODES 1024
#define MAX_OBJECTS 1      // 500
#define MAX_ENVIRONMENTS 1 // MAX_NODES*MAX_NODES
#define MAX_MOTIONS 1024
#define MAX_CONNECTIONS MAX_NODES *MAX_NODES // MAX_NODES*MAX_NODES

#ifdef TCHK_ELAPSED
#define TCHK_START(name)            \
    struct timespec name##_prev;    \
    struct timespec name##_current; \
    clock_gettime(CLOCK_MONOTONIC, &name##_prev)

#define TCHK_END(name)                                                       \
    clock_gettime(CLOCK_MONOTONIC, &name##_current);                         \
    time_t name##_sec;                                                       \
    long name##_nsec;                                                        \
    if (name##_current.tv_sec == name##_prev.tv_sec) {                       \
        name##_sec  = name##_current.tv_sec - name##_prev.tv_sec;            \
        name##_nsec = name##_current.tv_nsec - name##_prev.tv_nsec;          \
    } else if (name##_current.tv_sec != name##_prev.tv_sec) {                \
        int name##_carry = 1000000000;                                       \
        name##_sec       = name##_current.tv_sec - name##_prev.tv_sec;       \
        if (name##_prev.tv_nsec > name##_current.tv_nsec) {                  \
            name##_nsec =                                                    \
                name##_carry - name##_prev.tv_nsec + name##_current.tv_nsec; \
            name##_sec--;                                                    \
            if (name##_nsec > name##_carry) {                                \
                name##_nsec = name##_nsec - name##_carry;                    \
                name##_sec++;                                                \
            }                                                                \
        } else {                                                             \
            name##_nsec = name##_current.tv_nsec - name##_prev.tv_nsec;      \
        }                                                                    \
    }                                                                        \
    printf("%s: sec:%lu.%09ld\n", #name, name##_sec, name##_nsec);
#else
#define TCHK_START(name)
#define TCHK_END(name)
#endif

/////////////////////////////////////////////
// Definitions of the main structures
// (used to avoid circular references)
/////////////////////////////////////////////

struct node_class;
struct interface_class;
struct object_class;
struct environment_class;
struct motion_class;
struct connection_class;
struct scenario_class;
struct xml_scenario_class;
struct xml_jpgis_class;

#endif
