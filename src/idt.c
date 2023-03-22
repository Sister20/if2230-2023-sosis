#include "lib-header/stdtype.h"
#include "lib-header/idt.h"

void *isr_stub_table[ISR_STUB_TABLE_LIMIT];

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
            .gate_32 = 0, // 32-bit interrupt gate
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
            .gate_32 = 0, // 32-bit interrupt gate
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

void initialize_idt(void) {
    for (int i = 0; i < ISR_STUB_TABLE_LIMIT; i++) {
        set_interrupt_gate(i, isr_stub_table[i], GDT_KERNEL_CODE_SEGMENT_SELECTOR, 0);
    }
    __asm__ volatile("lidt %0" : : "m"(_idt_idtr));
    __asm__ volatile("sti");
}

void set_interrupt_gate(uint8_t int_vector, void *handler_address, uint16_t gdt_seg_selector, uint8_t privilege) {
    struct IDTGate *idt_int_gate = &interrupt_descriptor_table.table[int_vector];
    // Set handler offset
    idt_int_gate->offset_low = (uint32_t)handler_address & 0xFFFF;
    idt_int_gate->offset_high = ((uint32_t)handler_address >> 16) & 0xFFFF;
    // Set privilege level
    idt_int_gate->dpl = privilege;
    // Set segment selector
    idt_int_gate->segment = gdt_seg_selector;
    // Target system 32-bit and flag this as valid interrupt gate
    idt_int_gate->_r_bit_1    = INTERRUPT_GATE_R_BIT_1;
    idt_int_gate->_r_bit_2    = INTERRUPT_GATE_R_BIT_2;
    idt_int_gate->_r_bit_3    = INTERRUPT_GATE_R_BIT_3;
    idt_int_gate->gate_32     = 1;
    idt_int_gate->p   = 1;
}
