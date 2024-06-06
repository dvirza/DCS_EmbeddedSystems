#include  "../header/halGPIO.h"     // private library - HAL layer


//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	GPIOconfig();
	TIMERconfig();
	ADCconfig();
}
//--------------------------------------------------------------------
// 				Print Byte to 8-bit LEDs array 
//--------------------------------------------------------------------
void print2LEDs(unsigned char ch){
	LEDsArrPort = ch;
}    
//--------------------------------------------------------------------
//				Clear 8-bit LEDs array 
//--------------------------------------------------------------------
void clrLEDs(void){
	LEDsArrPort = 0x000;
}
//--------------------------------------------------------------------
//		Transmit PWM signal with a input dc from port 2 [0]
//--------------------------------------------------------------------
void P27_PWM(int time1,int time0){
  if (time0 == 0xFFFF){
    Port2Out &= 0x07;
  }
  else if (time1 == 0xFFFF){
      Port2Out |= 0x80;
  }
  else
  {//given the clock freq is 1MHZ
    // T = 1/1,048,576s
     Port2Out |= 0x80; // P2[0} = '1'
     delay(time1);
    //delay 2500 cycles total
     Port2Out &= 0x07; // // P2[0} = '0'
     delay(time0);
  }   
  }

//--------------------------------------------------------------------
//				Toggle 8-bit LEDs array 
//--------------------------------------------------------------------
void toggleLEDs(char ch){
	LEDsArrPort ^= ch;
}
//--------------------------------------------------------------------
//				Shift left 8-bit LEDs array
//--------------------------------------------------------------------
void ShiftLeftLED(){
    LEDsArrPort = (LEDsArrPort*2) % 255;
}
//--------------------------------------------------------------------
//				Set 8-bit LEDs array 
//--------------------------------------------------------------------
void setLEDs(char ch){
	LEDsArrPort |= ch;
}
//--------------------------------------------------------------------
//				Read value of 4-bit SWs array 
//--------------------------------------------------------------------
unsigned char readSWs(void){
	unsigned char ch;
	
	ch = PBsArrPort;
	ch &= ~SWmask;     // mask the least 4-bit
	return ch;
}
//---------------------------------------------------------------------
//             Increment / decrement LEDs shown value 
//---------------------------------------------------------------------
void incLEDs(char val){
	LEDsArrPort += val;
}
//---------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  // t[msec]
	volatile unsigned int i;
	
	for(i=t; i>0; i--);
}
//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03) 
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}
//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//--------------------------------------------------------------------
//            Selects which interrupts are enabled interrupts
//--------------------------------------------------------------------
void EnablePBinterrupts(unsigned char pbs){ // 
  PBsArrIntEn |= pbs;
}

void DisablePBinterrupts(unsigned char pbsToSetOff){
  PBsArrIntEn &= ~pbsToSetOff;
}
void ClearLEDs(void){
  LEDsArrPort = 0x00;
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}
//*********************************************************************
//            Port2 Interrupt Service Rotine
//*********************************************************************
#pragma vector=PORT2_VECTOR
  __interrupt void PBs_handler(void){
   
	delay(debounceVal);
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------
	if(PBsArrIntPend & PB0){
          toggleCount = toggleCount ^ 1; // change direction of counting for each time returning to this state
          LEDsArrPort = ledValS1;        // prints the last value of state 1 on the led screen
	  state = state1;
          state1Timer = 0;               // resets the timer of this state
	  PBsArrIntPend &= ~PB0;
        }
        else if(PBsArrIntPend & PB1){
	  LEDsArrPort = ledValS2;        // prints the last value of state 2 on the led screen
          state = state2;
          state2Timer = 0;               // resets the timer of this state
	  PBsArrIntPend &= ~PB1; 
        }
	else if(PBsArrIntPend & PB2){ 
	  state = state3;
          PBsArrIntPend &= ~PB2;
        }
        else if(PBsArrIntPend & PB3){
          state = state4;
          PBsArrIntPend &= ~PB3;
        }



//---------------------------------------------------------------------
//            Exit from a given LPM 
//---------------------------------------------------------------------	
        switch(lpm_mode){
		case mode0:
		 LPM0_EXIT; // must be called from ISR only
		 break;
		 
		case mode1:
		 LPM1_EXIT; // must be called from ISR only
		 break;
		 
		case mode2:
		 LPM2_EXIT; // must be called from ISR only
		 break;
                 
                case mode3:
		 LPM3_EXIT; // must be called from ISR only
		 break;
                 
                case mode4:
		 LPM4_EXIT; // must be called from ISR only
		 break;
	}
        
}
 