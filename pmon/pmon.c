#include <stdio.h>
#include <stdint.h> // standard integer types like uint64_t
#include <stdlib.h> // for exit()
#include <fcntl.h> // for open()
#include <sys/mman.h> // for mmap()
#include <assert.h> // for assert()
#include <unistd.h> // for pread()
#include <x86intrin.h> // for _mm_clflush()

//msr addresses
//There base msr offset nums are in common between Xeon-SP (Skylake SP, Cascade Lake SP) and Xeon Phi (KNL, KNM)
#define IA32_TIME_STAMP_COUNTER 0x10
#define U_MSR_PMON_GLOBAL_CTL 0x0700
#define U_MSR_PMON_GLOBAL_STATUS 0x0701

#define C0_MSR_PMON_UNIT_CTL 0x0e00
#define C0_MSR_PMON_UNIT_STATUS 0x0e07
#define C0_MSR_PMON_CTL0 0x0e01
//To CTL3, increment +0x1
#define C0_MSR_PMON_CTR0 0x0e08
//To CTL3, increment +0x1

//The only one difference is diff of CHA offsets (Xeon: +0x10, Xeon Phi: +0x0c)
//Xeon: To CHA27, increment +0x10 from the base CHA0 addresses
#define CHA_DIFF_OFFSET 0x10
//Xeon Phi: To CHA37, increment +0x0c from the base CHA0 addresses
//#define CHA_DIFF_OFFSET 0x0c

//ev_sel and umask
#define VERT_RING_X_IN_USE 0xaa //BL - data
//#define VERT_RING_X_IN_USE 0xa6 //AD
//#define VERT_RING_X_IN_USE 0xa8 //AK
//#define VERT_RING_X_IN_USE 0xac //IV
#define UMASK_UP 0x3
#define UMASK_DOWN 0xc
#define HORZ_RING_X_IN_USE 0xab //BL - data
//#define HORZ_RING_X_IN_USE 0xa7 //AD
//#define HORZ_RING_X_IN_USE 0xa9 //AK
//#define HORZ_RING_X_IN_USE 0xad //IV
#define UMASK_LEFT 0x3
#define UMASK_RIGHT 0xc

#define REG_NUM_PER_BLOCK 4

#define CHA_NUM 28
//#define CHA_NUM 38

#define STREAM_ARRAY_SIZE 125000000 //1GB buffer size per array

//#define DEBUG

void init(double *a, double *b, double *c){
	int j;
	for(j=0; j<STREAM_ARRAY_SIZE; j++){
		a[j] = 5.0;
		b[j] = 7.0;
		c[j] = 11.0;
	}
}

void flush(double *a, double *b, double *c){
	int j;
	for(j=0; j<STREAM_ARRAY_SIZE; j+=8){
		_mm_clflush(a+j);
		_mm_clflush(b+j);
		_mm_clflush(c+j);
	}
}

void stream(double *a, double *b, double *c){
	int j;
	double scalar = 3.0;
	for(j=0; j<STREAM_ARRAY_SIZE; j++){
		a[j] = b[j] + scalar * c[j];
	}
}

