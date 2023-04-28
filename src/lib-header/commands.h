#ifndef COMMANDS_H
#define COMMANDS_H
#include "stdtype.h"
#include "fs-syscall.h"

void cd(uint32_t clusterNumber, char* path);

void ls(uint32_t clusterNumber);

void mkdir(uint32_t clusterNumber, char* dirName);

void cat(uint32_t clusterNumber, char* fileName, char* fileExt);

#endif