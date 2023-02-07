#!/usr/bin/env python

import sys
import subprocess
import json

#--------------------------------------------------------

cmd = 'rm -f pmon_omp_exe result.json'
subprocess.call(cmd, shell=True)
cmd = 'gcc pmon_omp.c -o pmon_omp_exe -fopenmp'
subprocess.call(cmd, shell=True)

#--------------------------------------------------------

'''
cmd = 'sudo rdmsr 0x1a4'
out = subprocess.run(cmd, shell=True, encoding='utf-8', stdout=subprocess.PIPE)
print('rdmsr 0x1a4 (original)')
print(out.stdout)

cmd = 'sudo wrmsr -a 0x1a4 0xf' #Xeon
#cmd = 'sudo wrmsr -a 0x1a4 0x7' #Xeon Phi
out = subprocess.run(cmd, shell=True, encoding='utf-8', stdout=subprocess.PIPE)

cmd = 'sudo rdmsr 0x1a4'
out = subprocess.run(cmd, shell=True, encoding='utf-8', stdout=subprocess.PIPE)
print('rdmsr 0x1a4 (after wrmsr)')
print(out.stdout)
'''

#--------------------------------------------------------

'''
#snedo04 Cascade lake
#cpus = range(24)
#snedo01 KNM
#cpus = range(1,69,2)
'''

cpus = [8,25]

#--------------------------------------------------------

result = {}

for core in cpus:
	cond_s = 'Core' + str(core)
	result[cond_s] = {}

	cmd = 'export OMP_NUM_THREADS=2'
	out = subprocess.run(cmd, shell=True, encoding='utf-8', stdout=subprocess.PIPE)
	cmd = 'export GOMP_CPU_AFFINITY="3 '+str(core)+'"'
	out = subprocess.run(cmd, shell=True, encoding='utf-8', stdout=subprocess.PIPE)
	cmd = 'echo $GOMP_CPU_AFFINITY'
	out = subprocess.run(cmd, shell=True, encoding='utf-8', stdout=subprocess.PIPE)

	#cmd = 'sudo numactl -C ' + str(core) + ' -l ' + ' ./pmon_exe'
	cmd = 'sudo numactl -l ' + ' ./pmon_exe'
	out = subprocess.run(cmd, shell=True, encoding='utf-8', stdout=subprocess.PIPE)

	temp_list = out.stdout.split('\n')
	for line in temp_list:
		if 'CHA' in line:
			one_line_list = line.split(',')
			pmc_s = one_line_list[0]
			up = int(one_line_list[1])
			dn = int(one_line_list[2])
			lt = int(one_line_list[3])
			rt = int(one_line_list[4])
			#ac = one_line_list[5] #access count, might be implemented later for ARM

			result[cond_s][pmc_s] = {}
			result[cond_s][pmc_s]['position'] = {}
			result[cond_s][pmc_s]['position']['x'] = 0
			result[cond_s][pmc_s]['position']['y'] = 0
			# reverse property: null: no reverse, h: horizontal rv, v: vertical rv, hv: both
			result[cond_s][pmc_s]['rv'] = ''
			result[cond_s][pmc_s]['up'] = up
			result[cond_s][pmc_s]['dn'] = dn
			result[cond_s][pmc_s]['lt'] = lt
			result[cond_s][pmc_s]['rt'] = rt
		else:
			print(line)
	print('Test'+str(core)+' is done')

#--------------------------------------------------------

f = open('result.json', 'x')
json.dump(result, f, indent='\t')

#--------------------------------------------------------

'''
cmd = 'sudo wrmsr -a 0x1a4 0'
out = subprocess.run(cmd, shell=True, encoding='utf-8', stdout=subprocess.PIPE)

cmd = 'sudo rdmsr 0x1a4'
out = subprocess.run(cmd, shell=True, encoding='utf-8', stdout=subprocess.PIPE)
print('rdmsr 0x1a4 (after wrmsr back to 0)')
print(out.stdout)
'''

#--------------------------------------------------------







