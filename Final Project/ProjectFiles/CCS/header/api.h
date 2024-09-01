#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer
extern void UART_TransmitInt(const int * val);


extern unsigned int count;
extern unsigned int FullRoundSteps;
extern void stepCount(unsigned int *MPos);
extern void Calibrate();
extern void PauseDuration(unsigned int);
extern void MotorCWStep(unsigned int *Pos);
extern void MotorCWStep(unsigned int *Pos);
extern void JoyStickIE();
extern void JoyStickID();
extern void rotateMotor();
extern unsigned int CalibToggle;

extern void ScriptMode(void);





#endif
