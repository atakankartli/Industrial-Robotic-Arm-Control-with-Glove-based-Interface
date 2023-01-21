#ifndef _displayPages_H
#define _displayPages_H
#include <stdint.h>
#include "ST7066.h"
typedef struct
{
    uint8_t pageNum;
    uint8_t rowNum;
    // bool child;
    // bool parent;
    bool changable;
    uint8_t cursorPos;
    char row0[TUAL_ST7066_COLS / 2 - 1];
    char row1[TUAL_ST7066_COLS / 2 - 1];
    char row2[TUAL_ST7066_COLS / 2 - 1];
    char row3[TUAL_ST7066_COLS / 2 - 1];

    struct TUAL_TIMES_sPage *child0;
    struct TUAL_TIMES_sPage *child1;
    struct TUAL_TIMES_sPage *child2;
    struct TUAL_TIMES_sPage *child3;
    struct TUAL_TIMES_sPage *parent;

} TUAL_TIMES_sPage;
typedef struct
{
    TUAL_TIMES_sPage page0;
    TUAL_TIMES_sPage page1;
    TUAL_TIMES_sPage page2;
    TUAL_TIMES_sPage *CurrentPage;
} TUAL_TIMES_Pages;

#endif