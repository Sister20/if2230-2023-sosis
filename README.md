# SOSIS-OS
Tugas Besar IF2230 - Sistem Operasi 2023

## **Table of Contents**
* [General Information](#general-information)
* [Technologies Used](#technologies-used)
* [How to Run and Compile](#how-to-run-and-compile-windows)
* [Screenshot](#screenshots)
* [Progress Report](#progress-report)
* [Author](#authors)

## **General Information**
### Milestone 1
Tugas ini akan membuat sebuah Sistem operasi yang akan berjalan pada arsitektur x86 32 bit yang nanti akan dijalankan dengan emulator QEMU.
* Menyiapkan alat & repository
* Pembuatan build script
* Menjalankan sistem operasi
* Membuat output dengan text
* Memasuki Protected Mode

### Milestone 2
Milestone ini akan berfokus kepada interrupt, simple hardware device driver untuk keyboard & disk, dan file system “FAT32 - IF2230 edition”  
* Interrupt & IDT
* Keyboard Driver
* Disk Driver
* File System
* CMOS Filesystem



## **Screenshots**


## **Technologies Used**

1. [Window Subsytem for Linux](https://docs.microsoft.com/en-us/windows/wsl/install)
2. [Ubuntu 20.04 LTS](https://releases.ubuntu.com/20.04/)
3. [Netwide assembler](https://www.nasm.us/) 
4. [GNU C Compiler](https://man7.org/linux/man-pages/man1/gcc.1.html) 
5. [GNU Linker](https://linux.die.net/man/1/ld)
6. [QEMU - System i386](https://www.qemu.org/docs/master/system/target-i386.html) 
7. [GNU Make](https://www.gnu.org/software/make/) 
8. [genisoimage](https://linux.die.net/man/1/genisoimage) 

## **Program Features**
1. kernel
2. keyboard
3. Filesystem

## Setup

1. Install all the requirements in the technologies section

```
sudo apt update
sudo apt install gcc nasm make qemu-system-x86 genisoimage
```

2. Run WSL
3. clone the github repository by using `git clone https://github.com/Sister20/if2230-2023-sosis`
4. run `make bla blabla` 

## **References**

1. [Milestone1 Doc](https://docs.google.com/document/d/1ebhX-D_bNafray9C6T8cmgAy8_E58i_uWkngrNWHjr4/edit#)
2. [Milestone2 Doc](https://docs.google.com/document/d/10RjQ4Z6DKzXhTVmj0kUDDEYZIdNClO25J7pIAzHE188/edit#)


## **Authors**
| Name | NIM |
| ---- | --- |
| Wilson Tansil | 13521054 |
| Eugene Yap Jin Quan | 13521074 |
| Alexander Jason | 13521100 |
| Jimly Firdaus | 13521102 |

## **Progress Report**
| Checkpoint | Status |
| ---------- | :------: |
| Milestone 1 | :heavy_check_mark:|
| Milestone 2 |  :heavy_check_mark:  |
