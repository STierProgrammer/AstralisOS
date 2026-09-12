.PHONY: all modules

C_SRCS := $(shell find src \
	-path 'src/arch' -prune -o \
	-name '*.c' -print 2>/dev/null)

C_SRCS += $(shell find src/arch/$(ARCH) -name '*.c' 2>/dev/null)

ASM_SRCS := $(shell find src \
	-path 'src/arch' -prune -o \
	-name '*.asm' -print 2>/dev/null)

ASM_SRCS += $(shell find src/arch/$(ARCH) -name '*.asm' 2>/dev/null)

C_OBJS    := $(patsubst src/%.c, $(OBJS_DIR)/$(MODULE_NAME)/%.c.o, $(C_SRCS))
ASM_OBJS  := $(patsubst src/%.asm, $(OBJS_DIR)/$(MODULE_NAME)/%.asm.o, $(ASM_SRCS))
DEPS      := $(patsubst %.o, %.d, $(C_OBJS))

MODULES := $(patsubst %/makefile, %, $(wildcard */makefile))
	
include $(ARCH_MK)

CFLAGS += $(ARCH_CFLAGS)
CPPFLAGS += $(ARCH_CPPFLAGS)
NASMFLAGS += $(ARCH_NASMFLAGS)

ifeq ($(BUILD),debug)
	CFLAGS += $(DEBUG_CFLAGS)
	CPPFLAGS += $(DEBUG_CPPFLAGS)
else ifeq ($(BUILD),release)
	CFLAGS += $(RELEASE_CFLAGS)
	CPPFLAGS += $(RELEASE_CPPFLAGS)
else
$(error Unsupported build type: $(BUILD))
endif

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


