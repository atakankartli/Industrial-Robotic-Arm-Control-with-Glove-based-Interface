#include <time.h>
#include <stdio.h>
#include <stdint.h>


typedef struct{
    uint8_t             STX;
    char                constStrD[2];
    char                dayOfMonth[2];   
    char                month[2];
    char                yearOfCentury[2];
    char                constStrColon1;
    char                constStrT[2];          
    char                dayOfWeek;
    char                constStrColon2;
    char                constStrU[2];
    char                hours[2];
    char                minutes[2];
    char                seconds[2];
    char                clockSync;
    char                receiverPosition;
    char                timeZone;
    char                clockJumpAnnouncement;
    uint8_t             ETX;
}tual_meinbergStdTime;

tual_meinbergStdTime g_sMeinbergStdTime;

void intToTwoDigitChar(int integer, char* character);

int main(void)
{

    memset(&g_sMeinbergStdTime.STX, ' ', 32);

    g_sMeinbergStdTime.STX = 0x02;
    memcpy(g_sMeinbergStdTime.constStrD, "D:", 2);
    g_sMeinbergStdTime.constStrColon1 = ';';
    g_sMeinbergStdTime.constStrColon2 = ';';
    memcpy(g_sMeinbergStdTime.constStrT, "T:", 2);
    memcpy(g_sMeinbergStdTime.constStrT, "U:", 2);
    g_sMeinbergStdTime.ETX = 0x03;


    struct tm *newtime;
    time_t ltime;

    ltime = time(&ltime);
    newtime = localtime(&ltime);
    printf("The date and time is %s\n", asctime(newtime));
    printf("Sec: %d\n", newtime->tm_sec);
    printf("Min: %d\n", newtime->tm_min);
    printf("Hour: %d\n", newtime->tm_hour);
    printf("Mday: %d\n", newtime->tm_mday);
    printf("Month: %d\n", newtime->tm_mon);
    printf("Year: %d\n", newtime->tm_year + 1900);
    printf("Wday: %d\n", newtime->tm_wday);
    printf("Yday: %d\n", newtime->tm_yday);
    printf("Isdst: %d\n", newtime->tm_isdst);
    printf("Time: %d\n", ltime);


    intToTwoDigitChar(newtime->tm_mday - 48, g_sMeinbergStdTime.dayOfMonth);
    intToTwoDigitChar((newtime->tm_mon + 1) - 48, g_sMeinbergStdTime.month);
    intToTwoDigitChar((newtime->tm_year % 100) - 48, g_sMeinbergStdTime.yearOfCentury);

    printf("%d\n", g_sMeinbergStdTime.month[1]);

    uint8_t meinbergStdTime[32];


}

void intToTwoDigitChar(int integer, char* character)
{
    if(integer >= 10)
    {
        character[0] = integer % 100;
    }
    else{
        character[0] = 0;
    }

    character[1] = integer % 10;
}