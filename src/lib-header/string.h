#ifndef STRING_H
#define STRING_H
#include "stdtype.h"

/**
 * count length of string
*/
int strlen(char* str);

/**
 * Compare string
 * 
 * @return 1: str1>str2, 0: str1==str2, -1: str1<str2
*/
int strcmp(char *str1, char *str2);

/**
 * copy string
*/
void strcpy(char *str_dest, char *str_src);

/**
 * copy string with size of n
 * @param str_dest dest string
 * @param str_src src string
 * @param n string length
*/
void strncpy(char *str_dest, char *str_src, tssize_t n);

/**
 * concat src & dest string and store it to dest string
 * @param dest dest string
 * @param src src string
*/
void strcat(char *dest, const char *src);

/**
 * Split buf into 2 parts by offset
 * @param buf string
 * @param first_section first part of split
 * @param second_section second part of split
 * @param offset split index
*/
void split(char* buf, char* first_section, char* second_section, int offset);

#endif