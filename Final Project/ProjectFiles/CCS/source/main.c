#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;


void main(void){
  
  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();
  lcd_init();
  lcd_clear();
  while(1){
      lcd_clear();
      switch(state){
	  case state0:
        RXType = 0;
        UART_Receive();
        enterLPM();
        break;
		 
	  case state1:
        Manual_Control();
        UART_Receive();
        break;
		 
	  case state2:
        JoyStickIE(); // Enable Joystick Interrupt
        Paint();
        JoyStickID();
        break;
                
	  case state3:
        count = 0; // End This Step
        JoyStickIE(); // Enable Joystick Interrupt
        Calibrate();
        JoyStickID();
        break;
            
	  case state4:
        //send pc the already loaded programs
	    //**** Send the mcu all the existing files by the order (file number, file name, file data)
	      ScriptMode();

		break;   

	}
  }
}
