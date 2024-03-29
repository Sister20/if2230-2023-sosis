#ifndef COMMANDS_H
#define COMMANDS_H
#include "stdtype.h"
#include "fs-syscall.h"
#include "string.h"
#include "stdmem.h"

struct CWDdata {
    uint32_t currentCluster;
    char cwdName[8];
    uint32_t prevCluster;
};

void print(char *buf, uint8_t color);

void log(char *buf);

void cd(struct CWDdata* cwd_data, char* folderName);

void ls(struct CWDdata cwd_data);

void mkdir(uint32_t clusterNumber, char* dirName);

void cat(uint32_t clusterNumber, char* fileName);

int cp(struct CWDdata cwd_data, char* sourceFileName, char* destinationFileName);

int rm(struct CWDdata cwd_data, char* filePath);

void whereis(struct CWDdata cwd_data, char *filename);

void search(struct CWDdata cwd_data, char name[]);

void mv(struct CWDdata cwd_data, char *src, char *dest);

#endif