#ifndef _api_H_
#define _api_H_

#define FREQ_STR_LEN 14

#include  "../header/halGPIO.h"     // private library - HAL layer

extern void printSWs2LEDs(void);
extern void printArr2SWs(char Arr[], int size, unsigned int rate);
extern const char * LCD_state1_txt;
extern void idiomrecord(void);
extern char idiom_recorder[32];
extern void Merge(void);
extern int unsigned SampledFreq;
extern unsigned char readSWs(void);
extern int TimerBDone;
extern int State3Arrray[9];
extern void DMAPrepareST3 (int* Src);

void ChooseSentence(void);
void DMAstate2(char*, char*, char*);
void showString(char*);
void LedSwitch(void);
void DMA2Leds(int * Src);

#endif
