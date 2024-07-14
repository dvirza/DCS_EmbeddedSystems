#include  "../header/halGPIO.h"     // private library - HAL layer
#include "stdio.h"
#include<string.h>


char* LoveMy = "I love my Negev\n";
char ReceivedData;
//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	GPIOconfig();
	TIMER0_A0_config();
    TIMER1_A0_config();
	ADCconfig();
	UARTconfig();
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
	  _BIS_SR(LPM0_bits + GIE);     /* Enter Low Power Mode 0 */
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
//            LCD
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
//                        init the LCD
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
    lcd_cmd(0x0C); // remove curser
}
//******************************************************************
// Delay usec functions
//******************************************************************
void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) asm(" nop"); // tha command asm("nop") takes raphly 1usec

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
  asm(" NOP");
 // asm("NOP");
  LCD_EN(0);
}

//-------------------------------------------------------------
//                         Blink RGB
//-------------------------------------------------------------
void blinkRGB(){
    unsigned int i = 0;
    RGBArrPortOut &= ~0x07;
    while(state == state1){
        RGBArrPortOut++;
        i++;
        if(i == 8){
            RGBArrPortOut &= ~0x07;
            i = 0;
        }
        startTimerA0();
        enterLPM(mode0);
        stopTimerA0();
    }
    RGBArrPortOut = 0x00;
}
//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}
 
//*********************************************************************
//            TimerA0 Interrupt Service Routine
//*********************************************************************
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR (void){
  if (Timer0_CTL & TAIFG){
    Timer0_CTL &= ~TAIFG;            //turn off flag
    LPM0_EXIT;
  }
}

//-------------------------------------------------------------
//              StartTimer and FinishTimer For Count UpDown
//-------------------------------------------------------------
void startTimerA0(){
    Timer0_CTL |= TASSEL_2 + MC_3 + ID_3 + TACLR; //  [SMCLK] + [Up/Down]  [/8]
    Timer0_CTL |= TAIE;                           //  TimerA0 Interrupt Enable
}

void stopTimerA0(){
    Timer0_CTL |= TASSEL_2 + MC_0 + ID_3 + TACLR;
    Timer0_CTL &= ~TAIE;
}

//-------------------------------------------------------------
//   start/shutdown timer and enable/disable interrupts
//-------------------------------------------------------------
void startTimerA1(){
    Timer1_CTL |= TASSEL_2 + MC_1 + ID_0 + TACLR; //[SMCLK] + [UPMODE UP->TACCR0] + [/1] + [TIMERA CLEAR]
}

void stopTimerA1(){
    Timer1_CTL |= TASSEL_2 + MC_0 + ID_0 + TACLR; // [SMCLK] + [STOP MODE-HALTED] + [/1] + [TIMERA CLEAR]
}

void enableTimerA1_bzr(){
    Timer1_CCTL2 |=  OUTMOD_7; // [RESET/SET]
}

void disableTimerA1_bzr(){
    Timer1_CCTL2 &=  ~OUTMOD_7;
}

void get_X(){
    state = state4;
    if (ReceivedData == '\n'){
        timerInput[X_flag - 1] = ReceivedData;
        timerInput[X_flag - 1] = '\0';
        X_flag = 0;
        state = state4;
    }
    else if (ReceivedData >= '0' && ReceivedData <= '9') {
        timerInput[X_flag - 1] = ReceivedData;
        X_flag++;
    }
    else{
        X_flag = 404;
        state = state0;
    }
}

//*********************************************************************
//            ADC10 Vector Interrupt Service Routine
//*********************************************************************
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    LPM0_EXIT;
}
//-------------------------------------------------------------
//   start/shutdown ADC
//-------------------------------------------------------------
void startADC10(){
     ADC_CTL0 |= ENC + ADC10SC;  //   [ADC10 enable] + [Start Sample and Conversion]
}

void stopADC10(){
    ADC_CTL0 &= ~ADC10ON;
}

void UART_Receive(){
    UARTIntReg |= UARTRecIE;
}

void UART_Transmit(){
    UARTIntReg |= UARTTransIE;
//    if (state == state7){
//            IE2 |= UCA0TXIE;

}
void UART_menuReprint(){
    IE2 |= UCA0TXIE;
    //IE2 |= UCA0RXIE;
}
//-------------------------------------------------------------
//                      UART TX ISR
//-------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{

    if (X_flag){
        if (X_flag < 6){
            UART0TransBuf = '4'; // Send next Char
        }
        else{
            UART0TransBuf = 'E'; // Error - Char sent is not 0-9.
            X_flag = 0;
        }
    }
//    else if (ReceivedData == '7'){
//            UART0TransBuf = '7'; // Print Menu
//            ReceivedData = ' ';
//        }

    else if (state == state7){
            UART0TransBuf = LoveMy[Trans7Pointer++]; // Print I love my negev on terminal
    }

    else if (ReceivedData == '8'){
        UART0TransBuf = '8'; // Print Menu
    }
    else{
        UART0TransBuf = 'R'; // Received
    }
    if ((state != state7) || (Trans7Pointer == 17)){
        UARTIntReg &= ~UARTTransIE;
        LPM0_EXIT; // must be called from ISR only
    }
}

//-------------------------------------------------------------
//                      UART RX ISR
//-------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  ReceivedData = UART0RecBuf;
  PBsArrIntEn &= ~0x01;

  if (ReceivedData == '1' && !X_flag){
      state = state1;
      UART_Transmit();
  }
  else if (ReceivedData == '2' && !X_flag){
      state = state2;
      UART_Transmit();
  }
  else if (ReceivedData == '3' && !X_flag){
      state = state3;
      UART_Transmit();
  }
  else if (ReceivedData == '4' || X_flag){
      if(!X_flag){
          X_flag = 1;
          state = state0;
      }else get_X();
      UART_Transmit();
  }
  else if (ReceivedData == '5' && !X_flag){
      state = state5;
      UART_Transmit();
  }
  else if (ReceivedData == '6' && !X_flag){
      num = 0;
      UART_Transmit();
      lcd_clear();
  }
  else if (ReceivedData == '7' && !X_flag){
      PBsArrIntEn |= 0x01;
  }

  else if (ReceivedData == '8' && !X_flag){
      UART_menuReprint();
      state = state0;
  }

  else if (ReceivedData == '9' && !X_flag){
      UART_Transmit();
      state = state0;
  }

  else {
      X_flag = 404;
      state = state0;
  }

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

    if(PBsArrIntPend & PB0){
          state = state7;
      PBsArrIntPend &= ~PB0;
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

