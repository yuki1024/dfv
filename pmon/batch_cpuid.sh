#!/bin/bash

gcc cpuid.c -o cpuid
for i in `seq 0 47` ; do echo -n "$i," ; numactl -C $i ./cpuid ; done




