#!/usr/bin/awk -f
{
    OFMT="%.9f"
}
{
    node_number        += $1;
    loop_count         += $2;
    search_count       += $3;
    area               += $4;
    width              += $5;
    height             += $6;
    density            += $7;
    neighbor_avg       += $8;
    all_elapsed        += $9;
    parse_elapsed      += $10;
    update_elapsed     += $11;
    search_elapsed     += $12;
    send_elapsed       += $13;
    all_elapsed_avg    += $14;
    parse_elapsed_avg  += $15;
    update_elapsed_avg += $16;
    search_elapsed_avg += $17;
    send_elapsed_avg   += $18;
}
END {
    printf "%d %d %d %.4f %.4f %.4f %.4f %d %.9f %.9f %.9f %.9f %.9f %.9f %.9f %.9f %.9f %.9f\n",
           node_number       ,
           loop_count        ,
           search_count      ,
           area              ,
           width             ,
           height            ,
           density           ,
           neighbor_avg      ,
           all_elapsed       ,
           parse_elapsed     ,
           update_elapsed    ,
           search_elapsed    ,
           send_elapsed      ,
           all_elapsed_avg   ,
           parse_elapsed_avg ,
           update_elapsed_avg,
           search_elapsed_avg,
           send_elapsed_avg  > "/dev/stderr";
}
{
    printf "%d %d %d %.4f %.4f %.4f %.4f %d %.9f %.9f %.9f %.9f %.9f %.9f %.9f %.9f %.9f %.9f\n",
           node_number        / NR,
           loop_count         / NR,
           search_count       / NR,
           area               / NR,
           width              / NR,
           height             / NR,
           density            / NR,
           neighbor_avg       / NR,
           all_elapsed        / NR,
           parse_elapsed      / NR,
           update_elapsed     / NR,
           search_elapsed     / NR,
           send_elapsed       / NR,
           all_elapsed_avg    / NR,
           parse_elapsed_avg  / NR,
           update_elapsed_avg / NR,
           search_elapsed_avg / NR,
           send_elapsed_avg   / NR;
}
