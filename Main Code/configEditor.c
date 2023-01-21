#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <gps.h>
#include <math.h>

#include "ST7066.h"
#include "configEditor.h"
#include "timeserver.h"
#include "displayPages.h"

TUAL_CONF_sNtp gNtpConfig;
TUAL_CONF_sNtp gprevNtpConfig;
TUAL_CONF_sPtp gPtpConfig;
TUAL_CONF_sPtp gprevPtpConfig;

bool TUAL_CONF_gptp4lSync = false;
bool TUAL_CONF_gpleasewait = false;
uint8_t TUAL_CONF_gGpsFixNtp;
bool TUAL_CONF_gGpsFixPtp;
long TUAL_CONF_gGpsErr;
char TUAL_CONF_gts2phcOffset[20];
char TUAL_CONF_gptp4lOffset[20];
bool TUAL_CONF_gts2phcZeroEvent;
uint8_t TUAL_CONF_gts2phcOffsetLen;
uint8_t TUAL_CONF_gptp4lOffsetLen;
bool TUAL_CONF_gEth;

uint8_t TUAL_CONF_WriteNtpServer()
{
    FILE *configFile;

    if (gNtpConfig.broadcastOrServer == false) // false=server, true=broadcast
    {

        switch (gNtpConfig.serverNum)
        {
        case 0:
            if ((configFile = fopen(TUAL_CONF_NTP_SERVER0_FILE, "w")) == NULL)
            {
                return 1;
            }
            goto write;
        case 1:
            if ((configFile = fopen(TUAL_CONF_NTP_SERVER1_FILE, "w")) == NULL)
            {
                return 1;
            }
            goto write;
        case 2:
            if ((configFile = fopen(TUAL_CONF_NTP_SERVER2_FILE, "w")) == NULL)
            {
                return 1;
            }
            goto write;
        case 3:
            if ((configFile = fopen(TUAL_CONF_NTP_SERVER3_FILE, "w")) == NULL)
            {
                return 1;
            }
            goto write;
        default:
            return 1;
        }
    write:

        fprintf(configFile, "server %s", gNtpConfig.serverAddress);
        if (gNtpConfig.key == true)
        {
            fprintf(configFile, " key %d", gNtpConfig.keyNum);
        }
        if (gNtpConfig.iburst == true)
        {
            fprintf(configFile, " iburst");
        }
        fclose(configFile);
        if ((configFile = fopen(TUAL_CONF_NTP_BROADCAST_FILE, "w")) == NULL)
        {
            return 1;
        }
        fprintf(configFile, " ");
        fclose(configFile);
        return 0;
    }
    else if (gNtpConfig.broadcastOrServer == true)
    {
        if ((configFile = fopen(TUAL_CONF_NTP_BROADCAST_FILE, "w")) == NULL)
        {
            return 1;
        }

        fprintf(configFile, "broadcast %d %s",
                gNtpConfig.broadcastInterval, gNtpConfig.serverAddress);
        fclose(configFile);

        if ((configFile = fopen(TUAL_CONF_NTP_SERVER0_FILE, "w")) == NULL)
        {
            return 1;
        }
        fprintf(configFile, " ");
        fclose(configFile);
        return 0;
    }

    if (gNtpConfig.localStratum == true)
    {
        if ((configFile = fopen(TUAL_CONF_NTP_LOCAL_STRATUM_FILE, "w")) == NULL)
        {
            return 1;
        }
        fprintf(configFile, "local stratum %d", gNtpConfig.localStratumNum);
        fclose(configFile);
    }
}

