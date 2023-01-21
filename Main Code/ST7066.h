// registers
#ifndef _ST7066_H
#define _ST7066_H

#include <pthread.h>

#define RS 2 // RS pin
#define RW 3 // RW pin
#define RE 4 // RE pin
// data pins
#define D0 17
#define D1 27
#define D2 22
#define D3 23
// used in 4 bit mode
#define D4 24
#define D5 10
#define D6 9
#define D7 11

#define TUAL_ST7066_BUSY_FLAG D7
#define TUAL_ST7066_INIT_SEQ 0x30
#define TUAL_ST7066_DISP_CLEAR 0x01
#define TUAL_ST7066_DISP_OFF 0x08
#define TUAL_ST7066_DISP_ON 0x0C
#define TUAL_ST7066_CURSOR_ON 0x0E
#define TUAL_ST7066_CURSOR_OFF 0x0C
#define TUAL_ST7066_CURSOR_BLINK 0x0F
#define TUAL_ST7066_RETURN_HOME 0x02
#define TUAL_ST7066_ENTRY_MODE 0x06
#define TUAL_ST7066_4BIT_MODE 0x20
#define TUAL_ST7066_8BIT_MODE 0x30
#define TUAL_ST7066_2_ROWS 0x08
#define TUAL_ST7066_FONT_5x8 0x00
#define TUAL_ST7066_FONT_5x10 0x04
#define TUAL_ST7066_POSITION 0x80

#define Orta 0b10000011
#define Ileri 0b10000101

#define TUAL_ST7066_SHIFT 0x10
#define TUAL_ST7066_DISPLAY 0x08
#define TUAL_ST7066_LEFT 0x00
#define TUAL_ST7066_RIGHT 0x04
#define TUAL_ST7066_CURSOR 0x00

#define TUAL_ST7066_CLEARLINE "                   "

#define TUAL_ST7066_ROWS 2  // 16x2 ekran için sadece cols değişiyor
#define TUAL_ST7066_COLS 40 // 40x2 için row 2 start aynı
#define TUAL_ST7066_COLS2 80
#define TUAL_ST7066_ROW1_START 0x00
#define TUAL_ST7066_ROW1_END TUAL_ST7066_COLS
#define TUAL_ST7066_ROW2_START 0x40
#define TUAL_ST7066_ROW2_END TUAL_ST7066_COLS2

#define TUAL_ST7066_MODE TUAL_ST7066_4BIT_MODE

#define TUAL_ST7066_CLRBIT(REG) \
    {                           \
        GPIO_CLR = 1 << REG;    \
    }
#define TUAL_ST7066_SETBIT(REG) \
    {                           \
        GPIO_SET = 1 << REG;    \
    }

#define ERROR 1

void TUAL_ST7066_SetD0toD7output();
void TUAL_ST7066_ClrPin(uint8_t PIN);
void TUAL_ST7066_SetPin(uint8_t PIN);
void TUAL_ST7066_Send8bitsIn8bitMode(unsigned short int buffer);
void TUAL_ST7066_SendInstruction(unsigned short int data);
void TUAL_ST7066_Initialize8bit();
void TUAL_ST7066_Disable();
void TUAL_ST7066_Enable();
void TUAL_ST7066_PulseE();
void TUAL_ST7066_SendData(unsigned short int data);
void TUAL_ST7066_DrawChar(char character);
void TUAL_ST7066_DrawString(char *str);
void TUAL_ST7066_DisplayClear();
void TUAL_ST7066_DisplayOn(void);
void TUAL_ST7066_CursorOn(void);
void TUAL_ST7066_CursorOff(void);
void TUAL_ST7066_CursorBlink(void);
char TUAL_ST7066_Shift(char item, char direction);
void TUAL_ST7066_ShiftStringRight(char *str);
void TUAL_ST7066_DisplayOff(void);
void TUAL_ST7066_SekilliYazi(char *str);
char TUAL_ST7066_PositionXY(uint8_t x, uint8_t y);
void TUAL_ST7066_Msleep(uint8_t time);
void TUAL_ST7066_Startup();
void TUAL_ST7066_GetandPrintTime();
void TUAL_ST7066_RemoveAllPulls();
void TUAL_ST7066_SetAllOutput();
void TUAL_ST7066_GetandPrintTime();
void TUAL_ST7066_DrawWall();
uint8_t TUAL_ST7066_PositionXYString(uint8_t x, uint8_t y, char *str);
void TUAL_TIMES_ChangePage(uint8_t num);
void TUAL_ST7066_SetUppNibble(unsigned short int data);
void TUAL_ST7066_SetLowNibble(unsigned short int data);
void TUAL_ST7066_Send8bitsIn4bitMode(unsigned short int data);
void TUAL_ST7066_Send4bitsIn4bitMode(unsigned short int data);
void TUAL_ST7066_SetD4toD7output();
void TUAL_ST7066_Send4bitsIn4bitMode(unsigned short int data);
void TUAL_ST7066_Initialize4bitNew();
void TUAL_ST7066_SetD4toD7pullup();
void TUAL_ST7066_SetD4toD7input();
void TUAL_ST7066_CheckBFin4bitMode();
#endif