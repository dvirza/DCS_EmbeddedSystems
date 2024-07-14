#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include <stdlib.h>
#include "stdio.h"
int Trans7Pointer = 0;

char ST1str[5];
//-------------------------------------------------------------
//                    convert int to string
//-------------------------------------------------------------
void to_string(char *str, unsigned int num){
    long tmp = num, len = 0;
    int j;
    while(tmp){
        len++;
        tmp /= 10;
    }
    for(j = len - 1; j >= 0; j--){
        str[j] = (num % 10) + '0';
        num /= 10;
    }
    str[len] = '\0';
}



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


//-------------------------------------------------------------
//                       Count up to LCD
//-------------------------------------------------------------
void count(){ /// counting weird
    char temp[6];
    while(state == state2){
        to_string(temp, num++); //count up
        lcd_home();
        lcd_puts(temp);
        startTimerA0();
        enterLPM(mode0);
        stopTimerA0();
    }
}

//-------------------------------------------------------------
//                         shift buzzer freq
//-------------------------------------------------------------
void buzzer(){
    int tone_series[7] = {1048, 838, 700, 600, 524, 466, 420};
    unsigned int i = 0;
    startTimerA1(); // Config
    enableTimerA1_bzr(); // OUTMOD reset/set
    while(state == state3){
        Timer1_CCR0 = tone_series[i];
        Timer1_CCR2 = tone_series[i++] >> 1;
        if(i == 7){
            i = 0;
        }
        startTimerA0();
        enterLPM(mode0);
        stopTimerA0();

    }
    disableTimerA1_bzr();
    stopTimerA1();
}

//-------------------------------------------------------------
//                         set X value
//-------------------------------------------------------------
void set_X(){
    stopTimerA0();
    X = atoi(timerInput); //    string->integer
    X = (X*33) >> 5; // = X * 1.024
    if(X >= 1024){
        X = 0xFFFF;
    }
    else {
        X = X << 6;
    }
    TIMER0_A0_config();
}

//-------------------------------------------------------------
//                      Measure LDR Value
//-------------------------------------------------------------
void measADC10(){
    int LDR_meas;
    char num[6];
    int i;

    while(state == state5){
        i = 0;
        startADC10();                           // Enable
        enterLPM(mode0);                        // Enter LPM
        stopADC10();
        LDR_meas = ( 52 * ADC_MEM ) >> 4;       // = sample_value * 3.3
        to_string(num, LDR_meas);               // int val measured -> str 'num'
        lcd_home();
        lcd_puts("ADC10 Meas:");
        lcd_new_line;
        if(LDR_meas <= 999){
            lcd_putchar('0');                   // if meas val < 1 print 0 o
        }else{
            lcd_putchar(num[i++]);
        }
        lcd_putchar('.');
        if(LDR_meas <= 99){
            lcd_putchar('0');
        }else{
            lcd_putchar(num[i++]);
        }
        if(LDR_meas <= 9){
            lcd_putchar('0');
        }else{
            lcd_putchar(num[i++]);
        }
        lcd_putchar(num[i++]);
        lcd_puts(" [v]");

        startTimerA0();
        enterLPM(mode0);
        stopTimerA0();
    }
}

void ShowLove(){
    Trans7Pointer = 0;
    UART_Transmit();
    enterLPM(lpm_mode);

    /*while (Trans7Pointer < 17){
        ();
        Trans7Pointer++;
    }*/
    UART_Receive();
    state = state0;

}
