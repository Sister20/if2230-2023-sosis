#include "../lib-header/string.h"
/**
 * count length of string
*/
int strlen(char str[]){
    int count = 0;
    while(str[count]!='\0'){
        count++;
    }
    return count;
}

/**
 * Compare string
 * 
 * @return 1: str1>str2, 0: str1==str2, -1: str1<str2
*/
int strcmp(char str1[], char str2[]){
    int i = 0;
    while (str1[i]!='\0'&& str2[i]!='\0'){
        if(str1[i]<str2[i]){
            return -1;
        }
        if(str1[i]>str2[i]){
            return 1;
        }
        i++;
    }
    if(str1[i]<str2[i]){
        return -1;
    }
    if(str1[i]>str2[i]){
        return 1;
    }
    return 0;
}

/**
 * copy string
*/
void strcpy(char *str_dest[], char str_src[]){
    int i = 0;
    while(str_src[i]!='\0'){
        *str_dest[i] = str_src[i];
        i++;
    }
    *str_dest[i] = '\0';
    return;
}