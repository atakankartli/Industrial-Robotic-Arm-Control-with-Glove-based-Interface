#include <sys/mman.h>
#include <stdint.h>
#include <pigpio.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <cjson/cJSON.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <gps.h>
#include <math.h>
#include "ST7066.h"
#include "configEditor.h"
#include "timeserver.h"
#include "displayPages.h"

TUAL_TIMES_sThread Thread;
TUAL_TIMES_Pages Pages;

uint8_t TUAL_TIMES_ginit = true;
uint8_t TUAL_TIMES_gPrevPage = 1;
uint8_t TUAL_TIMES_gPage2Process = true;

int main()
{
    /////////////////////////////initialize sequence//////////////////////////////////

    // TUAL_ST7066_DisplayClear();
    // TUAL_ST7066_PositionXYString(0, 0, "Booting....");
    TUAL_TIMES_Initialize();
    TUAL_TIMES_ThreadSetup();

    // TUAL_ST7066_Startup();
    // TUAL_ST7066_DisplayOn();
    // TUAL_ST7066_DisplayClear();
    // TUAL_TIMES_Initialize();
    
    TUAL_ST7066_ButtonInit();
    TUAL_TIMES_AssignPages();

    //////////////////////////////////////////////////////////////////////////////////
    while (1)
    {
    }
    return 0;
}

void TUAL_TIMES_ThreadSetup()
{
    char *messageThreadReadPipe = "Thread.ReadPipe";
    char *messageThreadReadDaemonGps = "Thread.ReadDaemonGps";
    char *messageThreadReadDaemonTs2phc = "Thread.ReadDaemonTs2phc";
    char *messageThreadReadDaemonPtp4l = "Thread.ReadDaemonPtp4l";
    char *messageThreadReadEth = "Thread.ReadEth";
    char *messageThreadHandleModes = "Thread.HandleModes";

    pthread_create(&Thread.ReadPipe, NULL, TUAL_TIMES_ThreadReadPipe,
                   (void *)messageThreadReadPipe);
    pthread_create(&Thread.ReadDaemonGps, NULL, TUAL_TIMES_ThreadReadDaemonGps,
                   (void *)messageThreadReadDaemonGps);
    pthread_create(&Thread.ReadDaemonTs2phc, NULL, TUAL_TIMES_ThreadReadDaemonTs2phc,
                   (void *)messageThreadReadDaemonTs2phc);
    pthread_create(&Thread.ReadDaemonPtp4l, NULL, TUAL_TIMES_ThreadReadDaemonPtp4l,
                   (void *)messageThreadReadDaemonPtp4l);
    pthread_create(&Thread.ReadEth, NULL, TUAL_TIMES_ThreadReadEth,
                   (void *)messageThreadReadEth);
    pthread_create(&Thread.HandleModes, NULL, TUAL_TIMES_ThreadHandleModes,
                   (void *)messageThreadHandleModes);
}

void *TUAL_TIMES_ThreadReadPipe()
{
    while (1)
    {
        if (TUAL_TIMES_ginit == false)
        {
            char *temp[300];
            uint8_t whichconfig;

            whichconfig = TUAL_CONF_ReadPipe(TUAL_CONF_READPIPE); // JSON okuyo pipe.txt'den , gNtpConfig ya da gPtpConfig structlarına yazıyo.
            printf("PIPE OKUNDU, GELEN DATA 2 ISE NTP, 3 ISE PTP;\nWHICHCONFIG:%d\n", whichconfig);

            TUAL_ST7066_DisplayClear();
            TUAL_ST7066_PositionXYString(0, 0, "Please wait...");
            // gprevle kontrol edip değişenleri ekranda yansıt ve uygula eğer doğru sayfadaysa
            if (whichconfig == 3)
            {

                TUAL_CONF_StopPtp();
                TUAL_CONF_WritePtpServer();

                if (gPtpConfig.state == true)
                {
                    TUAL_CONF_StartPtp();
                }
                else
                {
                }
            }
            else if (whichconfig == 2)
            {
                TUAL_CONF_StopNtp();

                TUAL_CONF_WriteNtpServer();
                //  statee bak kapat ya da aç

                if (gNtpConfig.state == true)
                {
                    TUAL_CONF_StartNtp();
                }
                else
                {
                }
            }

            TUAL_CONF_CopyStructs();

            TUAL_CONF_SaveConfig();

            // Ufuka ACK atmak için
            if (whichconfig == 2)
            {
                TUAL_CONF_WritePipe(TUAL_CONF_CreateJsonStringPtp(gprevPtpConfig),
                                    TUAL_CONF_WRITEPIPE);
            }
            else if (whichconfig == 3)
            {
                TUAL_CONF_WritePipe(TUAL_CONF_CreateJsonStringNtp(gprevNtpConfig),
                                    TUAL_CONF_WRITEPIPE);
            }
        }
    }
}

