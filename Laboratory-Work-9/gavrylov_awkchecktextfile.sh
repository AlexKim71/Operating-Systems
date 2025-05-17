#!/usr/bin/awk -f
NR == 3 {print $1, $3, $11}
