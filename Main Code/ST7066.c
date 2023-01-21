#define BCM2708_PERI_BASE 0x3F000000
#define GPIO_BASE (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <string.h>
#include <pigpio.h>
#include <time.h>
#include <pthread.h>
#include "ST7066.h"
#include "configEditor.h"
#include "timeserver.h"
#include <time.h>

#define PAGE_SIZE (4 * 1024)
#define BLOCK_SIZE (4 * 1024)

static uint8_t dataPins[] = {D0, D1, D2, D3, D4, D5, D6, D7};
char SCREEN_BUFFER[80];

pthread_mutex_t timeserverMutex = PTHREAD_MUTEX_INITIALIZER;

void TUAL_ST7066_modeSelect(uint8_t registers_sw) // tamam-test et
{
    switch (registers_sw)
    {
    case 0:
        TUAL_ST7066_ClrPin(RS);
        TUAL_ST7066_ClrPin(RW);
        break;

    case 1:
        TUAL_ST7066_ClrPin(RS);
        TUAL_ST7066_SetPin(RW);
        break;

    case 2:
        TUAL_ST7066_SetPin(RS);
        TUAL_ST7066_ClrPin(RW);
        break;

    case 3:
        TUAL_ST7066_SetPin(RS);
        TUAL_ST7066_SetPin(RW);
        break;

    default:
        break;
    }
}

void TUAL_ST7066_Enable() // tamam-test et
{
    TUAL_ST7066_SetPin(RE);
}

void TUAL_ST7066_Disable() // tamam-test et
{
    TUAL_ST7066_ClrPin(RE);
}

void TUAL_ST7066_Initialize8bit() // tamam-test et
{

    gpioSetMode(RE, PI_OUTPUT);
    gpioSetMode(RW, PI_OUTPUT);
    gpioSetMode(RS, PI_OUTPUT);

    TUAL_ST7066_SetD0toD7output();

    TUAL_ST7066_ClrPin(RS);
    TUAL_ST7066_ClrPin(RW);

    usleep(16000);

    TUAL_ST7066_SendInstruction(TUAL_ST7066_INIT_SEQ);
    usleep(4500);
    TUAL_ST7066_PulseE();
    usleep(150);
    TUAL_ST7066_PulseE();
    usleep(50);
    TUAL_ST7066_SendInstruction(TUAL_ST7066_8BIT_MODE | TUAL_ST7066_2_ROWS | TUAL_ST7066_FONT_5x8);
    TUAL_ST7066_SendInstruction(TUAL_ST7066_DISP_OFF);
    TUAL_ST7066_SendInstruction(TUAL_ST7066_DISP_CLEAR);
    TUAL_ST7066_SendInstruction(TUAL_ST7066_ENTRY_MODE);
}

void TUAL_ST7066_Initialize4bit() // tamam-test et
{

    gpioSetMode(RE, PI_OUTPUT);
    gpioSetMode(RW, PI_OUTPUT);
    gpioSetMode(RS, PI_OUTPUT);

    TUAL_ST7066_SetD4toD7output();
    usleep(16000);

    TUAL_ST7066_ClrPin(RS);
    TUAL_ST7066_ClrPin(RW);
    TUAL_ST7066_ClrPin(RE);
    TUAL_ST7066_Send4bitsIn4bitMode(0b0011); // 00110000
    usleep(4300);
    TUAL_ST7066_PulseE();
    usleep(150);
    TUAL_ST7066_Send4bitsIn4bitMode(0b0011);

    TUAL_ST7066_Send4bitsIn4bitMode(0b0010);
    TUAL_ST7066_Send4bitsIn4bitMode(0b0010);
    TUAL_ST7066_Send4bitsIn4bitMode(0b0010);

    TUAL_ST7066_Send4bitsIn4bitMode(0b0000);
    TUAL_ST7066_Send4bitsIn4bitMode(0b1000);

    TUAL_ST7066_Send4bitsIn4bitMode(0b0000);
    TUAL_ST7066_Send4bitsIn4bitMode(0b0001);

    TUAL_ST7066_Send4bitsIn4bitMode(0b0000);
    TUAL_ST7066_Send4bitsIn4bitMode(0b0110);
}

