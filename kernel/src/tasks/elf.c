#include <tasks/elf.h>

bool elf_verify(Elf64_Ehdr *hdr)
{
    if (!hdr)
        return false;

    if (hdr->e_ident[EI_MAG0] != ELFMAG0)
        return false;

    if (hdr->e_ident[EI_MAG1] != ELFMAG1)
        return false;

    if (hdr->e_ident[EI_MAG2] != ELFMAG2)
        return false;

    if (hdr->e_ident[EI_MAG3] != ELFMAG3)
        return false;

    return true;
}

bool elf_supported(Elf64_Ehdr *hdr)
{
    if (!elf_verify(hdr))
        return false;

    if (hdr->e_ident[EI_CLASS] != ELFCLASS64)
        return false;

    if (hdr->e_ident[EI_DATA] != ELFDATA2LSB)
        return false;

    return true;
}


