include ../../make.conf

CFLAGS    		:= -Wall -Werror -Wextra -std=gnu11 	-ffreestanding -fno-stack-protector \
             		-fno-stack-check -fno-PIC -m64 -march=x86-64 \
             		-mno-mmx -mno-red-zone -mcmodel=kernel -g \
					-mno-80387 -msse 
CPPFLAGS  		:=  -MMD -MP -Iinclude -I$(ROOT_DIR)/libc/include/
NASMFLAGS 		:= -Wall -Werror -f elf64 

MODULE_NAME := libds
include $(BUILD_MK)
