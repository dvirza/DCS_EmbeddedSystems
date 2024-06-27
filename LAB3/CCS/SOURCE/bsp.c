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
  PBsArrPortSel &= ~0x0f;
  PBsArrPortDir &= ~0x0f;
  PBsArrIntEdgeSel |= 0x03;          // pull-up mode
  PBsArrIntEdgeSel &= ~0x0c;         // pull-down mode
  PBsArrIntEn |= 0x0f;
  PBsArrIntPend &= ~0x0f;            // clear pending interrupts

  
  // LEDs Setup
  LEDsArrPort = 0x00;                // clear leds array
  LEDsArrPortDir |= 0xFF;             // Use leds us output
  LEDsArrPortSel &= ~0xFF;           // I/O pins mode
  
  
  //Key-Pad Setup
  KPDir = 0x0f;
  KPOut = 0x00;
  KPArrPortSel &= ~0x02;
  KPArrPortDir &= ~0x02;
  KPArrIntEdgeSel |= 0x02;
  KPArrIntPend &= ~0x02;

  
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
void TIMERconfig(void){
    TBCTL = TBSSEL_1 + ID_2 + TBCLR;
    TB0CCTL0 = CCIE;    // TimerB IE

}


void DMAconfig(void){

       //DMACTL0 = DMA0TSEL_0 + DMA1TSEL_8; // first dma turnes on manually and secound turnes on by timer b compare
       DMACTL0 = DMA0TSEL_0; // software triggered
       DMA0CTL = DMADT_1 + DMADSTBYTE + DMASRCBYTE + DMAIE + DMADSTINCR_3 + DMASRCINCR_3; // block mode , enable, interrupt enable, dec order
       DMA1SZ = 0x001;
       DMA1CTL = DMADT_1 + DMASBDB + DMAIE; // block mode , byte-to-byte , enable
       //DMA1SA = (void (*)())&ledsValue;
       //DMA1DA = (void (*)())&LEDsArrPort; //LEDsVal
       DMA1DA = &LEDsArrPort;
}



