#!/usr/bin/env python

import sys
import json

#--------------------------------------------------------
# argv[1] base_file.json
# argv[2] file_includes_already_set_topology_info.json
# argv[3] output file name
# topology_info: pos,rv,cmt for dfv

f_base = open(sys.argv[1], 'r')
f_from = open(sys.argv[2], 'r')
j_base = json.load(f_base)
j_from = json.load(f_from)

for core in j_base:
	for cha in j_base[core]:
		j_base[core][cha]['position']['x'] = j_from[core][cha]['position']['x']
		j_base[core][cha]['position']['y'] = j_from[core][cha]['position']['y']
		j_base[core][cha]['rv'] = j_from[core][cha]['rv']
		if 'cmt' in j_from[core][cha]:
			j_base[core][cha]['cmt'] = j_from[core][cha]['cmt']

f = open(sys.argv[3], 'x')
json.dump(j_base, f, indent='\t')

#--------------------------------------------------------



