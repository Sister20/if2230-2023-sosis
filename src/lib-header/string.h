#ifndef STRING_H
#define STRING_H
#include "stdtype.h"

/**
 * count length of string
*/
int strlen(char str[]);

/**
 * Compare string
 * 
 * @return 1: str1>str2, 0: str1==str2, -1: str1<str2
*/
int strcmp(char str1[], char str2[]);

/**
 * copy string
*/
void strcpy(char *str_dest[], char str_src[]);

/**
 * Parse user command, call the apropriate function if parsed result is known, otherwise puts error
 * @param buf user entered string
*/
void commandParser(char* buf);

#endif