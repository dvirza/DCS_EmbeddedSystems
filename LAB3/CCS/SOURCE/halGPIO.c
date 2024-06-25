#include  "../header/halGPIO.h"     // private library - HAL layer
int cyc = 0;

//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	GPIOconfig();
	TIMERconfig();
	ADCconfig();
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
//void ClearLEDs(void){
//  LEDsArrPort = 0x00;
//}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
 
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
  
            
            
//*********************************************************************
//            Port1 Interrupt Service Rotine
//*********************************************************************
#pragma vector=PORT1_VECTOR
  __interrupt void PBs_handler(void){
   
	delay(debounceVal);
        
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------
	if(PBsArrIntPend & PB0){
          state = state1;
               // resets the timer of this state
	  PBsArrIntPend &= ~PB0;
        }
        else if(PBsArrIntPend & PB1){
	  state = state2;
                         // resets the timer of this state
	  PBsArrIntPend &= ~PB1; 
        }
	else if(PBsArrIntPend & PB2){ 
	  state = state3;
          PBsArrIntPend &= ~PB2;
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

  //*********************************************************************
  //            Port2 Interrupt Service Rotine
  //*********************************************************************
#pragma vector=PORT2_VECTOR
  __interrupt void KPI_handler(void){

      //start counting timerB0 For staying in this routine
      //while (TimerB0)
      delay(debounceVal);

      KB = 'K';
      Port10Out = 0x0E;
      if (( Port10In & 0x10 ) == 0 ) KB = 'D';
      else if (( Port10In & 0x20 ) == 0 ) KB = 'E';
      else if (( Port10In & 0x40 ) == 0 ) KB = '0';
      else if (( Port10In & 0x80 ) == 0 ) KB = 'F';

      Port10Out = 0x0D;
      if (( Port10In & 0x10 ) == 0 ) KB = 'C';
      else if (( Port10In & 0x20 ) == 0 ) KB = '9';
      else if (( Port10In & 0x40 ) == 0 ) KB = '8';
      else if (( Port10In & 0x80 ) == 0 ) KB = '7';

      Port10Out = 0x0B;
      if (( Port10In & 0x10 ) == 0 ) KB = 'B';
      else if (( Port10In & 0x20 ) == 0 ) KB = '6';
      else if (( Port10In & 0x40 ) == 0 ) KB = '5';
      else if (( Port10In & 0x80 ) == 0 ) KB = '4';

      Port10Out = 0x07;
      if (( Port10In & 0x10 ) == 0 ) KB = 'A';
      else if (( Port10In & 0x20 ) == 0 ) KB = '3';
      else if (( Port10In & 0x40 ) == 0 ) KB = '2';
      else if (( Port10In & 0x80 ) == 0 ) KB = '1';

      }
  }



//************************************************************************************
//            TimerB Interrupt Service Rotine
//************************************************************************************


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMERB0_VECTOR//TIMERB0_VECTOR||TIMER0_B0_VECTOR
__interrupt void Timer_B (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B0_VECTOR))) Timer_B (void)
#else
#error Compiler not supported!
#endif
{
    KPToggle = 0
    LPM0_EXIT;
}

//******************************************************************************
//            TimerB Functions
//******************************************************************************
void enable_TimerBCCIE(){
    TB0CCTL0 |= CCIE;
}

void disable_TimerBCCIE(){
    TB0CCTL0 &= ~(CCIE);


//---------------------------------------------------------------------
//            LCD Functions 
//---------------------------------------------------------------------	




//******************************************************************
// send a command to the LCD
//******************************************************************
void lcd_cmd(unsigned char c){
  
	LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

	if (LCD_MODE == FOURBIT_MODE)
	{
		LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
                LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
		lcd_strobe();
                LCD_DATA_WRITE &= ~OUTPUT_DATA;
    		LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
		lcd_strobe();
	}
	else
	{
		LCD_DATA_WRITE = c;
		lcd_strobe();
	}
}
//******************************************************************
// send data to the LCD
//******************************************************************
void lcd_data(unsigned char c){
        
	LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

	LCD_DATA_WRITE &= ~OUTPUT_DATA;       
	LCD_RS(1);
	if (LCD_MODE == FOURBIT_MODE)
	{
    		LCD_DATA_WRITE &= ~OUTPUT_DATA;
                LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;  
		lcd_strobe();		
                LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
                LCD_DATA_WRITE &= ~OUTPUT_DATA;
		LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET; 
		lcd_strobe();
	}
	else
	{
		LCD_DATA_WRITE = c;
		lcd_strobe();
	}
          
	LCD_RS(0);   
}
//******************************************************************
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){
  
	while(*s)
		lcd_data(*s++);
}
//******************************************************************
// initialize the LCD
//******************************************************************
void lcd_init(){
  
	char init_value;

	if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
        else init_value = 0x3F;
	
	LCD_RS_DIR(OUTPUT_PIN);
	LCD_EN_DIR(OUTPUT_PIN);
	LCD_RW_DIR(OUTPUT_PIN);
        LCD_DATA_DIR |= OUTPUT_DATA;
        LCD_RS(0);
	LCD_EN(0);
	LCD_RW(0);
        
	DelayMs(15);
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
	LCD_DATA_WRITE |= init_value;
	lcd_strobe();
	DelayMs(5);
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
	LCD_DATA_WRITE |= init_value;
	lcd_strobe();
	DelayUs(200);
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
	LCD_DATA_WRITE |= init_value;
	lcd_strobe();
	
	if (LCD_MODE == FOURBIT_MODE){
		LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
                LCD_DATA_WRITE &= ~OUTPUT_DATA;
		LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
		lcd_strobe();
		lcd_cmd(0x28); // Function Set
	}
        else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots 
	
	lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
	lcd_cmd(0x1); //Display Clear
	lcd_cmd(0x6); //Entry Mode
	lcd_cmd(0x80); //Initialize DDRAM address to zero
}
//******************************************************************
// Delay usec functions
//******************************************************************
void DelayUs(unsigned int cnt){
  
	unsigned char i;
        for(i=cnt ; i>0 ; i--) asm("nop"); // tha command asm("nop") takes raphly 1usec
	
}
//******************************************************************
// Delay msec functions
//******************************************************************
void DelayMs(unsigned int cnt){
  
	unsigned char i;
        for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec
	
}
//******************************************************************
// lcd strobe functions
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
  asm("nop");
  asm("nop");
  LCD_EN(0);
}