char *TUAL_CONF_CreateJsonStringNtp()
{
    char *string = NULL;
    cJSON *jsonNtpConfig = cJSON_CreateObject();

    cJSON_AddBoolToObject(jsonNtpConfig, "ntpOrPtp",
                          gNtpConfig.ntpOrPtp);
    cJSON_AddBoolToObject(jsonNtpConfig, "state",
                          gNtpConfig.state);
    cJSON_AddBoolToObject(jsonNtpConfig, "broadcastOrServer",
                          gNtpConfig.broadcastOrServer);
    cJSON_AddNumberToObject(jsonNtpConfig, "serverNum",
                            gNtpConfig.serverNum);
    cJSON_AddNumberToObject(jsonNtpConfig, "broadcastInterval",
                            gNtpConfig.broadcastInterval);
    cJSON_AddStringToObject(jsonNtpConfig, "serverAddress",
                            gNtpConfig.serverAddress);
    cJSON_AddBoolToObject(jsonNtpConfig, "key",
                          gNtpConfig.key);
    cJSON_AddNumberToObject(jsonNtpConfig, "keyNum",
                            gNtpConfig.keyNum);
    cJSON_AddBoolToObject(jsonNtpConfig, "iburst",
                          gNtpConfig.iburst);
    cJSON_AddBoolToObject(jsonNtpConfig, "localStratum",
                          gNtpConfig.localStratum);
    cJSON_AddNumberToObject(jsonNtpConfig, "localStratumNum",
                            gNtpConfig.localStratumNum);

    string = cJSON_Print(jsonNtpConfig);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print monitor.\n");
    }
    printf("%s", string);
    cJSON_Delete(jsonNtpConfig);
    return string;
}

char *TUAL_CONF_CreateJsonStringPtp()
{
    char *string = NULL;
    cJSON *jsonPtpConfig = cJSON_CreateObject();

    cJSON_AddBoolToObject(jsonPtpConfig, "ntpOrPtp",
                          gPtpConfig.ntpOrPtp);
    cJSON_AddBoolToObject(jsonPtpConfig, "state",
                          gPtpConfig.state);
    cJSON_AddBoolToObject(jsonPtpConfig, "masterOrSlave",
                          gPtpConfig.masterOrSlave);
    cJSON_AddBoolToObject(jsonPtpConfig, "delay_mechanismE2eOrP2p",
                          gPtpConfig.delay_mechanismE2eOrP2p);
    cJSON_AddStringToObject(jsonPtpConfig, "domainNumber",
                            gPtpConfig.domainNumber);
    cJSON_AddStringToObject(jsonPtpConfig, "priority1",
                            gPtpConfig.priority1);
    cJSON_AddStringToObject(jsonPtpConfig, "priority2",
                            gPtpConfig.priority2);
    cJSON_AddStringToObject(jsonPtpConfig, "logAnnounceInterval",
                            gPtpConfig.logAnnounceInterval);
    cJSON_AddStringToObject(jsonPtpConfig, "logSyncInterval",
                            gPtpConfig.logSyncInterval);
    cJSON_AddStringToObject(jsonPtpConfig, "logMinDelayReqInterval",
                            gPtpConfig.logMinDelayReqInterval);
    cJSON_AddStringToObject(jsonPtpConfig, "announceReceiptTimeout",
                            gPtpConfig.announceReceiptTimeout);

    string = cJSON_Print(jsonPtpConfig);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print monitor.\n");
    }
    printf("%s", string);
    cJSON_Delete(jsonPtpConfig);
    return string;
}

