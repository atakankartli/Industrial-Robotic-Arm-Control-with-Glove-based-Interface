#ifndef _timeserver_H
#define _timeserver_H

#include "ST7066.h"
#include "configEditor.h"
#include <pthread.h>
#define TUAL_TIMES_VERSION "1.0"
#define TUAL_TIMES_VERSIONFILE "version.txt"
#define TUAL_TIMES_WEBSTART "sudo node /home/timeserver/index.js &"
// Select pins for switch
#define TUAL_TIMES_SEL0 19
#define TUAL_TIMES_SEL1 26

#define TUAL_TIMES_BUTTONR 21
#define TUAL_TIMES_BUTTONL 20
#define TUAL_TIMES_BUTTONU 12
#define TUAL_TIMES_BUTTOND 16
#define TUAL_TIMES_BUTTONOK 6
#define TUAL_TIMES_BUTTONESC 13
#define TUAL_TIMES_BUTTONF1 0
#define TUAL_TIMES_BUTTONF2 5

typedef struct
{
    pthread_t Display;
    pthread_t ReadPipe;
    pthread_t Command;
    pthread_t HandleModes;
    pthread_t ReadDaemonPtp4l;
    pthread_t ReadDaemonGps;
    pthread_t ReadDaemonTs2phc;
    pthread_t ReadEth;

} TUAL_TIMES_sThread;

extern TUAL_CONF_sNtp gNtpConfig;
extern TUAL_CONF_sPtp gPtpConfig;
extern TUAL_CONF_sPtp gprevPtpConfig;
extern TUAL_CONF_sNtp gprevNtpConfig;
extern bool TUAL_CONF_gptp4lSync;
extern bool TUAL_CONF_gpleasewait;
extern uint8_t TUAL_CONF_gGpsFixNtp;
extern bool TUAL_CONF_gGpsFixPtp;
extern char TUAL_CONF_gts2phcOffset[20];
extern char TUAL_CONF_gptp4lOffset[20];
extern bool TUAL_CONF_gts2phcZeroEvent;
extern uint8_t TUAL_CONF_gts2phcOffsetLen;
extern bool TUAL_CONF_gEth;

// Threads
void *TUAL_TIMES_ThreadReadPipe();
void *TUAL_TIMES_ThreadDisplay();
void *TUAL_TIMES_ThreadReadDaemonGps();
void *TUAL_TIMES_ThreadReadDaemonTs2phc();
void *TUAL_TIMES_ThreadReadDaemonPtp4l();
void *TUAL_TIMES_ThreadReadEth();
void *TUAL_TIMES_ThreadHandleModes();

// Functions
void TUAL_TIMES_Initialize();
void TUAL_TIMES_AssignPages();
void TUAL_TIMES_ThreadSetup();
uint8_t TUAL_TIMES_SetSelPin(char mode);
void TUAL_ST7066_ButtonInit();
void TUAL_TIMES_ButtonFunction(int gpio, int level, uint32_t tick);
void TUAL_TIMES_PrintPage();
void TUAL_TIMES_PrintCursor();

#endif