void TUAL_ST7066_Initialize4bitNew() // tamam-test et
{
    gpioSetMode(RE, PI_OUTPUT);
    gpioSetMode(RW, PI_OUTPUT);
    gpioSetMode(RS, PI_OUTPUT);

    TUAL_ST7066_SetD4toD7output();

    TUAL_ST7066_ClrPin(RS);
    TUAL_ST7066_ClrPin(RW);
    TUAL_ST7066_ClrPin(RE);
    usleep(16000);
    TUAL_ST7066_Send4bitsIn4bitMode(TUAL_ST7066_INIT_SEQ);
    usleep(4500);
    TUAL_ST7066_Send4bitsIn4bitMode(TUAL_ST7066_INIT_SEQ);
    usleep(150);
    TUAL_ST7066_Send4bitsIn4bitMode(TUAL_ST7066_INIT_SEQ);
    usleep(50);
    TUAL_ST7066_Send4bitsIn4bitMode(TUAL_ST7066_4BIT_MODE);

    TUAL_ST7066_SendInstruction(TUAL_ST7066_4BIT_MODE | 0x02 | TUAL_ST7066_FONT_5x8);
    TUAL_ST7066_SendInstruction(TUAL_ST7066_DISP_OFF);
    TUAL_ST7066_SendInstruction(TUAL_ST7066_DISP_CLEAR);
    TUAL_ST7066_SendInstruction(TUAL_ST7066_ENTRY_MODE);
}

void TUAL_ST7066_SendInstruction(unsigned short int data) // modifiye
{
    // Clear RS
    TUAL_ST7066_ClrPin(RS);
    // send required data in required mode
    if (TUAL_ST7066_MODE == TUAL_ST7066_8BIT_MODE)
    {
        TUAL_ST7066_Send8bitsIn8bitMode(data);
    }
    else if (TUAL_ST7066_MODE == TUAL_ST7066_4BIT_MODE)
    {
        TUAL_ST7066_Send8bitsIn4bitMode(data);
        // TUAL_ST7066_CheckBFin4bitMode();
    }
    usleep(500);
    // check busy flag
}

void TUAL_ST7066_SendData(unsigned short int data) // modifiye
{
    // Set RS
    TUAL_ST7066_SetPin(RS);
    // send required data in required mode
    if (TUAL_ST7066_MODE == TUAL_ST7066_8BIT_MODE)
    {
        TUAL_ST7066_Send8bitsIn8bitMode(data);
    }
    else if (TUAL_ST7066_MODE == TUAL_ST7066_4BIT_MODE)
    {
        TUAL_ST7066_Send8bitsIn4bitMode(data);
        // TUAL_ST7066_CheckBFin4bitMode();
    }
    usleep(50);
    // check busy flag
    // TUAL_ST7066_CheckBFin8bitMode();
    TUAL_ST7066_ClrPin(RS);
}

void TUAL_ST7066_Send8bitsIn8bitMode(unsigned short int buffer) // modifiye
{
    uint8_t binary[] = {0, 0, 0, 0, 0, 0, 0, 0};

    int i = 0;

    // Set E
    TUAL_ST7066_SetPin(RE);

    // Send data to LCD
    while (buffer > 0)
    {
        binary[i] = buffer % 2;
        buffer = buffer / 2;
        i++;
    }

    for (int g = 7; g >= 0; g--)
    {
        if (binary[g] == 1)
        {
            TUAL_ST7066_SetPin(dataPins[g]);
            // usleep(50);
        }
        else if (binary[g] == 0)
        {
            TUAL_ST7066_ClrPin(dataPins[g]);
            // usleep(50);
        }
    }
    usleep(50);
    // PWeh delay time > 450ns
    // Clear E
    TUAL_ST7066_ClrPin(RE);
    // TcycE > 1000ns -> delay depends on PWeh delay time
    // delay = TcycE - PWeh = 1000 - 500 = 500ns
    usleep(50);
}

