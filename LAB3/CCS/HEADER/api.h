#ifndef _api_H_
#define _api_H_

#define FREQ_STR_LEN 14

#include  "../header/halGPIO.h"     // private library - HAL layer

extern void printSWs2LEDs(void);
extern void printArr2SWs(char Arr[], int size, unsigned int rate);

extern void idiomrecorder(void);

extern char ST1str[6];
extern int unsigned SampledFreq;
extern unsigned char readSWs(void);
extern const char * LCD_state1_txt;

#endif
