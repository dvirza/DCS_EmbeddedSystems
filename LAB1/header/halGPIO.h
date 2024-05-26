#ifndef _halGPIO_H_
#define _halGPIO_H_

#include  "../header/bsp.h"    		// private library - BSP layer
#include  "../header/app.h"    		// private library - APP layer


extern enum FSMstate state;   // global variable
extern enum SYSmode lpm_mode; // global variable
extern int toggleCount; // global variable
extern int state1Timer; // global variable
extern int state2Timer; // global variable
extern int ledValS1; // global variable
extern int ledValS2; // global variable



extern void sysConfig(void);
extern void print2LEDs(unsigned char);
extern void clrLEDs(void);
extern void toggleLEDs(char);
extern void setLEDs(char);
extern unsigned char readSWs(void);
extern void delay(unsigned int);
extern void enterLPM(unsigned char);
extern void incLEDs(char);
extern void enable_interrupts();
extern void disable_interrupts();
extern void ShiftLeftLED();
extern void P20_PWM(int DutyCycle);
extern void EnablePBinterrupts(unsigned char pbs);


extern __interrupt void PBs_handler(void);

#endif







