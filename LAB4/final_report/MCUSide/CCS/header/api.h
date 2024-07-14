#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer

extern unsigned int num;
extern int Trans7Pointer;


extern void count(void);
extern void buzzer(void);
extern void set_X(void);
extern void measADC10(void);
extern void get_X();
extern void int2String(int integer, char str[6]);


#endif
