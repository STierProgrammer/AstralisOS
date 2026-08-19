#include <stdint.h>


static inline long syscall1(long n, long a1){
	long ret;
	asm volatile ("int $0x80" : "=a"(ret) : "a"(n), "D"(a1) : "memory");
	return ret;
}

int main()
{
    int *ptr1 = (int*)syscall1(4, sizeof(int));
    *ptr1 = 4;
    int *ptr2 = (int*)syscall1(4, sizeof(int));
    *ptr2 = 5;

    return 0;
}

void _start(void)
{
    main();
    while (1);
}


