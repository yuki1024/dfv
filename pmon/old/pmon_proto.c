//prototype of pmon.c
//This code includes codes for pci config space, cpuid, msr __asm__() 2 kind versions


#include <stdio.h>
#include <stdint.h> // standard integer types like uint64_t
#include <stdlib.h> // for exit()
#include <fcntl.h> // for open()
#include <sys/mman.h> // for mmap()
#include <assert.h> // for assert()
#include <unistd.h> // for pread()

#define CPU_NUM 24

#define IA32_TIME_STAMP_COUNTER 0x10
#define U_MSR_PMON_GLOBAL_STATUS 0x0701
#define U_MSR_PMON_GLOBAL_CTL 0x0700
#define C0_MSR_PMON_UNIT_CTL 0x0e00
#define C0_MSR_PMON_UNIT_STATUS 0x0e07
#define C0_MSR_PMON_CTL0 0x0e01
#define C0_MSR_PMON_CTR0 0x0e08

/*
uint32_t PCI_cfg_index(unsigned int Bus, unsigned int Device, unsigned int Function, unsigned int Offset){
	uint32_t byteaddress;
	uint32_t index;

	// assert (Bus == BUS);
	assert (Device >= 0);
	assert (Function >= 0);
	assert (Offset >= 0);
	assert (Device < (1<<5));
	assert (Function < (1<<3));
	assert (Offset < (1<<12));

#ifdef DEBUG
	fprintf(log_file,"Bus,(Bus<<20)=%x\n",Bus,(Bus<<20));
	fprintf(log_file,"Device,(Device<<15)=%x\n",Device,(Device<<15));
	fprintf(log_file,"Function,(Function<<12)=%x\n",Function,(Function<<12));
	fprintf(log_file,"Offset,(Offset)=%x\n",Offset,Offset);
#endif
	byteaddress = (Bus<<20) | (Device<<15) | (Function<<12) | Offset;
	index = byteaddress / 4;
	return ( index );
}
*/


