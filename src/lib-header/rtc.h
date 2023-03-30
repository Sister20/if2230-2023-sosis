#include "portio.h"
#define CURRENT_YEAR        2023                            // Change this each year!
#define CMOS_ADDRESS_PORT 0x70
#define CMOS_DATA_PORT    0x71

unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned char day;
unsigned char month;
unsigned int year;

int get_update_in_progress_flag();

unsigned char get_RTC_register(int reg);

void read_rtc();