uint8_t TUAL_CONF_ParseString(const char *const configString)
{
    uint8_t status = 1;
    // cJSON *jsonConfig = cJSON_ParseWithLength(configString, sizeof(configString));

    cJSON *jsonConfig = cJSON_Parse(configString);
    char *jsonConfigstr = cJSON_Print(jsonConfig);
    printf("\nPARSING:%s\n", jsonConfigstr);

    if (jsonConfig == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Parsed message is not json!: %s\n", error_ptr);
        }

        cJSON_Delete(jsonConfig);
        return 1;
    }

    cJSON *ntpOrPtp = cJSON_GetObjectItemCaseSensitive(jsonConfig, "ntpOrPtp");
    if (cJSON_IsBool(ntpOrPtp) == true)
    {
        if (cJSON_IsTrue(ntpOrPtp) == true)
        {
            status = 2; // PARSESTRING FUNCTION WILL RETURN 2 IF INCOMING CONF IS NTP.
            gNtpConfig.ntpOrPtp = cJSON_IsTrue(ntpOrPtp);

            cJSON *state = cJSON_GetObjectItemCaseSensitive(jsonConfig, "state");
            if (cJSON_IsBool(state) != false)
            {
                gNtpConfig.state = cJSON_IsTrue(state);
            }

            cJSON *serverAddress = cJSON_GetObjectItemCaseSensitive(jsonConfig, "serverAddress");
            if (cJSON_IsString(serverAddress) && (serverAddress->valuestring != NULL))
            {
                strcpy(gNtpConfig.serverAddress, serverAddress->valuestring);
            }
            cJSON *broadcastOrServer = cJSON_GetObjectItemCaseSensitive(jsonConfig, "broadcastOrServer");
            if (cJSON_IsBool(broadcastOrServer) != false)
            {
                gNtpConfig.broadcastOrServer = cJSON_IsTrue(broadcastOrServer);
            }

            cJSON *broadcastInterval = cJSON_GetObjectItemCaseSensitive(jsonConfig, "broadcastInterval");
            if (cJSON_IsNumber(broadcastInterval) != false)
            {
                gNtpConfig.broadcastInterval = broadcastInterval->valuedouble;
            }
            cJSON *key = cJSON_GetObjectItemCaseSensitive(jsonConfig, "key");
            if (cJSON_IsBool(key) != false)
            {
                gNtpConfig.key = cJSON_IsTrue(key);
            }
            cJSON *keyNum = cJSON_GetObjectItemCaseSensitive(jsonConfig, "keyNum");
            if (cJSON_IsNumber(keyNum) != false)
            {
                gNtpConfig.keyNum = keyNum->valuedouble;
            }
            cJSON *serverNum = cJSON_GetObjectItemCaseSensitive(jsonConfig, "serverNum");
            if (cJSON_IsNumber(serverNum) != false)
            {
                gNtpConfig.serverNum = serverNum->valuedouble;
            }
            cJSON *localStratum = cJSON_GetObjectItemCaseSensitive(jsonConfig, "localstratum");
            if (cJSON_IsBool(localStratum) != false)
            {
                gNtpConfig.localStratum = cJSON_IsTrue(localStratum);
            }
            cJSON *localStratumNum = cJSON_GetObjectItemCaseSensitive(jsonConfig, "localstratumNum");
            if (cJSON_IsNumber(localStratumNum) != false)
            {
                gNtpConfig.localStratumNum = localStratumNum->valuedouble;
            }
            cJSON *iburst = cJSON_GetObjectItemCaseSensitive(jsonConfig, "iburst");
            if (cJSON_IsBool(iburst) != false)
            {
                gNtpConfig.localStratum = cJSON_IsTrue(iburst);
            }
        }
        else if (cJSON_IsTrue(ntpOrPtp) == false)
        {
            status = 3; // PARSESTRING FUNCTION WILL RETURN 3 IF INCOMING CONF IS PTP.
            gPtpConfig.ntpOrPtp = cJSON_IsTrue(ntpOrPtp);
            cJSON *state = cJSON_GetObjectItemCaseSensitive(jsonConfig, "state");
            if (cJSON_IsBool(state) != false)
            {
                gPtpConfig.state = cJSON_IsTrue(state);
            }
            cJSON *masterOrSlave = cJSON_GetObjectItemCaseSensitive(jsonConfig, "masterOrSlave");
            if (cJSON_IsBool(masterOrSlave) != false)
            {
                gPtpConfig.masterOrSlave = cJSON_IsTrue(masterOrSlave);
            }
            cJSON *delay_mechanismE2eOrP2p = cJSON_GetObjectItemCaseSensitive(jsonConfig, "delay_mechanismE2eOrP2p");
            if (cJSON_IsBool(delay_mechanismE2eOrP2p) != false)
            {
                gPtpConfig.delay_mechanismE2eOrP2p = cJSON_IsTrue(delay_mechanismE2eOrP2p);
            }
            cJSON *domainNumber = cJSON_GetObjectItemCaseSensitive(jsonConfig, "domainNumber");
            if (cJSON_IsString(domainNumber) && (domainNumber->valuestring != NULL))
            {
                strcpy(gPtpConfig.domainNumber, domainNumber->valuestring);
            }
            cJSON *priority1 = cJSON_GetObjectItemCaseSensitive(jsonConfig, "priority1");
            if (cJSON_IsString(priority1) && (priority1->valuestring != NULL))
            {
                strcpy(gPtpConfig.priority1, priority1->valuestring);
            }
            cJSON *priority2 = cJSON_GetObjectItemCaseSensitive(jsonConfig, "priority2");
            if (cJSON_IsString(priority2) && (priority2->valuestring != NULL))
            {
                strcpy(gPtpConfig.priority2, priority2->valuestring);
            }
            cJSON *logAnnounceInterval = cJSON_GetObjectItemCaseSensitive(jsonConfig, "logAnnounceInterval");
            if (cJSON_IsString(logAnnounceInterval) && (logAnnounceInterval->valuestring != NULL))
            {
                strcpy(gPtpConfig.logAnnounceInterval, logAnnounceInterval->valuestring);
            }
            cJSON *logSyncInterval = cJSON_GetObjectItemCaseSensitive(jsonConfig, "logSyncInterval");
            if (cJSON_IsString(logSyncInterval) && (logSyncInterval->valuestring != NULL))
            {
                strcpy(gPtpConfig.logSyncInterval, logSyncInterval->valuestring);
            }
            cJSON *logMinDelayReqInterval = cJSON_GetObjectItemCaseSensitive(jsonConfig, "logMinDelayReqInterval");
            if (cJSON_IsString(logMinDelayReqInterval) && (logMinDelayReqInterval->valuestring != NULL))
            {
                strcpy(gPtpConfig.logMinDelayReqInterval, logMinDelayReqInterval->valuestring);
            }
            cJSON *announceReceiptTimeout = cJSON_GetObjectItemCaseSensitive(jsonConfig, "announceReceiptTimeout");
            if (cJSON_IsString(announceReceiptTimeout) && (announceReceiptTimeout->valuestring != NULL))
            {
                strcpy(gPtpConfig.announceReceiptTimeout, announceReceiptTimeout->valuestring);
            }
        }
    }

    cJSON_Delete(jsonConfig);
    return status;
}

