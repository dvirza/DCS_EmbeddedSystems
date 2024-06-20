#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer

char ST1str[6];
int Sec = 0;
int Min = 0;
char ST2str[6];


//*****************************************************************************
// STATE 1 FUNCTIONS
//*****************************************************************************

//--------------------------------------------------------------------
//            Prints Frequency measurement template to LCD
//--------------------------------------------------------------------
void LCD_State1(){
  lcd_clear();
  lcd_home();
  lcd_puts(LCD_state1_txt);
}


//--------------------------------------------------------------------
//            Measure The Input Signal Frequency
//--------------------------------------------------------------------
void FreqScan(){
  float FreqDiv;
  int SmpFreq;
  TA1start = 0;
  TA1end = 0;
  
  
  enable_TA1CCTL2();
  __bis_SR_register(LPM0_bits + GIE);
  asm("nop");
  CycleDiff = TA1end - TA1start;
  FreqDiv = SMCLK_FREQ/CycleDiff ; // div of signal T to the smclk T
  asm("nop");
  DelayMs(500);
  SmpFreq = (unsigned int)(FreqDiv );
  asm("nop");
  SampledFreq = SmpFreq;
}

//--------------------------------------------------------------------
//            Converts a >=5 digits integer into a string 'ST1str'
//--------------------------------------------------------------------

void int2String(int integer, char str[6]){
  int digit;
  int i;
  ST1str[5] = '\0';
  
  
  if (integer < 10){
    ST1str[0] = ' ';
    ST1str[1] = ' ';
    ST1str[2] = ' ';
    ST1str[3] = ' ';
    //adapted for
    for(i=1;i<2;i++){
      digit = integer % 10;
      ST1str[5-i] = '0' + digit;
      integer = integer / 10;
    }
    
  }
  
  else if (integer < 100){
    ST1str[0] = ' ';
    ST1str[1] = ' ';
    ST1str[2] = ' ';
    //adapted for
    for(i=1;i<3;i++){
      digit = integer % 10;
      ST1str[5-i] = '0' + digit;
      integer = integer / 10;
    }
     
  }
  
  
  else if (integer < 1000){
    ST1str[0] = ' '   ;
    ST1str[1] = ' '   ;
    //adapted for     ;
    for(i=1;i<4;i++){
      digit = integer % 10;
      ST1str[5-i] = '0' + digit;
      integer = integer / 10;
    }
    
  }
  
  
  else if (integer < 10000){
    ST1str[0] = ' ';
    //adapted for
    for(i=1;i<5;i++){
      digit = integer % 10;
      ST1str[5-i] = '0' + digit;
      integer = integer / 10;
    }
  }
  else {
    for(i=1;i<6;i++){
      digit = integer % 10;
      ST1str[5-i] = '0' + digit;
      integer = integer / 10;
  }
  
}
}

//--------------------------------------------------------------------
//            Updates the freq meas val on lcd screen
//--------------------------------------------------------------------

void refreshFreq(int freq,char * str){
  lcd_home();
  for(int i = 0;i<5;i++) lcd_cursor_right();  
  int2String(freq,ST1str); 
  lcd_puts(ST1str);
  //DelayMs(500);
}
//*****************************************************************************
// STATE 2 FUNCTIONS
//*****************************************************************************

void countBySwitch(){
  lcd_clear();
  lcd_home();
  int2StrClk();
  lcd_puts(ST2str);
  /*
  Timing Variables Invrement
  */
  if ((SWsArrPort & ~SWmask) == 0x01){
    Sec ++;
  }
  start_timerA0On();
  disable_TimerA0();
}

void int2StrClk(){
  ST2str[5] = '\0';
  ST2str[2] = ':';
  if (Sec == 60){
    Min = Min + 1 ;
    Sec = 0;
  }
    // Convert Values into the string:
  if (Sec < 10){
    ST2str[3] = '0';
    ST2str[4] = '0' + Sec;
  }
  else {
    ST2str[4] = '0' + (Sec % 10); 
    ST2str[3] = '0' + (Sec / 10);
  }
  if (Min < 10){
    ST2str[0] = '0';
    ST2str[1] = '0' + Min;
  }
  else {
    ST2str[1] = '0' + (Min % 10); 
    ST2str[0] = '0' + (Min / 10);
  }
}


//*****************************************************************************
// STATE 3 FUNCTIONS
//*****************************************************************************


void Buzz(void){
    unsigned int Nadc = 0;
    unsigned int res;
    unsigned int Fout;
    enable_ADC10(); //start sampling adc10
    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
    Nadc = GetMeas(); // Receive sampled value from adc10 // takes value from reg ADC10MEM
    Fout = CoefCalc(Nadc) + 1000; //1236
    res = SMCLK_FREQ/Fout;
    Ins2Regis4ADC(res);
}

unsigned int CoefCalc(unsigned int x){ // 1.466 = 1.0111(bin)
    unsigned int temp = x;
    unsigned int Coeff = 0;
    Coeff = temp;
    temp = temp >> 2; //SHR(tmp,2) 
    Coeff = Coeff+ temp;
    temp = temp >> 1; //SHR(tmp,1) 
    Coeff = Coeff + temp;
    temp = temp >> 1; //SHR(tmp,1) 
    Coeff = Coeff+ temp;
    return Coeff;
}





void SkippingB(){
  for(int i = 0; i < 32; i++){ 
    lcd_putchar('B');
    
    //Pause
    
    start_timerA0On();
    disable_TimerA0();
    
    lcd_cursor_left();
    lcd_putchar(' ');
    if (i == 15){
    lcd_new_line;
    }
  }
  
  
}