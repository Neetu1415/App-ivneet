#include <stdio.h>
#include <stdint.h>

/* ================= BASE TIME ================= */
#define BASE_YEAR  2000
#define MAX_YEAR   2136

/* Date-Time structure */
typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} DateTime;

/* Days in month (non-leap) */
const uint8_t days_in_month[12] =
{
    31,28,31,30,31,30,31,31,30,31,30,31
};

/* Leap year check */
int isLeapYear(uint16_t year)
{
    if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
        return 1;
    return 0;
}

/* ================= VALIDATION ================= */
int isValidDateTime(DateTime dt)
{
    if (dt.year < BASE_YEAR || dt.year > MAX_YEAR)
        return 0;

    if (dt.month < 1 || dt.month > 12)
        return 0;

    uint8_t max_day = days_in_month[dt.month - 1];
    if (dt.month == 2 && isLeapYear(dt.year))
        max_day = 29;

    if (dt.day < 1 || dt.day > max_day)
        return 0;

    if (dt.hour > 23)
        return 0;

    if (dt.minute > 59)
        return 0;

    if (dt.second > 59)
        return 0;

    return 1;
}

/* =========================================
   TIMESTAMP → RTC COUNTER
   ========================================= */
uint32_t Apptime_gettimestamp(DateTime dt)
{
    uint32_t seconds = 0;
    uint16_t y;
    uint8_t m;

    for (y = BASE_YEAR; y < dt.year; y++)
        seconds += (isLeapYear(y) ? 366 : 365) * 86400;

    for (m = 1; m < dt.month; m++)
    {
        if (m == 2 && isLeapYear(dt.year))
            seconds += 29 * 86400;
        else
            seconds += days_in_month[m - 1] * 86400;
    }

    seconds += (dt.day - 1) * 86400;
    seconds += dt.hour * 3600;
    seconds += dt.minute * 60;
    seconds += dt.second;

    return seconds;
}

/* =========================================
   RTC COUNTER → TIMESTAMP
   ========================================= */
void Apptimer_performtimesync(uint32_t rtc, DateTime *dt)
{
    uint32_t seconds = rtc;
    uint16_t year = BASE_YEAR;
    uint8_t month;

    while (1)
    {
        uint32_t year_sec =
            (isLeapYear(year) ? 366 : 365) * 86400;

        if (seconds >= year_sec)
        {
            seconds -= year_sec;
            year++;
        }
        else
            break;
    }

    for (month = 1; month <= 12; month++)
    {
        uint32_t month_sec;

        if (month == 2 && isLeapYear(year))
            month_sec = 29 * 86400;
        else
            month_sec = days_in_month[month - 1] * 86400;

        if (seconds >= month_sec)
            seconds -= month_sec;
        else
            break;
    }

    dt->year = year;
    dt->month = month;
    dt->day = (seconds / 86400) + 1;
    seconds %= 86400;

    dt->hour = seconds / 3600;
    seconds %= 3600;

    dt->minute = seconds / 60;
    dt->second = seconds % 60;
}

/* =========================================
   MAIN
   ========================================= */
int main()
{
    int choice;
    DateTime dt;
    uint32_t rtc_counter;

    while (1) // Infinite loop to keep the menu running
    {
        printf("\n===== RTC TIME SYNC =====\n");
        printf("Base Time: 1 Jan 2000 00:00:00\n");
        printf("1. Timestamp -> RTC Counter\n");
        printf("2. RTC Counter -> Timestamp\n");
        printf("3. Exit (Back to Menu)\n"); // Updated description
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 1)
        {
            printf("\nEnter Timestamp\n");
            printf("Year   : "); scanf("%hu", &dt.year);
            printf("Month  : "); scanf("%hhu", &dt.month);
            printf("Day    : "); scanf("%hhu", &dt.day);
            printf("Hour   : "); scanf("%hhu", &dt.hour);
            printf("Minute : "); scanf("%hhu", &dt.minute);
            printf("Second : "); scanf("%hhu", &dt.second);

            if (!isValidDateTime(dt))
            {
                printf("\n❌ Invalid date-time entered!\n");
                continue; // Go back to menu
            }

            rtc_counter = Apptime_gettimestamp(dt);
            printf("\nRTC Counter Value = %u seconds\n", rtc_counter);
        }
        else if (choice == 2)
        {
            uint64_t temp_counter;
            printf("\nEnter RTC Counter Value: ");
            scanf("%llu", &temp_counter);

            if (temp_counter > 4294967295ULL)
            {
                printf("\n❌ Invalid RTC counter value (out of 32-bit range)\n");
                continue; // Go back to menu
            }

            rtc_counter = (uint32_t)temp_counter;
            Apptimer_performtimesync(rtc_counter, &dt);

            printf("\nRecovered Timestamp:\n");
            printf("%04d-%02d-%02d %02d:%02d:%02d\n",
                   dt.year, dt.month, dt.day,
                   dt.hour, dt.minute, dt.second);
        }
        else if (choice == 3) 
        {
            printf("\nReturning to main menu...\n");
            continue; // Go back to menu
        }
        else
        {
            printf("\n❌ Invalid choice! Please enter 1, 2, or 3.\n");
        }
    }

    return 0;
}
