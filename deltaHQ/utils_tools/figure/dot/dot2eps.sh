#!/bin/sh
dot $2 $3 $4 $5 $6 -T eps $1 -o $1.eps && open $1.eps