void TUAL_ST7066_ClrPin(uint8_t PIN) // tamam-test et
{
    gpioWrite(PIN, 0);
}

void TUAL_ST7066_SetPin(uint8_t PIN) // tamam-test et
{
    gpioWrite(PIN, 1);
}

void TUAL_ST7066_PulseE()
{
    // Set E
    TUAL_ST7066_SetPin(RE);
    // PWeh delay time > 450ns
    usleep(1);
    // Clear E
    TUAL_ST7066_ClrPin(RE);
    // TcycE > 1000ns -> delay depends on PWeh delay time
    // delay = TcycE - PWeh = 1000 - 500 = 500ns
    usleep(1);
}

void TUAL_ST7066_SetD0toD7output()
{
    gpioSetMode(D0, PI_OUTPUT);
    gpioSetMode(D1, PI_OUTPUT);
    gpioSetMode(D2, PI_OUTPUT);
    gpioSetMode(D3, PI_OUTPUT);
    gpioSetMode(D4, PI_OUTPUT);
    gpioSetMode(D5, PI_OUTPUT);
    gpioSetMode(D6, PI_OUTPUT);
    gpioSetMode(D7, PI_OUTPUT);
}

void TUAL_ST7066_DrawString(char *str)
{
    unsigned char i = 0;
    // loop through 5 bytes
    while (str[i] != '\0')
    {
        // read characters and increment index
        TUAL_ST7066_SendData(str[i++]);
    }
}

void TUAL_ST7066_DrawChar(char character)
{
    // Diplay clear
    TUAL_ST7066_SendData(character);
}

void TUAL_ST7066_DisplayOn(void)
{
    // send instruction - display on
    TUAL_ST7066_SendInstruction(TUAL_ST7066_DISP_ON);
}

void TUAL_ST7066_DisplayClear(void)
{
    // Diplay clear
    TUAL_ST7066_SendInstruction(TUAL_ST7066_DISP_CLEAR);
}

void TUAL_ST7066_CursorBlink(void)
{
    // send instruction - Cursor blink
    TUAL_ST7066_SendInstruction(TUAL_ST7066_CURSOR_BLINK);
}

void TUAL_ST7066_CursorOff(void)
{
    // send instruction - cursor on
    TUAL_ST7066_SendInstruction(TUAL_ST7066_CURSOR_OFF);
}

void TUAL_ST7066_CursorOn(void)
{
    // send instruction - cursor on
    TUAL_ST7066_SendInstruction(TUAL_ST7066_CURSOR_ON);
}

char TUAL_ST7066_Shift(char item, char direction)
{
    // check if item is cursor or display or direction is left or right
    if ((item != TUAL_ST7066_DISPLAY) && (item != TUAL_ST7066_CURSOR))
    {
        // error
        return ERROR;
    }
    // check if direction is left or right
    if ((direction != TUAL_ST7066_RIGHT) && (direction != TUAL_ST7066_LEFT))
    {
        // error
        return ERROR;
    }

    // cursor shift
    if (item == TUAL_ST7066_CURSOR)
    {
        // right shift
        if (direction == TUAL_ST7066_RIGHT)
        {
            // shit cursor / display to right / left
            TUAL_ST7066_SendInstruction(TUAL_ST7066_SHIFT | TUAL_ST7066_CURSOR | TUAL_ST7066_RIGHT);
        }
        else
        {
            // shit cursor / display to right / left
            TUAL_ST7066_SendInstruction(TUAL_ST7066_SHIFT | TUAL_ST7066_CURSOR | TUAL_ST7066_LEFT);
        }
        // display shift
    }
    else
    {
        // right shift
        if (direction == TUAL_ST7066_RIGHT)
        {
            // shit cursor / display to right / left
            TUAL_ST7066_SendInstruction(TUAL_ST7066_SHIFT | TUAL_ST7066_DISPLAY | TUAL_ST7066_RIGHT);
        }
        else
        {
            // shit cursor / display to right / left
            TUAL_ST7066_SendInstruction(TUAL_ST7066_SHIFT | TUAL_ST7066_DISPLAY | TUAL_ST7066_LEFT);
        }
    }
    // success
    return 0;
}

