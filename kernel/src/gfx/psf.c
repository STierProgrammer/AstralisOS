#include <stdint.h>

#include <gfx/psf.h>
#include <gfx/font.h>
#include <fs/vfs.h>
#include <errno.h>

int psf_load(gfx_font_t **font, const path_t path)
{
#if 0
    inode_t *inode = NULL;
    int e;
    if ((e = vfs_lookup(path, &inode)) < 0)
    {
        *font = NULL;
        return e;
    }

    psf1_header_t header;
    long read;
    if ((read = inode_read(inode, &header, sizeof(header), 0)) < 0)
    {
        *font = NULL;
        return read;
    }

    if (read != sizeof(header))
    {
        *font = NULL;
        return -EINVAL;
    }

    if (header.magic != PSF1_FONT_MAGIC)
    {
        *font = NULL;
        return -EINVAL;
    }
    
#endif
}