void *TUAL_TIMES_ThreadHandleModes()
{
    // her mod değişimde bu thread çağrılır.
    // gps kilitli mi, offset düşük mü, ethernet bağlı mı kontrol et, hepsi varsa ptp aç

    // gps kiltili mi kontrol et bağlıysa ntp aç.
}

void *TUAL_TIMES_ThreadReadDaemonPtp4l()
{
    sleep(5);
    while (1)
    {
        TUAL_CONF_GetPtp4l();
    }
}

void *TUAL_TIMES_ThreadReadDaemonGps()
{
    while (1)
    {
        if (TUAL_TIMES_ginit == false)
        {
            TUAL_CONF_ReadGpsd();
        }
    }
}

void *TUAL_TIMES_ThreadReadEth()
{
    while (1)
    {
        if (TUAL_TIMES_ginit == false)
        {
            TUAL_CONF_CheckEth();
        }
    }
}

void *TUAL_TIMES_ThreadReadDaemonTs2phc()
{
    sleep(5);
    while (1)
    {
        TUAL_CONF_ReadTs2phc();
    }
}

uint8_t TUAL_TIMES_SetSelPin(char mode)
{
    if ((gpioSetMode(TUAL_TIMES_SEL0, PI_OUTPUT) & gpioSetMode(TUAL_TIMES_SEL1, PI_OUTPUT)) == 0)
    {
        switch (mode)
        {
        case 'm':
        {
            TUAL_ST7066_ClrPin(TUAL_TIMES_SEL0);
            TUAL_ST7066_SetPin(TUAL_TIMES_SEL1);
        }
        break;

        case 's':
        {
            TUAL_ST7066_ClrPin(TUAL_TIMES_SEL1);
            TUAL_ST7066_SetPin(TUAL_TIMES_SEL0);
        }
        break;
        }
        return 0;
    }
    else
        return 1;
}

void TUAL_TIMES_Initialize()
{
    gpioInitialise();
    TUAL_ST7066_Startup();
    TUAL_ST7066_DisplayClear();
    TUAL_ST7066_PositionXYString(0, 0, "Booting...");
    TUAL_TIMES_AssignPages();
    TUAL_CONF_WriteTxt(TUAL_TIMES_VERSION, TUAL_TIMES_VERSIONFILE);
    system(TUAL_TIMES_WEBSTART);

    sleep(5);

    // TEXT DOSYASINDAN ÖNCEKİ DEĞERLERİ AL gprev'e at
    TUAL_CONF_ReadConfig(TUAL_CONF_PTPCONFFILE);
    TUAL_CONF_ReadConfig(TUAL_CONF_NTPCONFFILE);
    TUAL_CONF_CopyStructs();

    // TUAL_CONF_WritePipeBlock(TUAL_CONF_CreateJsonStringNtp(),
    //                          TUAL_CONF_WRITEPIPE);
    // TUAL_CONF_WritePipeBlock(TUAL_CONF_CreateJsonStringPtp(),
    //                          TUAL_CONF_WRITEPIPE);

    // Check ptp, add gps check, ethernet check....
    if (gPtpConfig.state)
    {
        TUAL_CONF_StartPtp();
    }
    // Check ptp, add gps check, ethernet check....
    if (gNtpConfig.state)
    {
        TUAL_CONF_StartNtp();
    }

    TUAL_TIMES_ginit = false;
}

