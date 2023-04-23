#include "lib-header/stdtype.h"
#include "lib-header/gdt.h"

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
