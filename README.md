# SOSIS-OS
Tugas Besar IF2230 - Sistem Operasi 2023

## **Table of Contents**
* [General Information](#general-information)
* [Screenshot](#screenshots)
* [Technologies Used](#technologies-used)
* [Setup](#how-to-run-and-compile-windows)
* [Compile and Run](#how-to-run-and-compile-windows)
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

### Milestone 3
*TBA*


## **Screenshots**
*TBA*

## **Technologies Used**

1. [Window Subsytem for Linux](https://docs.microsoft.com/en-us/windows/wsl/install)
2. [Ubuntu 20.04 LTS](https://releases.ubuntu.com/20.04/)
3. [Netwide assembler](https://www.nasm.us/) 
4. [GNU C Compiler](https://man7.org/linux/man-pages/man1/gcc.1.html) 
5. [GNU Linker](https://linux.die.net/man/1/ld)
6. [QEMU - System i386](https://www.qemu.org/docs/master/system/target-i386.html) 
7. [GNU Make](https://www.gnu.org/software/make/) 
8. [genisoimage](https://linux.die.net/man/1/genisoimage) 
9. [XServer](https://sourceforge.net/projects/vcxsrv/)

## **Program Features**
1. Kernel
2. Keyboard
3. Filesystem

## Setup

1. For Windows users, install WSL
2. Using WSL or Linux, install all the requirements in the technologies section

```
sudo apt update
sudo apt install gcc nasm make qemu-system-x86 genisoimage
```
3. For WSL, we recommend installing XServer and creating a shortcut to the XServer binaries with launch configs such as `"C:\Program Files\VcXsrv\vcxsrv.exe" :0 -ac -terminate -lesspointer -multiwindow -clipboard -wgl -dpi auto`

4. Clone the github repository by using `git clone https://github.com/Sister20/if2230-2023-sosis`

## **Compile and Run**

1. Open WSL or Linux terminal
2. If using XServer, open XServer using the created shortcut, and run ``echo ‘export DISPLAY=`grep -oP "(?<=nameserver ).+" /etc/resolv.conf`:0.0’ >> ~/.bashrc`` in the terminal
3. Navigate to the root of the cloned repository in terminal, run the following to create a blank OS disk
```
make disk
```
4. Run these two commands to build and start the OS
```
make all
```
```
make run
```

5. If the OS is unable to start, try opening this repository in Visual Studio Code and start `Run and Debug` or press `F5`.

## **References**

1. [Milestone1 Docs](https://docs.google.com/document/d/1ebhX-D_bNafray9C6T8cmgAy8_E58i_uWkngrNWHjr4/edit#)
2. [Milestone2 Docs](https://docs.google.com/document/d/10RjQ4Z6DKzXhTVmj0kUDDEYZIdNClO25J7pIAzHE188/edit#)
3. [Milestone3 Docs](https://docs.google.com/document/d/1h50kOZ3R97aLGhHBzNCPpaSdgi05Kaow7A4ulFuyxno/edit#)
4. [Debugger & WSL](https://docs.google.com/document/d/1Zt3yzP_OEiFz8g2lHlpBNNr9qUyXghFNeQlAeQpAaII/edit#)

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
| Milestone 1 |  :heavy_check_mark:  |
| Milestone 2 |  :heavy_check_mark:  |
| Milestone 2 |    |
