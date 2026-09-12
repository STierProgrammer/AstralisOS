include make.conf

.PHONY: directories initrd all clean build iso

all: directories build initrd iso

directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(ISO_ROOT_DIR)
	@mkdir -p $(BOOT_DIR)
	@mkdir -p $(OBJS_DIR)

include $(BUILD_MK)

INITRD_DIR  := initrd/
INITRD_ZIP  := initrd.tar

initrd: directories
	rm -f $(BOOT_DIR)/$(INITRD_ZIP)
	@tar -cvf $(BOOT_DIR)/$(INITRD_ZIP) $(INITRD_DIR)

iso: build
	make -C boot/target/$(ARCH)/$(BOOTLDR)/

x86_64-run:
	qemu-system-x86_64 \
		-serial stdio \
		-d int \
		-D qemu.log \
		-no-reboot \
		-no-shutdown \
		-m 2g \
		$(BUILD_DIR)/image.iso

aarch64-run:
	qemu-system-aarch64 \
		-M virt \
		-cpu cortex-a72 \
		-serial stdio \
		-d int \
		-D qemu.log \
		-no-reboot \
		-no-shutdown \
		-m 2g \
		-cdrom .build/image.iso

clean:
	rm -rf $(BUILD_DIR)



