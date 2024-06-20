#include  "../header/bsp.h"    // private library - BSP layer

//-----------------------------------------------------------------------------  
//           GPIO congiguration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
   
  // Switches Setup
  SWsArrPortSel &= ~0x01;
  SWsArrPortDir &= ~0x01;
  
  // PushButtons Setup
  PBsArrPortSel &= ~0x07;
  PBsArrPortDir &= ~0x07;
  PBsArrIntEdgeSel |= 0x30;  	     // pull-up mode
  PBsArrIntEdgeSel &= ~0x04;         // pull-down mode
  PBsArrIntEn |= 0x07;
  PBsArrIntPend &= ~0x07;            // clear pending interrupts 
  
  Port2Select &= 0xEF;
  Port2Dir &= 0xEF;
  Port2Dir |= 0x04;
  
    //genrator Setup 
  GENsArrPortSel |= 0x10;
  GENsArrPortDir &= ~0x10;
  
  //buzer Setup 
   BUZER_OUT     &= ~0x04;
   BUZER_DIR     |= 0x04;
   BUZER_SEL     |= 0x04;
   
  //LCD Setup 
   LCD_DATA_WRITE      &= ~0xFF;
   LCD_DATA_DIR        |= 0xF0;
   LCD_DATA_SEL        &= ~0xF0;
   LCD_CTL_SEL         &= ~0xE0;
  
  
  
  _BIS_SR(GIE);                     // enable interrupts globally
}                       

//------------------------------------------------------------------------------------- 
//            Timers congiguration 
//-------------------------------------------------------------------------------------
void TIMERconfig(void){//de
        TA0CTL = TASSEL_2+ TACLR + ID_3 + MC_3;  // timer is off SMCLK /8 = 0.5 sec interupt
        TA0CCTL0 = CCIE;    //interupt enable
        TA0CCR0 = 0xFFFF;      // value of 0.5 sec
	TA1CTL = TASSEL_2 +TACLR;   // choosing SMCLK
        TA1CCTL1 = OUTMOD_0;   // for state3
        TA1CCTL2 = CM_1 + SCS + CAP;     // Selecting Capture Edge +SYNC + Capture Mode  
} 
//------------------------------------------------------------------------------------- 
//            ADC congiguration 
//-------------------------------------------------------------------------------------
void ADCconfig(void){
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON + ADC10IE;
    ADC10CTL1 = INCH_3 + ADC10SSEL_3;
    ADC10AE0 |= BIT3;
}              