uint8_t TUAL_CONF_WritePipe(char *string, char *pipeFile)
{
    FILE *fd;
    mknod(pipeFile, S_IFIFO | 0640, 0);
    if ((fd = fopen(pipeFile, "w")) == NULL)
    {
        printf("Can't open the file:%s\n", pipeFile);
        return 1;
    }
    fprintf(fd, "%s", string);
    fclose(fd);
    return 0;
}

uint8_t TUAL_CONF_ReadPipe(char *pipeFile)
{
    int fd;
    char readbuffer[300];
    uint8_t read_bytes;
    uint8_t status;

    if ((fd = open(pipeFile, O_RDONLY)) == -1)
    {
        printf("Can't read the file:%s\n", pipeFile);
        return 1;
    }

    readbuffer[read_bytes] = '\0';
    if ((read_bytes = read(fd, readbuffer, sizeof(readbuffer))) != -1)
    {
        readbuffer[read_bytes] = '\0';
        printf("New pipe data:%s", readbuffer);
    }
    if ((status = TUAL_CONF_ParseString(readbuffer)) == 1)
    {
        close(fd);
        return 1;
    }

    close(fd);
    return status;
}

uint8_t TUAL_CONF_WritePipeBlock(char *string, char *pipeFile)
{
    int fd;
    mknod(pipeFile, S_IFIFO | 0640, 0);
    if ((fd = open(pipeFile, O_CREAT | O_WRONLY)) == -1)
    {
        printf("Can't open the file:%s\n", pipeFile);
        return 1;
    }

    write(fd, string, strlen(string));

    close(fd);
    return 0;
}

