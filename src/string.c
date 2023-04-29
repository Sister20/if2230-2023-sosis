#include "lib-header/string.h"
/**
 * count length of string
*/
int strlen(char* str){
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
int strcmp(char* str1, char* str2){
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
void strcpy(char *str_dest, char *str_src){
    int i = 0;
    while(str_src[i]!='\0'){
        str_dest[i] = str_src[i];
        i++;
    }
    str_dest[i] = '\0';
    return;
}


void strncpy(char *str_dest, char *str_src, tssize_t n) {
    tssize_t i;
    for (i = 0; i < n && str_src[i] != '\0'; i++) {
        str_dest[i] = str_src[i];
    }
    for (; i < n; i++) {
        str_dest[i] = '\0';
    }
}

void strcat(char *dest, const char *src) {
    tssize_t dest_len = strlen(dest);
    tssize_t i;
    for (i = 0; src[i] != '\0'; i++) {
        dest[dest_len + i] = src[i];
    }
    dest[dest_len + i] = '\0';
}

void split(char* buf, char* first_section, char* second_section, int offset) {
    int buf_len = strlen(buf);
    for (int i = 0; i < buf_len; i++) {
        first_section[i] = buf[i];
    }
    first_section[offset - 1] = '\0';

    for (int i = 0; i < buf_len - offset; i++)
    {
        second_section[i] = buf[i + offset];
    }
    second_section[buf_len - offset] = '\0';
}

void parse_int(uint32_t num, char *str) {
    int i = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    while (num != 0) {
        uint32_t rem = num % 10;
        str[i++] = rem + '0';
        num = num / 10;
    }

    str[i] = '\0';

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = *(str + start);
        *(str + start) = *(str + end);
        *(str + end) = temp;
        start++;
        end--;
    }
}
