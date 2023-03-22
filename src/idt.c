#include "lib-header/stdtype.h"
#include "lib-header/idt.h"

struct InterruptDescriptorTable interrupt_descriptor_table = {
    .table = {
        {
            // Null Descriptor
            .offset_low = 0,
            .segment = 0,
            ._reserved = 0,
            ._r_bit_1 = 0,
            ._r_bit_2 = 0,
            .gate_32 = 0,
            ._r_bit_3 = 0,
            .dpl = 0,
            .p = 0
        },
        {
            // Kernel Code
            .offset_low = 0,
            .segment = 0x08, // Segment selector for kernel code
            ._reserved = 0,
            ._r_bit_1 = 1,
            ._r_bit_2 = 1,
            .gate_32 = 0x9A, // 32-bit interrupt gate
            ._r_bit_3 = 0,
            .dpl = 0,
            .p = 1
        },
        {
            // Kernel Data
            .offset_low = 0,
            .segment = 0x10, // Segment selector for kernel data
            ._reserved = 0,
            ._r_bit_1 = 1,
            ._r_bit_2 = 1,
            .gate_32 = 0x92, // 32-bit interrupt gate
            ._r_bit_3 = 0,
            .dpl = 0,
            .p = 1
        },
    }
};

struct IDTR _idt_idtr = {
    .size = sizeof(struct InterruptDescriptorTable) - 1,
    .address = &interrupt_descriptor_table,
};

