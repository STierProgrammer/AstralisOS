#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <fs/tar.h>
#include <fs/vfs.h>
#include <misc/debug.h>

#define USTAR_OCTAL_DIGITS 11

static int64_t ustar_octtoi(const char *str)
{
    uint64_t res = 0;
    for (size_t i = 0; i < USTAR_OCTAL_DIGITS; ++i)
    {
        if (str[i] < '0' || str[i] > '8') return -1;
        res *= 8;
        res += str[i] - '0';
    }
    return res;
}

// :kekw: also taken idea from: https://github.com/tayoky/stanix/blob/main/kernel/fs/tarfs.c#L38
void tar_extract(void *tar)
{
    char *addr = tar;
    while (!memcmp(((tar_header_t*)addr)->magic, "ustar", 5))
    {
        tar_header_t *curr_file = (tar_header_t*)addr;
        char full_path[strlen(curr_file->name) + strlen("/") + 1];
        sprintf(full_path, "/%s", curr_file->name); 

        size_t file_size = ustar_octtoi(curr_file->size);
        if (curr_file->typeflag == TAR_DIR)
        {
            path_t path = vfs_path_from_abs(full_path);
            inode_t *inode = NULL;
            int e;
            if ((e = vfs_create(&path, INODE_DIR, &inode)) < 0)
            {
                srdebug(tar_extract, "Failed to create a dir: %d", e);
                break;
            }
        } else if (curr_file->typeflag == TAR_FILE)
        {
            path_t path = vfs_path_from_abs(full_path);
            inode_t *inode = NULL;
            int e;
            if ((e = vfs_create(&path, INODE_FILE, &inode)) < 0)
            {
                srdebug(tar_extract, "Failed to create a file: %d", e);
                break;
            }
            inode_write(inode, addr + 512, file_size, 0);
            inode->size = file_size;
        }

        addr += (((uint64_t)file_size + 1023) / 512) * 512;
    }
}