void TUAL_ST7066_ButtonInit()
{
    gpioInitialise();
    // set buttons as input
    gpioInitialise();
    gpioSetMode(TUAL_TIMES_BUTTONR, PI_INPUT);
    gpioSetMode(TUAL_TIMES_BUTTONL, PI_INPUT);
    gpioSetMode(TUAL_TIMES_BUTTONU, PI_INPUT);
    gpioSetMode(TUAL_TIMES_BUTTOND, PI_INPUT);
    gpioSetMode(TUAL_TIMES_BUTTONOK, PI_INPUT);
    gpioSetMode(TUAL_TIMES_BUTTONESC, PI_INPUT);
    gpioSetMode(TUAL_TIMES_BUTTONF1, PI_INPUT);
    gpioSetMode(TUAL_TIMES_BUTTONF2, PI_INPUT);

    // set buttons' pulls off
    gpioSetPullUpDown(TUAL_TIMES_BUTTONR, PI_PUD_UP);
    gpioSetPullUpDown(TUAL_TIMES_BUTTONL, PI_PUD_UP);
    gpioSetPullUpDown(TUAL_TIMES_BUTTONU, PI_PUD_UP);
    gpioSetPullUpDown(TUAL_TIMES_BUTTOND, PI_PUD_UP);
    gpioSetPullUpDown(TUAL_TIMES_BUTTONOK, PI_PUD_UP);
    gpioSetPullUpDown(TUAL_TIMES_BUTTONESC, PI_PUD_UP);
    gpioSetPullUpDown(TUAL_TIMES_BUTTONF1, PI_PUD_UP);
    gpioSetPullUpDown(TUAL_TIMES_BUTTONF2, PI_PUD_UP);
    // read the buttons using gpioSetAlertFunc and adjust gpageNum
    // if button is pressed, change page
    gpioSetAlertFunc(TUAL_TIMES_BUTTONR, TUAL_TIMES_ButtonFunction);
    gpioSetAlertFunc(TUAL_TIMES_BUTTONL, TUAL_TIMES_ButtonFunction);
    gpioSetAlertFunc(TUAL_TIMES_BUTTONU, TUAL_TIMES_ButtonFunction);
    gpioSetAlertFunc(TUAL_TIMES_BUTTOND, TUAL_TIMES_ButtonFunction);
    gpioSetAlertFunc(TUAL_TIMES_BUTTONOK, TUAL_TIMES_ButtonFunction);
    gpioSetAlertFunc(TUAL_TIMES_BUTTONESC, TUAL_TIMES_ButtonFunction);
    gpioSetAlertFunc(TUAL_TIMES_BUTTONF1, TUAL_TIMES_ButtonFunction);
    gpioSetAlertFunc(TUAL_TIMES_BUTTONF2, TUAL_TIMES_ButtonFunction);

    gpioGlitchFilter(TUAL_TIMES_BUTTONR, 5000);
    gpioGlitchFilter(TUAL_TIMES_BUTTONL, 5000);
    gpioGlitchFilter(TUAL_TIMES_BUTTONU, 5000);
    gpioGlitchFilter(TUAL_TIMES_BUTTOND, 5000);
    gpioGlitchFilter(TUAL_TIMES_BUTTONOK, 5000);
    gpioGlitchFilter(TUAL_TIMES_BUTTONESC, 5000);
    gpioGlitchFilter(TUAL_TIMES_BUTTONF1, 5000);
    gpioGlitchFilter(TUAL_TIMES_BUTTONF2, 5000);
}