int main(){
	int i,j;
	char filename[100];
	int msr_fd;
	uint64_t msr_val;
	int mem_fd;
	uint64_t en, rst, ev_sel, umask;
	uint64_t ctr[CHA_NUM][REG_NUM_PER_BLOCK];

	//preparation
	double *a = (double *)malloc(sizeof(double)*STREAM_ARRAY_SIZE);
	double *b = (double *)malloc(sizeof(double)*STREAM_ARRAY_SIZE);
	double *c = (double *)malloc(sizeof(double)*STREAM_ARRAY_SIZE);
	init(a,b,c);
	flush(a,b,c);

	sprintf(filename, "/dev/cpu/0/msr"); //cpu0
	msr_fd = open(filename, O_RDWR);
	if(msr_fd == -1){
		printf("Error when trying to open msr file\n");
		exit(1);
	}

#ifdef DEBUG

	//See how they work
	pread(msr_fd, &msr_val, sizeof(msr_val), IA32_TIME_STAMP_COUNTER);
	printf("IA32_TIME_STAMP_COUNTER: %ld\n", msr_val);
	pread(msr_fd, &msr_val, sizeof(msr_val), U_MSR_PMON_GLOBAL_CTL);
	printf("U_MSR_PMON_GLOBAL_CTL: 0x%lx\n", msr_val);
	pread(msr_fd, &msr_val, sizeof(msr_val), U_MSR_PMON_GLOBAL_STATUS);
	printf("U_MSR_PMON_GLOBAL_STATUS: 0x%llx\n", msr_val);
	pread(msr_fd, &msr_val, sizeof(msr_val), C0_MSR_PMON_UNIT_CTL);
	printf("C0_MSR_PMON_UNIT_CTL: 0x%llx\n", msr_val);
	pread(msr_fd, &msr_val, sizeof(msr_val), C0_MSR_PMON_UNIT_STATUS);
	printf("C0_MSR_PMON_UNIT_STATUS: 0x%llx\n", msr_val);
	pread(msr_fd, &msr_val, sizeof(msr_val), C0_MSR_PMON_CTL0);
	printf("C0_MSR_PMON_CTL0: 0x%llx\n", msr_val);
	pread(msr_fd, &msr_val, sizeof(msr_val), C0_MSR_PMON_CTR0);
	printf("C0_MSR_PMON_CTR0: 0x%llx\n", msr_val);

#else

	//freeze all counters
	msr_val = 1ULL << 63; //frz_all->1
	pwrite(msr_fd, &msr_val, sizeof(msr_val), U_MSR_PMON_GLOBAL_CTL);

	//reset & check before measurement
	for(i=0; i<CHA_NUM; i++){
		for(j=0; j<REG_NUM_PER_BLOCK; j++){
			//forced reset
			msr_val = 0;
			pwrite(msr_fd, &msr_val, sizeof(msr_val), C0_MSR_PMON_CTR0 + CHA_DIFF_OFFSET*i + 0x01*j);

			pread(msr_fd, &msr_val, sizeof(msr_val), C0_MSR_PMON_CTR0 + CHA_DIFF_OFFSET*i + 0x01*j);
			ctr[i][j] = msr_val;
		}
	}

	//enable counters and set event/umask
	for(i=0; i<CHA_NUM; i++){
		for(j=0; j<REG_NUM_PER_BLOCK; j++){
			if(j==0){
				ev_sel = VERT_RING_X_IN_USE; //ev_sel
				umask = UMASK_UP << 8; //umask
			} else if(j==1){
				ev_sel = VERT_RING_X_IN_USE; //ev_sel
				umask = UMASK_DOWN << 8; //umask
			} else if(j==2){
				ev_sel = HORZ_RING_X_IN_USE; //ev_sel
				umask = UMASK_LEFT << 8; //umask
			} else if(j==3){
				ev_sel = HORZ_RING_X_IN_USE; //ev_sel
				umask = UMASK_RIGHT << 8; //umask
			}
			en = 1 << 22; //en->1
			rst = 1 << 17; //rst->1 //somehow this doesn't work
			msr_val = en + rst + ev_sel + umask;
			pwrite(msr_fd, &msr_val, sizeof(msr_val), C0_MSR_PMON_CTL0 + CHA_DIFF_OFFSET*i + 0x01*j);
		}
	}

	//unfreeze all counters
	msr_val = 1ULL << 61; //unfrz_all->1
	pwrite(msr_fd, &msr_val, sizeof(msr_val), U_MSR_PMON_GLOBAL_CTL);

	//measure
	stream(a,b,c);

	//freeze all counters
	msr_val = 1ULL << 63; //frz_all->1
	pwrite(msr_fd, &msr_val, sizeof(msr_val), U_MSR_PMON_GLOBAL_CTL);

	//check after measurement
	for(i=0; i<CHA_NUM; i++){
		printf("CHA%d", i);
		for(j=0; j<REG_NUM_PER_BLOCK; j++){
			pread(msr_fd, &msr_val, sizeof(msr_val), C0_MSR_PMON_CTR0 + CHA_DIFF_OFFSET*i + 0x01*j);
			//printf("REALVAL: START : CHA%d CTR%d: %d\n", i, j, ctr[i][j]);
			//printf("REALVAL: END   : CHA%d CTR%d: %d\n", i, j, msr_val);
			ctr[i][j] = msr_val - ctr[i][j];
			//printf("DIFFVAL: ST-ED : CHA%2d CTR%d: %d\n", i, j, ctr[i][j]);
			printf(",%d", ctr[i][j]);
		}
		printf("\n"); //a line: CHA_NUM,UP,DOWN,LEFT,RIGHT
	}

#endif

	return 0;
}






