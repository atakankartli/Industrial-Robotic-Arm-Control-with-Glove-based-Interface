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
uint8_t TUAL_CONF_ReadTs2phc();
uint8_t TUAL_CONF_ReadPipe(char *pipeFile);

int main()
{
    while (1)
    {
        TUAL_CONF_ReadPipe(TUAL_CONF_TS2PHCLOG);
    }

    return 0;
}

uint8_t TUAL_CONF_ReadTs2phc()
{
    // Open the text file for reading
    int fd;

    if ((fd = open(TUAL_CONF_TS2PHCLOG, O_RDONLY)) == -1)
    {
        printf("Can't read the file:%s\n", TUAL_CONF_TS2PHCLOG);
        return 1;
    }

    // Read each line of the text file
    char line[BUFSIZ];
    char str3[20];
    char str5[20];
    char str2[20];
    uint8_t read_bytes;
    line[read_bytes] = '\0';
    if ((read_bytes = read(fd, line, sizeof(line))) != -1)
    {

        // Extract the desired column from the line
        printf("LINE:%s\n", line);
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
            }

            TUAL_CONF_gts2phcZeroEvent = true;
            printf("zero event\n");
        }
        else
        {
            if (TUAL_CONF_gGpsFixPtp == false)
            {
                TUAL_CONF_gGpsFixPtp = true;
            }
            TUAL_CONF_gts2phcZeroEvent = false;
        }
    }
    close(fd);
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

    close(fd);
    return status;
}
