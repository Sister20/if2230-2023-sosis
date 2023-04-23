#include "lib-header/stdtype.h"
#include "lib-header/gdt.h"
#include "lib-header/interrupt.h"

/**
 * global_descriptor_table, predefined GDT.
 * Initial SegmentDescriptor already set properly according to GDT definition in Intel Manual & OSDev.
 * Table entry : [{Null Descriptor}, {Kernel Code}, {Kernel Data (variable, etc)}, ...].
 */
struct GlobalDescriptorTable global_descriptor_table = {
    .table = {
        {// Null Descriptor
         .segment_low = 0,
         .base_low = 0,
         .base_mid = 0,
         .base_high = 0,
         .type_bit = 0,
         .non_system = 0,
         .dpl = 0,
         .present = 0,
         .limit = 0,
         .op_size = 0,
         .glanularity = 0,
         },
        {// Kernel Code
         .segment_low = 0xFFFF,
         .base_low = 0,
         .base_mid = 0,
         .base_high = 0,
         .type_bit = 0xA,
         .non_system = 1,
         .dpl = 0,
         .present = 1,
         .limit = 0xF,
         .op_size = 1,
         .glanularity = 1,
         },
        {// Kernel Data
         .segment_low = 0xFFFF,
         .base_low = 0,
         .base_mid = 0,
         .base_high = 0,
         .type_bit = 0x2,
         .non_system = 1,
         .dpl = 0,
         .present = 1,
         .limit = 0xF,
         .op_size = 1,
         .glanularity = 1,
         },
         {// User Code
         .segment_low = 0xFFFF,
         .base_low = 0,
         .base_mid = 0,
         .base_high = 0,
         .type_bit = 0xA,
         .non_system = 1,
         .dpl = 0x3,
         .present = 1,
         .limit = 0xF,
         .op_size = 1,
         .glanularity = 1,
         },
        {// User Data
         .segment_low = 0xFFFF,
         .base_low = 0,
         .base_mid = 0,
         .base_high = 0,
         .type_bit = 0x2,
         .non_system = 1,
         .dpl = 0x3,
         .present = 1,
         .limit = 0xF,
         .op_size = 1,
         .glanularity = 1,
         },
         {
            .limit      = (sizeof(struct TSSEntry) & (0xF << 16)) >> 16,
            .segment_low       = sizeof(struct TSSEntry),
            .base_high         = 0,
            .base_mid          = 0,
            .base_low          = 0,
            .non_system        = 0,    // S bit
            .type_bit          = 0x9,
            .dpl         = 0,    // DPL
            .present         = 1,    // P bit
            .op_size        = 1,    // D/B bit
            .code_segment         = 0,    // L bit
            .glanularity       = 0,    // G bit
        },
        {0},
    }};

/**
 * _gdt_gdtr, predefined system GDTR.
 * GDT pointed by this variable is already set to point global_descriptor_table above.
 * From: https://wiki.osdev.org/Global_Descriptor_Table, GDTR.size is GDT size minus 1.
 */
struct GDTR _gdt_gdtr = {
    .size = sizeof(struct GlobalDescriptorTable) - 1,
    .address = &global_descriptor_table,
};

void gdt_install_tss(void) {
    uint32_t base = (uint32_t) &_interrupt_tss_entry;
    global_descriptor_table.table[5].base_high = (base & (0xFF << 24)) >> 24;
    global_descriptor_table.table[5].base_mid  = (base & (0xFF << 16)) >> 16;
    global_descriptor_table.table[5].base_low  = base & 0xFFFF;
}
