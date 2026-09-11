.PHONY: all
MODULE_NAME := limine

ifeq ($(wildcard limine/limine.h),)
$(shell rm -rf limine && git clone https://github.com/stierium/stierium-limine-binaries limine)
endif

include ../../../../make.conf
include $(BUILD_MK)

ALL_OBJS = $(shell find $(OBJS_DIR) -name '*.o')

CPPFLAGS    := -I./limine/ \
			   -I$(ROOT_DIR)/boot/ \
			   -D __protocol_$(PROTOCOL)__ \
			   -MMD \
			   -MP

CFLAGS      := -Wall -Wextra -std=gnu11 -ffreestanding -fno-stack-protector \
               -fno-stack-check -fno-PIC -m64 -march=x86-64 -mno-80387 \
               -mno-mmx -mno-sse -mno-sse2 -mno-red-zone -mcmodel=kernel -g

LDFLAGS     := -Wl,-m,elf_x86_64 -Wl,--build-id=none -nostdlib -static \
               -z max-page-size=0x1000 -T ./linker.ld -fuse-ld=lld

KERNEL      := kernel
ISO         := $(BUILD_DIR)/image.iso 

all: $(ISO)

$(KERNEL): $(ALL_OBJS)
	$(CC)  $(CFLAGS) $(LDFLAGS) $(ALL_OBJS) -o $(KERNEL)

$(ISO): $(KERNEL) limine.conf
	@mkdir -p $(ISO_ROOT_DIR)/boot/limine $(ISO_ROOT_DIR)/EFI/BOOT
	@mv $(KERNEL) $(ISO_ROOT_DIR)/boot/$(KERNEL)
	@cp -v ./limine.conf $(ISO_ROOT_DIR)/boot/limine/
	@cp -v ./limine/limine-bios.sys \
	       ./limine/limine-bios-cd.bin \
	       ./limine/limine-uefi-cd.bin \
	       $(ISO_ROOT_DIR)/boot/limine/
	@cp -v ./limine/BOOTX64.EFI \
	       ./limine/BOOTIA32.EFI \
	       $(ISO_ROOT_DIR)/EFI/BOOT/
	@xorriso -as mkisofs -R -r -J \
	         -b boot/limine/limine-bios-cd.bin \
	         -no-emul-boot -boot-load-size 4 -boot-info-table \
	         -hfsplus -apm-block-size 2048 \
	         --efi-boot boot/limine/limine-uefi-cd.bin \
	         -efi-boot-part --efi-boot-image \
	         --protective-msdos-label \
	         $(ISO_ROOT_DIR) -o $(ISO)
	@./limine/limine bios-install $(ISO)

