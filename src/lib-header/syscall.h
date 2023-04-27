#ifndef SYSCALL_H
#define SYSCALL_H
#include "stdtype.h"

// void syscall(struct CPURegister cpu, __attribute__((unused)) struct InterruptStack info);
void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);
#endif