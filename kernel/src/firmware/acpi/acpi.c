#include <kernel.h>

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <firmware/acpi/acpi.h>
#include <firmware/acpi/fadt.h>
#include <firmware/acpi/madt.h>
#include <misc/debug.h>

#define ACPI_VERSION_2 2

static rsdp_t *rsdp = NULL;
static xsdp_t *xsdp = NULL;

static bool do_checksum(acpi_sdt_header *header)
{
    uint8_t sum = 0;
    for (size_t i = 0; i < header->length; ++i)
    {
        sum += ((char*)header)[i];
    }

    return sum == 0;
}

static inline void* acpi_get_rsdp(void)
{
    return (void*)bootctx(acpi_rsdp).addr;
}

void *acpi_find_sdt(const char signature[4])
{   
    if (rsdp->revision >= ACPI_VERSION_2)
    {
        xsdt_t *xsdt = (void*)to_vaddr(xsdp->xsdt_addr);
        if (xsdt->header.length < sizeof(acpi_sdt_header)) return NULL;
        if (!do_checksum(&xsdt->header)) return NULL;

        size_t num_entries = (xsdt->header.length - sizeof(acpi_sdt_header)) / sizeof(uint64_t);
        for (size_t i = 0; i < num_entries; ++i)
        {
            acpi_sdt_header *header = (void*)to_vaddr(xsdt->sdts[i]);
            if (strncmp(header->signature, signature, 4) == 0) return (void*)header;
        }
        return NULL;
    } else {
        rsdt_t *rsdt = (void*)to_vaddr(rsdp->rsdt_addr);
        if (rsdt->header.length < sizeof(acpi_sdt_header)) return NULL;
        if (!do_checksum(&rsdt->header)) return NULL;
        
        size_t num_entries = (rsdt->header.length - sizeof(acpi_sdt_header)) / sizeof(uint32_t);
        for (size_t i = 0; i < num_entries; ++i)
        {
            acpi_sdt_header *header = (void*)to_vaddr(rsdt->sdts[i]);   
            if (strncmp(header->signature, signature, 4) == 0) return (void*)header;
        }
        return NULL;
    }
}

fadt_t *fadt = NULL;
madt_t *madt = NULL;

volatile uint64_t lapic_addr = 0;
volatile uint64_t ioapic_addr = 0;

volatile uint64_t ioapic_gsi_base = 0;

void acpi_init(void)
{
    rsdp = acpi_get_rsdp(); 
    xsdp = acpi_get_rsdp();
    
    fadt_t *fadt_header = acpi_find_sdt("FACP");
    if (!fadt_header)
    {
        // NO FADT found
        return;
    }

    acpi_sdt_header *madt_header = acpi_find_sdt("APIC");
    if (madt_header)
    {
        madt = (madt_t*)madt_header;

        lapic_addr = madt->lapic_addr;

        char *curr = (char*)madt + sizeof(madt_t);
        char *end  = (char*)madt + madt->header.length;
        while (curr < end)
        {
            madt_entry_t *entry = (madt_entry_t*)curr;
            switch (entry->entry_type) {
            case MADT_ENTRY_TYPE_IO_APIC:
            {
                ioapic_addr = entry->ioapic.ioapic_addr;
                ioapic_gsi_base = entry->ioapic.gsi_base;
                break;
            }
            case MADT_ENTRY_TYPE_LAPIC_ADDR_OVERRIDE:
            {
                lapic_addr = entry->lapic_addr_override.lapic_addr;
                break;   
            }
            default:
                break;  
            }

            curr += entry->record_len;
        }
    } else {
        debug("MADT has not been found!");
    }

    info("Initalized!");
}   
