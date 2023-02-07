#include <stdio.h>
#include <stdint.h> // standard integer types like uint64_t

int main(){
	uint32_t a,b,c,d;

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

	__asm__("movl $11, %eax");
	__asm__("cpuid");
	__asm__("movl %%eax, %0" : "=r"(a));
	__asm__("movl %%ebx, %0" : "=r"(b));
	__asm__("movl %%ecx, %0" : "=r"(c));
	__asm__("movl %%edx, %0" : "=r"(d));

	//printf("CPUID dump: %lx %lx %lx %lx \n", a,b,c,d);
	printf("%ld\n", d);

	return 0;
}