int main(){
	int i;
	char filename[100];
	int msr_fd[CPU_NUM];
	uint64_t msr_val;
	int mem_fd;

	printf("Start\n");

	for(i=0; i<1; i++){
	//for(i=0; i<CPU_NUM; i++){
		sprintf(filename, "/dev/cpu/%d/msr", i);
		msr_fd[i] = open(filename, O_RDWR);
		if(msr_fd[i] == -1){
			printf("Error when trying to open msr file\n");
			exit(1);
		}
		
		//See if they work correctly
		pread(msr_fd[i], &msr_val, sizeof(msr_val), IA32_TIME_STAMP_COUNTER);
		printf("Test: TSC on core %d is %ld\n", i, msr_val);

		pread(msr_fd[i], &msr_val, sizeof(msr_val), U_MSR_PMON_GLOBAL_CTL);
		printf("Test: U_MSR_PMON_GLOBAL_CTL: %d, val: %llx\n", i, msr_val);

		pread(msr_fd[i], &msr_val, sizeof(msr_val), U_MSR_PMON_GLOBAL_STATUS);
		printf("Test: U_MSR_PMON_GLOBAL_STATUS: %d, val: %llx\n", i, msr_val);
	}


	pread(msr_fd[0], &msr_val, sizeof(msr_val), C0_MSR_PMON_UNIT_CTL);
	printf("Test: %llx\n", msr_val);
	pread(msr_fd[0], &msr_val, sizeof(msr_val), C0_MSR_PMON_UNIT_STATUS);
	printf("Test: %llx\n", msr_val);
	pread(msr_fd[0], &msr_val, sizeof(msr_val), C0_MSR_PMON_CTL0);
	printf("Test: %llx\n", msr_val);
	pread(msr_fd[0], &msr_val, sizeof(msr_val), C0_MSR_PMON_CTR0);
	printf("Test: %llx\n", msr_val);


	msr_val = 1ULL << 63; //frz_all
	pwrite(msr_fd[0], &msr_val, sizeof(msr_val), U_MSR_PMON_GLOBAL_CTL);

/*
	uint64_t en = 1ULL << 22; //en->1
	uint64_t ev_sel = 0xaaULL; //ev_sel
	uint64_t umask = 3ULL << 8; //umask
	msr_val = en + ev_sel + umask;
	pwrite(msr_fd[0], &msr_val, sizeof(msr_val), C0_MSR_PMON_CTL0);

	msr_val = 1ULL << 61; //unfrz_all
	pwrite(msr_fd[0], &msr_val, sizeof(msr_val), U_MSR_PMON_GLOBAL_CTL);

	//measure
	double *a = (double *)malloc(sizeof(double)*1000);
	double *b = (double *)malloc(sizeof(double)*1000);
	double *c = (double *)malloc(sizeof(double)*1000);

	int j;
	for(j=0; j<1000; j++){
		a[j] = 5.0;
		b[j] = 7.0;
		c[j] = 11.0;
	}
	double scalar = 3.0;

	for(j=0; j<1000; j++){
		a[j] = b[j] + scalar * c[j];
	}

*/
	pread(msr_fd[0], &msr_val, sizeof(msr_val), C0_MSR_PMON_CTR0);
	printf("Test: %llx\n", msr_val);









	//uint32_t a,b,c,d;

	/*
	__asm__(
		"movl $0, %%eax \n\t"
		"cpuid \n\t"
		"movl %%eax, %0 \n\t"
		"movl %%ebx, %1 \n\t"
		"movl %%ecx, %2 \n\t"
		"movl %%edx, %3 \n\t"
		: "=r"(a), "=r"(b), "=r"(c), "=r"(d)
	);
	*/

	/*
	__asm__("movl $0, %eax");
	__asm__("cpuid");
	__asm__("movl %%eax, %0" : "=r"(a));
	__asm__("movl %%ebx, %0" : "=r"(b));
	__asm__("movl %%ecx, %0" : "=r"(c));
	__asm__("movl %%edx, %0" : "=r"(d));
	*/

	/*
	__asm__("movl $10, %eax");
	__asm__("cpuid");
	__asm__("movl %%eax, %0" : "=r"(a));
	__asm__("movl %%ebx, %0" : "=r"(b));
	__asm__("movl %%ecx, %0" : "=r"(c));
	__asm__("movl %%edx, %0" : "=r"(d));

	//printf("CPUID dump: %lx %lx %lx %lx \n", a,b,c,d);
	*/






/*


	// DOUBLE-CHECK THIS ON NEW SYSTEMS!!!!!   grep MMCONFIG /proc/iomem | awk -F- '{print $1}'
	//unsigned long mmconfig_base=0x80000000;
	unsigned long mmconfig_base=0xe0000000;
	unsigned long mmconfig_size=0x10000000;

	sprintf(filename, "/dev/mem");
	mem_fd = open(filename, O_RDWR);
	if(mem_fd == -1){
		printf("Error when trying to open /dev/mem file\n");
		exit(1);
	}

	int map_prot = PROT_READ | PROT_WRITE;
	unsigned int *mmconfig_ptr = mmap(NULL, mmconfig_size, map_prot, MAP_SHARED, mem_fd, mmconfig_base);
	if(mmconfig_ptr == MAP_FAILED){
		printf("Error mmap mmconfig_ptr\n");
		exit(-1);
	}
	close(mem_fd);

	uint32_t bus = 0x00;
	uint32_t device = 0x5;
	uint32_t function = 0x0;
	uint32_t offset = 0x0;
	uint32_t index = PCI_cfg_index(bus, device, function, offset);
	uint32_t value = mmconfig_ptr[index];
	if(value == 0x20248086){
		printf("GOOD\n");
	} else {
		printf("BAD\n");
	}
*/

	printf("End\n");

	return 0;
}






