#ifndef USER_SHELL_H
#define USER_SHELL_H

#include "stdtype.h"
#include "commands.h"

/**
 * Get character from keyboard input 
*/
void fgets(char* buf, tssize_t buf_size);

/**
 * Parse user command, call the apropriate function if parsed result is known, otherwise puts error
 * @param buf user entered string
*/
void commandParser(char* buf);

#endif