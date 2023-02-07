#!/bin/bash

rm -rf pmon_exe
gcc pmon.c -o pmon_exe

rm -rf result.csv
for i in `seq 0 23` ; do
	echo "Core${i}" >> result.csv
	sudo numactl -C ${i} -l ./pmon_exe >> result.csv
	echo "Core${i} done"
done

echo "All done"