uint8_t TUAL_CONF_Write2Row(char *row2, char *variable4, char *value6)
{
    uint8_t status;
    char part1[100];
    char part3[] = "s/.*/";
    char part5[] = " ";
    char part7[] = "/' /etc/linuxptp/ptp4l.conf";
    strcpy(part1, "sed -i '");
    strcat(part1, row2);
    strcat(part1, part3);
    strcat(part1, variable4);
    strcat(part1, part5);
    strcat(part1, value6);
    strcat(part1, part7);

    if ((status = system(part1)) == 0)
    {
        return 0;
        printf("Write2Row function failed while writing: %s", part1);
    }
    else
        return 1;
    return 0;
}

uint8_t TUAL_CONF_WritePtpServer()
{

    TUAL_CONF_Write2Row("10", "domainNumber",
                        gPtpConfig.domainNumber);
    TUAL_CONF_Write2Row("8", "priority1",
                        gPtpConfig.priority1);
    TUAL_CONF_Write2Row("9", "priority2",
                        gPtpConfig.priority2);
    TUAL_CONF_Write2Row("25", "logAnnounceInterval",
                        gPtpConfig.logAnnounceInterval);
    TUAL_CONF_Write2Row("26", "logSyncInterval",
                        gPtpConfig.logSyncInterval);
    TUAL_CONF_Write2Row("28", "logMinDelayReqInterval",
                        gPtpConfig.logMinDelayReqInterval);
    TUAL_CONF_Write2Row("31", "announceReceiptTimeout",
                        gPtpConfig.announceReceiptTimeout);
    if (gPtpConfig.delay_mechanismE2eOrP2p == true)
    {
        TUAL_CONF_Write2Row("98", "delay_mechanismE2eOrP2p", "E2E");
    }
    else if (gPtpConfig.delay_mechanismE2eOrP2p == false)
    {
        TUAL_CONF_Write2Row("98", "delay_mechanismE2eOrP2p", "P2P");
    }

    if (gPtpConfig.masterOrSlave == true)
    {
        TUAL_CONF_Write2Row("6", "slaveOnly", "0");
        TUAL_CONF_Write2Row("36", "masterOnly", "1");
    }
    else if (gPtpConfig.masterOrSlave == false)
    {
        TUAL_CONF_Write2Row("6", "slaveOnly", "1");
        TUAL_CONF_Write2Row("36", "masterOnly", "0");
    }
    return 0;

    return 1;
}

uint8_t TUAL_CONF_ReadGpsd()
{
    struct gps_data_t gps_data;

    if (0 != gps_open("localhost", "2947", &gps_data))
    {
        printf("Open error.  Bye, bye\n");
        return 1;
    }

    (void)gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, NULL);

    while (gps_waiting(&gps_data, 5000000))
    {
        if (-1 == gps_read(&gps_data, NULL, 0))
        {
            printf("Can't read from gpsd.\n");
            break;
        }

        TUAL_CONF_gGpsFixNtp = gps_data.fix.mode;
        TUAL_CONF_gGpsErr = gps_data.qErr;

        if (TIME_SET == (TIME_SET & gps_data.set))
        {
            // not 32 bit safe
            printf("GPS Time:%ld.%09ld\n", gps_data.fix.time.tv_sec,
                   gps_data.fix.time.tv_nsec);
        }
        sleep(2);
        printf("\nGps fix:%d\n", TUAL_CONF_gGpsFixNtp);
    }

    // When you are done...
    (void)gps_stream(&gps_data, WATCH_DISABLE, NULL);
    (void)gps_close(&gps_data);
    return 0;
}

