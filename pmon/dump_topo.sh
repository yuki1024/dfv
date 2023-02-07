#!/bin/bash

DATA=""
DATA+="core_cpus "
DATA+="core_cpus_list "
DATA+="core_id "
DATA+="core_siblings "
DATA+="core_siblings_list "
DATA+="die_cpus "
DATA+="die_cpus_list "
DATA+="die_id "
DATA+="package_cpus "
DATA+="package_cpus_list "
DATA+="physical_package_id "
DATA+="thread_siblings "
DATA+="thread_siblings_list "

echo -n "core_id,"
echo "${DATA// /;}"

for i in `seq 0 47`
do
	echo -n "$i,"
	for d in ${DATA}
	do
		DIR="/sys/devices/system/cpu/cpu${i}/topology"
		echo -n `cat ${DIR}/${d}`
		echo -n ";"
	done
	echo ""
done




