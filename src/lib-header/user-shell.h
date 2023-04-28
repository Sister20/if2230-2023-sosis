#ifndef USER_SHELL_H
#define USER_SHELL_H

#include "stdtype.h"
#include "string.h"
#include "syscall.h"
#include "commands.h"

/**
 * Get character from keyboard input 
*/
void fgets(char* buf, tssize_t buf_size);

#endif