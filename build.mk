.PHONY: all modules

C_SRCS		:= $(shell find src -name '*.c')
ASM_SRCS 	:= $(shell find src -name '*.asm')

C_OBJS    := $(patsubst src/%.c, $(OBJS_DIR)/$(MODULE_NAME)/%.c.o, $(C_SRCS))
ASM_OBJS  := $(patsubst src/%.asm, $(OBJS_DIR)/$(MODULE_NAME)/%.asm.o, $(ASM_SRCS))
DEPS      := $(patsubst %.o, %.d, $(C_OBJS))

MODULES := $(patsubst %/makefile, %, $(wildcard */makefile))
	
all: $(C_OBJS) $(ASM_OBJS) modules
	@echo $(MODULES)

modules:
	@for m in $(MODULES); do \
		make -C $$m; \
	done

$(OBJS_DIR)/$(MODULE_NAME)/%.c.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJS_DIR)/$(MODULE_NAME)/%.asm.o: src/%.asm
	@mkdir -p $(dir $@)
	$(NASM) $(NASMFLAGS) $< -o $@

-include $(DEPS)

