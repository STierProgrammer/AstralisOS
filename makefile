include make.conf

.PHONY: directories initrd all clean build iso

all: directories build initrd iso

directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(ISO_ROOT_DIR)
	@mkdir -p $(BOOT_DIR)
	@mkdir -p $(OBJS_DIR)

build:
	make -f build.mk

INITRD_DIR  := initrd/
INITRD_ZIP  := initrd.tar

initrd: directories
	@tar -cvf $(BOOT_DIR)/$(INITRD_ZIP) $(INITRD_DIR)

iso: build
	make -C target/$(ARCH)/$(BOOTLDR)/

run:
	@qemu-system-x86_64 \
		-serial stdio \
		-d int \
		-D qemu.log \
		-no-reboot \
		-no-shutdown \
		-m 2g \
		$(BUILD_DIR)/image.iso

debug:
	@qemu-system-x86_64 \
		-serial stdio \
		-d int \
		-D qemu.log \
		-no-reboot \
		-no-shutdown \
		-m 2g \
		-s -S \
		$(BUILD_DIR)/image.iso

clean:
	rm -rf $(BUILD_DIR)
	