void TUAL_ST7066_ShiftStringRight(char *str)
{
    TUAL_ST7066_DrawString(str);

    while (1)
    {
        usleep(100000);

        TUAL_ST7066_Shift(TUAL_ST7066_DISPLAY, TUAL_ST7066_RIGHT);
        // TUAL_ST7066_SendInstruction(TUAL_ST7066_DISP_CLEAR);

        usleep(100000);
    }
}

char TUAL_ST7066_PositionXY(uint8_t x, uint8_t y)
{
    if (x > TUAL_ST7066_COLS || y > TUAL_ST7066_ROWS)
    {
        return ERROR;
    }
    switch (y)
    {
    case 0:
        TUAL_ST7066_SendInstruction(TUAL_ST7066_POSITION + TUAL_ST7066_ROW1_START + x);
        break;
    case 1:

        TUAL_ST7066_SendInstruction(TUAL_ST7066_POSITION + TUAL_ST7066_ROW2_START + x);
        break;
    }
}

void TUAL_ST7066_DisplayOff(void)
{
    // send instruction - display on
    TUAL_ST7066_SendInstruction(TUAL_ST7066_DISP_OFF);
}

void TUAL_ST7066_SekilliYazi(char *str)
{
    TUAL_ST7066_DisplayClear();
    uint8_t x, y;
    while (1)
    {

        for (x = 0; x <= 40 - strlen(str); x++)
        {
            switch (y)
            {
            case 0:
                TUAL_ST7066_PositionXY(x, y);
                TUAL_ST7066_DrawString(str);
                y = 1;
                break;
            case 1:
                TUAL_ST7066_PositionXY(x, y);
                TUAL_ST7066_DrawString(str);
                y = 0;
                break;
            }
            x = x + strlen(str) - 1;
            TUAL_ST7066_Msleep(250);
            TUAL_ST7066_DisplayClear();
            TUAL_ST7066_Msleep(50);
        }

        x = x - strlen(str) + 1;

        for (x; x >= 0 + strlen(str); x--)
        {
            x = x - strlen(str) + 1;
            switch (y)
            {
            case 0:
                TUAL_ST7066_PositionXY(x, y);
                TUAL_ST7066_DrawString(str);
                y = 1;
                break;
            case 1:
                TUAL_ST7066_PositionXY(x, y);
                TUAL_ST7066_DrawString(str);
                y = 0;
                break;
            }

            TUAL_ST7066_Msleep(250);
            TUAL_ST7066_DisplayClear();
            TUAL_ST7066_Msleep(50);
        }
    }
}

void TUAL_ST7066_Msleep(uint8_t time)
{
    for (int i = 0; i < 1000; i++)
    {
        usleep(time);
    }
}

void TUAL_ST7066_RemoveAllPulls()
{
    printf("RE %d\n", gpioSetPullUpDown(RE, PI_PUD_OFF));
    printf("RW %d\n", gpioSetPullUpDown(RW, PI_PUD_OFF));
    printf("RS %d\n", gpioSetPullUpDown(RS, PI_PUD_OFF));
    // data pins
    printf("D0 %d\n", gpioSetPullUpDown(D0, PI_PUD_OFF));
    printf("D1 %d\n", gpioSetPullUpDown(D1, PI_PUD_OFF));
    printf("D2 %d\n", gpioSetPullUpDown(D2, PI_PUD_OFF));
    printf("D3 %d\n", gpioSetPullUpDown(D3, PI_PUD_OFF));
    printf("D4 %d\n", gpioSetPullUpDown(D4, PI_PUD_OFF));
    printf("D5 %d\n", gpioSetPullUpDown(D5, PI_PUD_OFF));
    printf("D6 %d\n", gpioSetPullUpDown(D6, PI_PUD_OFF));
    printf("D7 %d\n", gpioSetPullUpDown(D7, PI_PUD_OFF));
}

