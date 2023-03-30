#include "lib-header/stdtype.h"
#include "lib-header/portio.h"


#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x02
#define RTC_HOURS 0x04
#define RTC_DAY_OF_MONTH 0x07
#define RTC_MONTH 0x08
#define RTC_YEAR 0x09

#define BCD_TO_BIN(val) ((val) = ((val) & 15) + ((val) >> 4) * 10)

void get_date_time(int* year, int* month, int* day, int* hour, int* minute, int* second);

/**
 * Function to get the current date as a uint16_t value
 * Returns the date as a uint16_t value in the format YYYYMMDD
 * If an error occurs, sets the value pointed to by date to 0
*/
void get_date(uint16_t* date);

/**
 * Function to get the current time as a uint16_t value
 * Returns the time as a uint16_t value in the format HHMMSS
 * If an error occurs, sets the value pointed to by time to 0
*/
void get_time(uint16_t* time);