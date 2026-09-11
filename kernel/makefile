include ../make.conf 

CFLAGS    		:= -Wall -Wextra -std=gnu11 \
					-ffreestanding -fno-stack-protector \
             		-fno-stack-check -fno-PIC -m64 -march=x86-64 \
             		-mno-mmx -mno-red-zone -mcmodel=kernel -g \
					-mno-80387 -mno-sse -mno-sse2
CPPFLAGS  		:=  -I$(ROOT_DIR)/boot/ \
					-I$(ROOT_DIR)/kernel/include \
					-D __ARCH_X86_64__ \
					-D __DEBUG__ \
					-MMD \
					-MP

NASMFLAGS 		:= -Wall -f elf64  

MODULE_NAME := kernel
include $(BUILD_MK)