void TUAL_ST7066_SetAllOutput()
{

    gpioSetMode(RE, PI_OUTPUT);
    gpioSetMode(RW, PI_OUTPUT);
    gpioSetMode(RS, PI_OUTPUT);
    // data pins
    gpioSetMode(D0, PI_OUTPUT);
    gpioSetMode(D1, PI_OUTPUT);
    gpioSetMode(D2, PI_OUTPUT);
    gpioSetMode(D3, PI_OUTPUT);
    gpioSetMode(D4, PI_OUTPUT);
    gpioSetMode(D5, PI_OUTPUT);
    gpioSetMode(D6, PI_OUTPUT);
    gpioSetMode(D7, PI_OUTPUT);
}

void TUAL_ST7066_Startup()
{
    if (TUAL_ST7066_MODE == TUAL_ST7066_8BIT_MODE)
    {
        TUAL_ST7066_Initialize8bit();
    }
    else if (TUAL_ST7066_MODE == TUAL_ST7066_4BIT_MODE)
    {
        TUAL_ST7066_Initialize4bitNew();
    }

    TUAL_ST7066_DisplayOn();
}

void TUAL_ST7066_GetandPrintTime()
{
    TUAL_ST7066_DisplayClear();
    time_t rawtime;
    struct tm *info;
    time(&rawtime);
    info = localtime(&rawtime);
    memcpy(SCREEN_BUFFER, asctime(info), strlen(asctime(info) + 1));
    TUAL_ST7066_PositionXYString(0, 0, SCREEN_BUFFER);
    printf("%s", SCREEN_BUFFER);
}

void TUAL_ST7066_DrawWall()
{
    TUAL_ST7066_PositionXY(19, 0);
    TUAL_ST7066_DrawChar('|');
    TUAL_ST7066_PositionXY(19, 1);
    TUAL_ST7066_DrawChar('|');
}

uint8_t TUAL_ST7066_PositionXYString(uint8_t x, uint8_t y, char *str)
{
    pthread_mutex_lock(&timeserverMutex);

    switch (y)
    {
    case 0:
        TUAL_ST7066_SendInstruction(TUAL_ST7066_POSITION + TUAL_ST7066_ROW1_START + x);
        break;
    case 1:

        TUAL_ST7066_SendInstruction(TUAL_ST7066_POSITION + TUAL_ST7066_ROW2_START + x);
        break;
    }
    unsigned char i = 0;

    while (str[i] != '\0')
    {
        // read characters and increment index
        TUAL_ST7066_SendData(str[i++]);
    }
    pthread_mutex_unlock(&timeserverMutex);
}

void TUAL_ST7066_SetUppNibble(unsigned short int data)
{
    // clear bits DB7-DB4
    TUAL_ST7066_ClrPin(D7);
    TUAL_ST7066_ClrPin(D6);
    TUAL_ST7066_ClrPin(D5);
    TUAL_ST7066_ClrPin(D4);

    // set DB7-DB4 if corresponding bit is set

    if (data & 0x80)
    {
        TUAL_ST7066_SetPin(D7);
    }
    if (data & 0x40)
    {
        TUAL_ST7066_SetPin(D6);
    }
    if (data & 0x20)
    {
        TUAL_ST7066_SetPin(D5);
    }
    if (data & 0x10)
    {
        TUAL_ST7066_SetPin(D4);
    }
    usleep(50);
}

void TUAL_ST7066_SetLowNibble(unsigned short int data)
{
    // clear bits DB7-DB4
    TUAL_ST7066_ClrPin(D3);
    TUAL_ST7066_ClrPin(D2);
    TUAL_ST7066_ClrPin(D1);
    TUAL_ST7066_ClrPin(D0);

    // set DB7-DB4 if corresponding bit is set

    if (data & 0x80)
    {
        TUAL_ST7066_SetPin(D3);
    }
    if (data & 0x40)
    {
        TUAL_ST7066_SetPin(D2);
    }
    if (data & 0x20)
    {
        TUAL_ST7066_SetPin(D1);
    }
    if (data & 0x10)
    {
        TUAL_ST7066_SetPin(D0);
    }
    usleep(50);
}

