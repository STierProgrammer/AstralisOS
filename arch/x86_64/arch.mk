
ARCH_CFLAGS := \
		--target=x86_64-elf \
    	-mno-mmx \
		-mno-red-zone \
		-mno-80387 \
		-mno-sse \
		-mno-sse2 \
		-m64 \
		-march=x86-64 \
		-mcmodel=kernel
ARCH_CPPFLAGS := \
		-DARCH_X86_64
ARCH_NASMFLAGS := \
			-f elf64

