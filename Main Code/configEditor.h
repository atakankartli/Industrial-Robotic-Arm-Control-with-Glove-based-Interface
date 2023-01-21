#ifndef _configEditor_H
#define _configEditor_H
#include <stdint.h>
#include <stdbool.h>
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include "displayPages.h"

#define TUAL_CONF_NTP_BASE "/etc/chrony/conf.d"
#define TUAL_CONF_NTP_EXT ".conf"
#define TUAL_CONF_NTP_BROADCAST_FILE TUAL_CONF_NTP_BASE "/broadcast" TUAL_CONF_NTP_EXT
#define TUAL_CONF_NTP_SERVER0_FILE TUAL_CONF_NTP_BASE "/server0" TUAL_CONF_NTP_EXT
#define TUAL_CONF_NTP_SERVER1_FILE TUAL_CONF_NTP_BASE "/server1" TUAL_CONF_NTP_EXT
#define TUAL_CONF_NTP_SERVER2_FILE TUAL_CONF_NTP_BASE "/server2" TUAL_CONF_NTP_EXT
#define TUAL_CONF_NTP_SERVER3_FILE TUAL_CONF_NTP_BASE "/server3" TUAL_CONF_NTP_EXT
#define TUAL_CONF_NTP_LOCAL_STRATUM_FILE TUAL_CONF_NTP_BASE "/local_stratum" TUAL_CONF_NTP_EXT
#define TUAL_CONF_ALLOW_FILE "allow.txt"
#define TUAL_CONF_READPIPE "web2c.fifo"
#define TUAL_CONF_WRITEPIPE "c2web.fifo"
#define TUAL_CONF_CONFPIPEPTP "LASTCONFIGPTP"
#define TUAL_CONF_CONFPIPENTP "LASTCONFIGNTP"

#define TUAL_CONF_BASH "#/bin/bash \n"

#define TUAL_CONF_PTP4LKILL "sudo systemctl stop ts2phc"
#define TUAL_CONF_PTP4LMASTERSTART "sudo systemctl start ptp4l-gm"
#define TUAL_CONF_PTP4LSLAVESTART "sudo systemctl start ptp4l-s"

#define TUAL_CONF_TS2PHCSTART "sudo systemctl start ts2phc@eth0"
#define TUAL_CONF_TS2PHCKILL "sudo systemctl stop ts2phc"

#define TUAL_CONF_CHRONYSTART "sudo systemctl start chrony"
#define TUAL_CONF_CHRONYKILL "sudo systemctl stop chrony"

#define TUAL_CONF_PHC2SHMSTART "sudo systemctl start phc2shm"
#define TUAL_CONF_PHC2SHMKILL "sudo systemctl stop phc2shm"

#define TUAL_CONF_SETPINFUNCIN "sudo ./setPinFunction -d /dev/ptp0 -L 0,1"
#define TUAL_CONF_SETPINFUNCOUT "sudo ./setPinFunction -d /dev/ptp0 -L 0,2"
#define TUAL_CONF_SETPINFUNCTIMESTAMP "sudo ./setPinFunction -d /dev/ptp0 -p 1000000000"

#define TUAL_CONF_TS2PHCLOG "/var/log/ts2phc.log"
#define TUAL_CONF_PTP4LLOG "/var/log/ptp4l.log"

#define TUAL_CONF_PTPCONFFILE "/home/timeserver/ptpconfigsave.txt"
#define TUAL_CONF_NTPCONFFILE "/home/timeserver/ntpconfigsave.txt"
#define TUAL_CONF_ETHFILE "/home/timeserver/eth.fifo"
#define TUAL_CONF_ETHCRFILE "/sys/class/net/eth0/carrier"

#define TUAL_CONF_LINE_PRIORITY1 8
#define TUAL_CONF_LINE_PRIORITY2 9
#define TUAL_CONF_LINE_DOMAINNUMBER 10
#define TUAL_CONF_LINE_ANNINTERVAL 25
#define TUAL_CONF_LINE_SYNCINTERVAL 26
#define TUAL_CONF_LINE_MINDELREQINTERVAL 28
#define TUAL_CONF_LINE_ANNRECTIMEOUT 26
#define TUAL_CONF_LINE_NETWORK_TRANSPORT 98
#define TUAL_CONF_LINE_DELAY_MECHANISM 98

extern uint8_t TUAL_TIMES_ginit;
extern TUAL_TIMES_Pages Pages;

typedef struct
{
    bool ntpOrPtp;             // 1 for ntp 0 for ptp
    bool state;                // 1 For ON 0 for OFF
    bool broadcastOrServer;    // 1 for broadcast 0 for server
    uint8_t serverNum;         // ServerNum for server config file server0.txt, server1.txt...
    uint8_t broadcastInterval; // broadcast interval in seconds
    char serverAddress[20];    // xxx.xxx.xxx.xxx
    bool key;                  // 1 for Symmetric Key enabled
    uint8_t keyNum;            // Number of the Symmetric Key
    bool iburst;               // 1 for iBurst enabled
    bool localStratum;         // local stratum on or off
    uint8_t localStratumNum;   // local stratum number

} TUAL_CONF_sNtp;

typedef struct
{
    bool ntpOrPtp;                  // 1 for ntp, 0 for ptp
    bool state;                     // 1 For ON 0 for OFF
    bool masterOrSlave;             // 1 for master, 0 for slave
    bool delay_mechanismE2eOrP2p;   // 1 for E2E 0 for P2P
    char domainNumber[5];           // Domain number 0-255
    char priority1[5];              // 0-255 default is 128
    char priority2[5];              // 0-255 default is 128
    char logAnnounceInterval[5];    // Default is 1(2Seconds)
    char logSyncInterval[5];        // Default is 0(1 Second)
    char logMinDelayReqInterval[5]; // Default is 0(1 Second)
    char announceReceiptTimeout[5]; // Default is 3

} TUAL_CONF_sPtp;

uint8_t TUAL_CONF_WriteNtpServer();
char *TUAL_CONF_CreateJsonStringPtp();
char *TUAL_CONF_CreateJsonStringNtp();
uint8_t TUAL_CONF_ParseString(const char *const configString);
uint8_t TUAL_CONF_ReadPipe(char *pipeFile);
uint8_t TUAL_CONF_WritePipe(char *string, char *pipeFile);
void TUAL_CONF_ThreadSetup();
uint8_t TUAL_CONF_Write2Row(char *row2, char *variable4, char *value6);
uint8_t TUAL_CONF_WritePtpServer();
uint8_t TUAL_CONF_ReadGpsd();
uint8_t TUAL_CONF_ReadTs2phc();
void TUAL_CONF_CopyStructs();
uint8_t TUAL_CONF_StopPtp();
uint8_t TUAL_CONF_StartPtp();
uint8_t TUAL_CONF_StopNtp();
uint8_t TUAL_CONF_StartNtp();
uint8_t TUAL_CONF_SaveConfig();
uint8_t TUAL_CONF_ReadConfig(char *File);
uint8_t TUAL_CONF_WriteTxt(char *string, char *File);
uint8_t TUAL_CONF_CheckEth();
uint8_t TUAL_CONF_WritePipeBlock(char *string, char *pipeFile);
static char *TUAL_CONF_ReadFile(const char *filename);
uint8_t TUAL_CONF_GetPtp4l();
#endif