void TUAL_ST7066_Send8bitsIn4bitMode(unsigned short int data)
{
    // Send upper nibble
    // ----------------------------------
    // Set E
    TUAL_ST7066_SetPin(RE);
    // send data to LCD
    TUAL_ST7066_SetUppNibble(data);
    // PWeh delay time > 450ns
    usleep(1);
    // Clear E
    TUAL_ST7066_ClrPin(RE);
    // TcycE > 1000ns -> delay depends on PWeh delay time
    // delay = TcycE - PWeh = 1000 - 500 = 500ns
    usleep(1);

    // Send lower nibble
    // ----------------------------------
    // Set E
    TUAL_ST7066_SetPin(RE);
    // send data to LCD
    TUAL_ST7066_SetUppNibble(data << 4);
    // PWeh delay time > 450ns
    usleep(1);
    // Clear E
    TUAL_ST7066_ClrPin(RE);
    // TcycE > 1000ns -> delay depends on PWeh delay time
    // delay = TcycE - PWeh = 1000 - 500 = 500ns
    usleep(1);
}

void TUAL_ST7066_Send4bitsIn4bitMode(unsigned short int data)
{
    // Set E
    TUAL_ST7066_SetPin(RE);
    // send data to LCD
    TUAL_ST7066_SetUppNibble(data);
    // PWeh delay time > 450ns
    usleep(1);
    // Clear E
    TUAL_ST7066_ClrPin(RE);
    // TcycE > 1000ns -> delay depends on PWeh delay time
    // delay = TcycE - PWeh = 1000 - 500 = 500ns
    usleep(1);
}

void TUAL_ST7066_CheckBFin4bitMode()
{
    unsigned char input = 0;
    TUAL_ST7066_SetD4toD7input();

    TUAL_ST7066_ClrPin(RS);
    TUAL_ST7066_SetPin(RW);

    while (1)
    {

        // Read upper nibble
        // --------------------------------
        // Set E
        TUAL_ST7066_SetPin(RE);
        // PWeh > 0.5us
        usleep(1);
        // read upper nibble (tDDR > 360ns)
        input = gpioRead(D7);
        // Clear E
        TUAL_ST7066_ClrPin(RE);
        // TcycE > 1000ns -> delay depends on PWeh delay time
        // delay = TcycE - PWeh = 1000 - 500 = 500ns
        usleep(1);

        // Read lower nibble
        // --------------------------------
        // Set E
        TUAL_ST7066_SetPin(RE);
        // PWeh > 0.5us
        usleep(1);
        // read lower nibble (tDDR > 360ns)
        input |= gpioRead(D7) >> 4;
        // Clear E
        TUAL_ST7066_ClrPin(RE);
        // TcycE > 1000ns -> delay depends on PWeh delay time
        // delay = TcycE - PWeh = 1000 - 500 = 500ns
        usleep(1);
        printf("flag busy\n");
        // check if DB7 is cleared
        if (!(input & (1 << gpioRead(D7))))
        {
            // if BF cleared -> end loop
            break;
        }
    }
    printf("flag released.\n");
    // clear RW
    TUAL_ST7066_ClrPin(RW);

    // set DB7-DB4 as output
    TUAL_ST7066_SetD4toD7output();
}

void TUAL_ST7066_SetD4toD7input()
{
    gpioSetMode(D4, PI_INPUT);
    gpioSetMode(D5, PI_INPUT);
    gpioSetMode(D6, PI_INPUT);
    gpioSetMode(D7, PI_INPUT);
}

void TUAL_ST7066_SetD4toD7output()
{
    gpioSetMode(D4, PI_OUTPUT);
    gpioSetMode(D5, PI_OUTPUT);
    gpioSetMode(D6, PI_OUTPUT);
    gpioSetMode(D7, PI_OUTPUT);
}

void TUAL_ST7066_SetD4toD7pullup()
{
    gpioSetPullUpDown(D4, PI_PUD_DOWN);
    gpioSetPullUpDown(D5, PI_PUD_DOWN);
    gpioSetPullUpDown(D6, PI_PUD_DOWN);
    gpioSetPullUpDown(D7, PI_PUD_DOWN);
}