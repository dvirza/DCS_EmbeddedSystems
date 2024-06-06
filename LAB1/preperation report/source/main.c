#include  "../header/api.h"            // private library - API layer
#include  "../header/app.h"            // private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;
int toggleCount; // count direction, 1 counts up and 0 counts down
int state1Timer; // counts time staying in state 1
int state2Timer; // counts time staying in state 2
int ledValS1; // saves the last leds count status on state1
int ledValS2; // saves the last leds count status on state2


void main(void){
  
  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();
  toggleCount = 1;
  state1Timer = 0;
  state2Timer = 0;
  ledValS1 = 0x0000;
  ledValS2 = 0x0001;
  
  
  while(1){
    switch(state){
      case state0:
        ClearLEDs();
                enterLPM(lpm_mode);
        break;
         
      case state1:
                DisablePBinterrupts(0xE0);// Enable only PB1 interrupts while it runs
                while (state1Timer < 20){ // Limits state1 time < 10s
                  if (toggleCount == 0){ // checks which direction to count
                   incLEDs(1);
                  }
          else {
                    incLEDs(-1);
                  }
          delay(LEDs_SHOW_RATE);    // delay of 500 [ms]
                  ledValS1 = LEDsArrPort; // saves count value for the next pb0 interrupt
                  state1Timer = state1Timer + 1;
                  }
                state = state0; //next state is state0
                EnablePBinterrupts(0xF0); // renable interrupts of al PBs
            break;
         
      case state2:
        disable_interrupts();
                state2Timer = 0;
                while (state2Timer < 14){ // loop that limits state 2 by 7 seconds
          ShiftLeftLED();         // SHL
                  delay(LEDs_SHOW_RATE);  // delay of 500 [ms]
                  ledValS2 = LEDsArrPort ;// Save the leds array value
                  state2Timer = state2Timer + 1;
                }
                state = state0; //next state is state0
                enable_interrupts();
        break;
                
        case state3:
          ClearLEDs();
              //creates pwm code 75% duty cycle
          while(state == state3)
            P20_PWM(75);
          break  ;
          
        case state4:
          state = state0;
          break;
    }
      }
}
