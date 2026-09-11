#include <kernel.h>
#include <modules.h>
#include <fs/initrd.h>
#include <fs/vfs.h>
#include <fs/tar.h>
#include <string.h>

static inode_t *initrd_inode = NULL;

void initrd_init(void)
{
    const path_t initrd_path = vfs_path_from_abs("/initrd");

    vfs_create(&initrd_path, INODE_DIR, &initrd_inode);
    if (!initrd_inode)
    {
        return;
    }

    module_t module;
    module_find(&bootctx(modules), &module, "/boot/initrd.tar");
    tar_extract(module.addr);
}

int initrd_get(const char *name, inode_t **ret)
{
    int e = inode_lookup(initrd_inode, name, strlen(name), ret);

    return e;
}
