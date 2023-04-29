#ifndef SYSCALL_H
#define SYSCALL_H
#include "stdtype.h"

#define FS_READ 0
#define FS_READ_DIR 1
#define FS_WRITE 2
#define FS_DELETE 3
#define KEYBOARD_INPUT 4
#define TEXT_OUTPUT 5

// void syscall(struct CPURegister cpu, __attribute__((unused)) struct InterruptStack info);
void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);
#endif