uint8_t TUAL_CONF_ReadTs2phc()
{
    // Open the text file for reading
    FILE *fd = fopen(TUAL_CONF_TS2PHCLOG, "r");
    if (fd == NULL)
    {
        printf("Couldn't open TS2PHCLOG.\n");
        return 1;
    }

    // Read each line of the text file
    char line[200];
    char str3[20];
    char str5[20];
    char str2[20];
    while (fgets(line, sizeof(line), fd))
    {
        // Extract the desired column from the line

        sscanf(line, "%*s %s %s %*s %s", str2, str3, str5);
        if (strcmp(str3, "master") == 0)
        {
            if (7 > strlen(str5))
            {
                strcpy(TUAL_CONF_gts2phcOffset, str5);
                TUAL_CONF_gts2phcOffsetLen = strlen(str5);
                printf("TS2PHC OFFSET:%s\n", TUAL_CONF_gts2phcOffset);
                // TUAL_ST7066_PositionXYString(28, 1, "           ");
                // TUAL_ST7066_PositionXYString(28, 1, TUAL_CONF_gts2phcOffset);
            }
            else
            {
                strcpy(TUAL_CONF_gts2phcOffset, "Exceed      ");
                printf("TS2PHC OFFSET:%s\n", TUAL_CONF_gts2phcOffset);
                // printf("\nExceed ts2phcOffset:%s\n", str5);
            }
        }

        if (strcmp(str2, "poll") == 0)
        {
            if (TUAL_CONF_gGpsFixPtp == true)
            {

                TUAL_CONF_gGpsFixPtp = false;
                if (Pages.CurrentPage->pageNum == 0)
                {
                    TUAL_ST7066_PositionXYString(28, 0, "Nofix.");
                }
            }

            TUAL_CONF_gts2phcZeroEvent = true;
            printf("zero event\n");
        }
        else
        {
            if (TUAL_CONF_gGpsFixPtp == false)
            {
                TUAL_CONF_gGpsFixPtp = true;
                if (Pages.CurrentPage->pageNum == 0)
                {
                    TUAL_ST7066_PositionXYString(28, 0, "Fixed.");
                }
            }
            TUAL_CONF_gts2phcZeroEvent = false;
        }
    }
    fclose(fd);
}

void TUAL_CONF_CopyStructs()
{
    gprevNtpConfig = gNtpConfig;
    gprevPtpConfig = gPtpConfig;
}

uint8_t TUAL_CONF_StopPtp()
{
    system(TUAL_CONF_TS2PHCKILL);
    return 0;
}

uint8_t TUAL_CONF_StartPtp()
{
    TUAL_ST7066_DisplayClear();
    TUAL_ST7066_PositionXYString(0, 0, "Ptp initializing...");
    if (gPtpConfig.masterOrSlave == true)
    {
        system(TUAL_CONF_PTP4LMASTERSTART);
        sleep(30);
    }
    else if (gPtpConfig.masterOrSlave == false)
    {
        system(TUAL_CONF_PTP4LSLAVESTART);
        sleep(30);
    }

    // burda ts2phc offseti kontrol edilebilir?? -done

    return 0;
}

uint8_t TUAL_CONF_StopNtp()
{
    sleep(3);
    system(TUAL_CONF_CHRONYKILL);
    return 0;
}

uint8_t TUAL_CONF_StartNtp()
{

    TUAL_ST7066_DisplayClear();
    TUAL_ST7066_PositionXYString(0, 0, "Ntp initializing...");
    system(TUAL_CONF_CHRONYSTART);
    sleep(3);
    // system("cgps &");
    // sleep(10);
    // system("pkill cgps");

    return 0;
}

uint8_t TUAL_CONF_SaveConfig()
{
    TUAL_CONF_WriteTxt(TUAL_CONF_CreateJsonStringPtp(), TUAL_CONF_PTPCONFFILE);
    TUAL_CONF_WriteTxt(TUAL_CONF_CreateJsonStringNtp(), TUAL_CONF_NTPCONFFILE);
}

uint8_t TUAL_CONF_ReadConfig(char *File)
{
    TUAL_CONF_ParseString(TUAL_CONF_ReadFile(File));
}

uint8_t TUAL_CONF_WriteTxt(char *string, char *File)
{
    FILE *fd;
    if ((fd = fopen(File, "w")) == NULL)
    {
        printf("Cant open %s to write.\n", File);
        return 1;
    }
    fprintf(fd, "%s", string);

    fclose(fd);
    return 0;
}