void TUAL_TIMES_ButtonFunction(int gpio, int level, uint32_t tick)
{
    // sınırlar 00-20, 0-2 mod alınca parenta git, 10la çarpınca childa git

    if (gpio == TUAL_TIMES_BUTTONR)
    {
        Pages.CurrentPage->cursorPos++;
        Pages.CurrentPage->cursorPos = Pages.CurrentPage->cursorPos % (Pages.CurrentPage->rowNum + 1);
        TUAL_TIMES_PrintCursor();
        printf("\nBUTTON:\n->\n");
    }
    else if (gpio == TUAL_TIMES_BUTTONL)
    {
        if (Pages.CurrentPage->cursorPos == 0)
        {
            Pages.CurrentPage->cursorPos = Pages.CurrentPage->rowNum;
        }
        else
        {
            Pages.CurrentPage->cursorPos--;
            Pages.CurrentPage->cursorPos = Pages.CurrentPage->cursorPos % (Pages.CurrentPage->rowNum + 1);
        }
        TUAL_TIMES_PrintCursor();
        printf("\nBUTTON:\n<-\n");
    }
    else if (gpio == TUAL_TIMES_BUTTONU)
    {
        Pages.CurrentPage->cursorPos = Pages.CurrentPage->cursorPos + 2;
        Pages.CurrentPage->cursorPos = Pages.CurrentPage->cursorPos % (Pages.CurrentPage->rowNum + 1);
        TUAL_TIMES_PrintCursor();
        printf("\nBUTTON:\n^\n");
    }
    else if (gpio == TUAL_TIMES_BUTTOND)
    {
        Pages.CurrentPage->cursorPos = Pages.CurrentPage->cursorPos + 2;
        Pages.CurrentPage->cursorPos = Pages.CurrentPage->cursorPos % (Pages.CurrentPage->rowNum + 1);
        TUAL_TIMES_PrintCursor();
        printf("\nBUTTON:\nv\n");
    }
    else if (gpio == TUAL_TIMES_BUTTONOK)
    {
        if (Pages.CurrentPage->cursorPos == 0)
        {
            if (Pages.CurrentPage->child0 != NULL)
            {
                Pages.CurrentPage = Pages.CurrentPage->child0;
                TUAL_TIMES_PrintPage();
            }
        }
        else if (Pages.CurrentPage->cursorPos == 1)
        {
            if (Pages.CurrentPage->child1 != NULL)
            {
                Pages.CurrentPage = Pages.CurrentPage->child1;
                TUAL_TIMES_PrintPage();
            }
        }
        else if (Pages.CurrentPage->cursorPos == 2)
        {
            if (Pages.CurrentPage->child2 != NULL)
            {
                Pages.CurrentPage = Pages.CurrentPage->child2;
                TUAL_TIMES_PrintPage();
            }
        }
        else if (Pages.CurrentPage->cursorPos == 3)
        {
            if (Pages.CurrentPage->child3 != NULL)
            {
                Pages.CurrentPage = Pages.CurrentPage->child3;
                TUAL_TIMES_PrintPage();
            }
        }
        printf("\nBUTTON:\nOK\n");
    }
    else if (gpio == TUAL_TIMES_BUTTONESC)
    {

        if (Pages.CurrentPage->parent != NULL)
        {
            Pages.CurrentPage = Pages.CurrentPage->parent;
            TUAL_TIMES_PrintPage();
        }
        printf("\nBUTTON:\nESC\n");
    }
    else if (gpio == TUAL_TIMES_BUTTONF1)
    {
        // increase gpageNum
        printf("\nBUTTON:\nF1\n");
    }
    else if (gpio == TUAL_TIMES_BUTTONF2)
    {
        // decrease gpageNum
        printf("\nBUTTON:\nF2\n");
    }
}
void TUAL_TIMES_PrintPage()
{
    TUAL_ST7066_DisplayClear();
    TUAL_ST7066_PositionXYString(0, 0, Pages.CurrentPage->row0);
    TUAL_ST7066_PositionXYString(20, 0, Pages.CurrentPage->row1);
    TUAL_ST7066_PositionXYString(0, 1, Pages.CurrentPage->row2);
    TUAL_ST7066_PositionXYString(20, 1, Pages.CurrentPage->row3);
    printf("\nPage num:%d", Pages.CurrentPage->pageNum);
}
void TUAL_TIMES_PrintCursor()
{
    TUAL_ST7066_PositionXYString(0, 0, " ");
    TUAL_ST7066_PositionXYString(20, 0, " ");
    TUAL_ST7066_PositionXYString(0, 1, " ");
    TUAL_ST7066_PositionXYString(20, 1, " ");
    switch (Pages.CurrentPage->cursorPos)
    {
    case 0:
        TUAL_ST7066_PositionXYString(0, 0, ">");
        printf("row0\n");
        break;
    case 1:
        TUAL_ST7066_PositionXYString(20, 0, ">");
        printf("row1\n");
        break;
    case 2:
        TUAL_ST7066_PositionXYString(0, 1, ">");
        printf("row2\n");
        break;
    case 3:
        TUAL_ST7066_PositionXYString(20, 1, ">");
        printf("row3\n");
        break;
    }
}

void TUAL_TIMES_AssignPages()
{

    Pages.page0 = (TUAL_TIMES_sPage){0, 1, false, 0, // pageNum, rowNum, changable, cursorPos
                                     "Ntp Info",
                                     "Ptp Info", //
                                     NULL,
                                     NULL, //
                                     &Pages.page1,
                                     &Pages.page2,
                                     NULL,
                                     NULL,
                                     NULL};          //
    Pages.page1 = (TUAL_TIMES_sPage){1, 3, false, 0, // INFO PAGE
                                     "Ntp State",
                                     "Slave or Master:", //
                                     "GPS Fix:",
                                     "Offset", //
                                     NULL,
                                     NULL,
                                     NULL,
                                     NULL,
                                     &Pages.page0};  //
    Pages.page2 = (TUAL_TIMES_sPage){1, 3, false, 0, // INFO PAGE
                                     "Ptp State",
                                     "Slave or Master:", //
                                     "GPS Fix:",
                                     "Offset", //
                                     NULL,
                                     NULL,
                                     NULL,
                                     NULL,
                                     &Pages.page0}; //
}

// Kill ptp4l
// Kill Ntp