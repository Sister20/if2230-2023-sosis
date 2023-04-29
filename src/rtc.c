#include "lib-header/rtc.h"

void get_date_time(int* year, int* month, int* day, int* hour, int* minute, int* second) {
    uint8_t century, year_byte, month_byte, day_byte, hour_byte, minute_byte, second_byte;

    out(CMOS_ADDRESS, RTC_SECONDS);
    second_byte = in(CMOS_DATA);
    out(CMOS_ADDRESS, RTC_MINUTES);
    minute_byte = in(CMOS_DATA);
    out(CMOS_ADDRESS, RTC_HOURS);
    hour_byte = in(CMOS_DATA);
    out(CMOS_ADDRESS, RTC_DAY_OF_MONTH);
    day_byte = in(CMOS_DATA);
    out(CMOS_ADDRESS, RTC_MONTH);
    month_byte = in(CMOS_DATA);
    out(CMOS_ADDRESS, RTC_YEAR);
    year_byte = in(CMOS_DATA);
    out(CMOS_ADDRESS, 0x32); // century byte
    century = in(CMOS_DATA);

    BCD_TO_BIN(second_byte);
    BCD_TO_BIN(minute_byte);
    BCD_TO_BIN(hour_byte);
    BCD_TO_BIN(day_byte);
    BCD_TO_BIN(month_byte);
    BCD_TO_BIN(year_byte);
    BCD_TO_BIN(century);

    *year = century * 100 + year_byte;
    *month = month_byte;
    *day = day_byte;
    *hour = hour_byte;
    *minute = minute_byte;
    *second = second_byte;
}

void get_time(uint16_t* time) {
    int hour, minute, second;
    int dummy = 0;  // dummy variable to pass to get_date_time
    get_date_time(&dummy, &dummy, &dummy, &hour, &minute, &second);
    *time = ((uint16_t) hour) * 256 + (uint16_t) minute;
}

void get_date(uint16_t* date) {
    int year, month, day;
    int dummy = 0;  // dummy variable to pass to get_date_time
    get_date_time(&year, &month, &day, &dummy, &dummy, &dummy);
    *date = ((uint16_t) (year % 100)) * 512 + ((uint16_t) month) * 32 + (uint16_t) day;
}