uint8_t TUAL_CONF_GetPtp4l()
{
    // Open the text file for reading
    FILE *fd = fopen(TUAL_CONF_PTP4LLOG, "r");
    if (fd == NULL)
    {
        printf("Couldn't open PTP4L log.\n");
        return 1;
    }

    // Read each line of the text file
    char line[200];
    char str3[20];
    char str5[20];
    char str2[20];
    while (fgets(line, sizeof(line), fd))
    {
        // Extract the desired column from the line

        sscanf(line, "%*s %s %s %*s %s", str2, str3, str5);
        if (strcmp(str3, "master") == 0)
        {
            if (7 > strlen(str5))
            {
                strcpy(TUAL_CONF_gptp4lOffset, str5);
                TUAL_CONF_gptp4lOffsetLen = strlen(str5);
                printf("\nPTP4L OFFSET:%s\n", TUAL_CONF_gptp4lOffset);
                // TUAL_ST7066_PositionXYString(28, 1, "           ");
                // TUAL_ST7066_PositionXYString(28, 1, TUAL_CONF_gts2phcOffset);
            }
            else
            {
                strcpy(TUAL_CONF_gptp4lOffset, "Exceed      ");
                printf("PTP4L OFFSET:%s", TUAL_CONF_gptp4lOffset);
                // printf("\nExceed ts2phcOffset:%s\n", str5);
            }
        }
    }
    fclose(fd);
}

uint8_t TUAL_CONF_CheckEth()
{
    int fdFifo;
    FILE *fdCarrier;
    int eth = 0;
    int len;

    if ((fdCarrier = fopen(TUAL_CONF_ETHCRFILE, "r")) == NULL)
    {
        printf("Can't access ETHCRFILE file.\n");
        return 1;
    }

    // First assignment of ethernet info from kernel file, ETHCRFILE.

    fscanf(fdCarrier, "%d", &eth);

    if (eth == 1)
    {
        TUAL_CONF_gEth = true;
        printf("\nEthernet cable is connected.\n");
    }
    else
    {
        TUAL_CONF_gEth = false;
        printf("\nEthernet cable is not connected.\n");
    }

    fclose(fdCarrier);

    // Read ethernet status from pipe, ETHFILE.
    // mkfifo(fdFifo, 0666);

    while (1)
    {

        if ((fdFifo = open(TUAL_CONF_ETHFILE, O_RDONLY)) == -1)
        {
            printf("Can't access ETHFILE.\n");
            return 1;
        }
        if ((len = read(fdFifo, &eth, 1)) != -1)
        {
            printf("len:%d\n", len);
            printf("eth:%d\n", eth);
            if (eth == 49)
            {
                TUAL_CONF_gEth = true;
                printf("\nEthernet cable connected.\n");
            }
            else if (eth == 48)
            {
                TUAL_CONF_gEth = false;
                printf("\nEthernet cable disconnected.\n");
            }

            close(fdFifo);
        }
        else
        {
            printf("Couldn't read from fdFifo.\n");
        }
    }

    return 0;
}

static char *TUAL_CONF_ReadFile(const char *filename)
{
    FILE *file = NULL;
    long length = 0;
    char *content = NULL;
    size_t read_chars = 0;

    /* open in read binary mode */
    file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Couldn't open %s to read.\n", filename);
        goto cleanup;
    }

    /* get the length */
    if (fseek(file, 0, SEEK_END) != 0)
    {
        goto cleanup;
    }
    length = ftell(file);
    if (length < 0)
    {
        goto cleanup;
    }
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        goto cleanup;
    }

    /* allocate content buffer */
    content = (char *)malloc((size_t)length + sizeof(""));
    if (content == NULL)
    {
        goto cleanup;
    }

    /* read the file into memory */
    read_chars = fread(content, sizeof(char), (size_t)length, file);
    if ((long)read_chars != length)
    {
        free(content);
        content = NULL;
        goto cleanup;
    }
    content[read_chars] = '\0';

cleanup:
    if (file != NULL)
    {
        fclose(file);
    }

    return content;
}
