#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer

//-------------------------------------------------------------
//            Print SWs value onto LEDs
//-------------------------------------------------------------
void printSWs2LEDs(void){
	unsigned char ch;
	
	ch = readSWs();
	print2LEDs(ch);
}
//--------------------------------------------------------------------
//            Print array to LEDs array with rate of LEDs_SHOW_RATE
//--------------------------------------------------------------------            
void printArr2SWs(char Arr[], int size, unsigned int rate){
	unsigned int i;
	
	for(i=0; i<size; i++){
		print2LEDs(Arr[i]);
		delay(rate);
	}
}


void DutyCycleHandler(){

  unsigned char DutyCycle = readSWs();

  if (DutyCycle & 0x10){ // sw 0 0% DutyCycle
      T0 = 0xFFFF; //πλπρ
      T1 = 0x0000;
      P27_PWM(T1,T0);
  }
  else if (DutyCycle & 0x20){ // sw 1 25% Duty Cycle
        T0= 0x0021; //πλπρ
        T1= 0x0006;
        P27_PWM(T1,T0);
  }
  else if (DutyCycle & 0x30){ // sw2 50%
        T0= 0x0013;
        T1= 0x0014;
        P27_PWM(T1,T0);
  }
  else if (DutyCycle & 0x40){ // sw 3 - 75%DutyCycle - 2KHz
       T0= 0x0006;
       T1= 0x0021;
       P27_PWM(T1,T0);
}
  else if (DutyCycle & 0x50){ // sw 5 - 100%DutyCycle - 2KHz
            T0 = 0x0000;
            T1 = 0xFFFF;
            P27_PWM(T1,T0);
  }
  else { 
   
  }
}

 
  

