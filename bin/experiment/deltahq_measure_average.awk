#!/usr/bin/awk -f
{
    OFMT="%.9f"
}
{
    node_number    += $1;
    loop_count     += $2;
    thread_count   += $3;
    area           += $4;
    width          += $5;
    height         += $6;
    density        += $7;
    all_elapsed    += $8;
    update_elapsed += $9;
    calc_elapsed   += $10;
    bmp_elapsed    += $11;
    meteor_elapsed += $12;
    send_elapsed   += $13;
}
END {
    printf "%d %d %d %.4f %.4f %.4f %.4f %.9f %.9f %.9f %.9f %.9f %.9f\n",
           node_number    ,
           loop_count     ,
           thread_count   ,
           area           ,
           width          ,
           height         ,
           density        ,
           all_elapsed    ,
           update_elapsed ,
           calc_elapsed   ,
           bmp_elapsed    ,
           meteor_elapsed ,
           send_elapsed   > "/dev/stderr";

    printf "%d %d %d %.4f %.4f %.4f %.4f %.9f %.9f %.9f %.9f %.9f %.9f\n",
           node_number    / NR,
           loop_count     / NR,
           thread_count   / NR,
           area           / NR,
           width          / NR,
           height         / NR,
           density        / NR,
           all_elapsed    / NR,
           update_elapsed / NR,
           calc_elapsed   / NR,
           bmp_elapsed    / NR,
           meteor_elapsed / NR,
           send_elapsed   / NR;
}
