#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include <stdlib.h>
#include "stdio.h"



unsigned int FullRoundSteps = 2050; /// 2050
unsigned int count = 0;
unsigned int TargetPos;
unsigned int d_delay = 100;





void Int2String(int val , char * buffer)
{
    buffer[0] = (val / 100) + '0';
    val = val % 100;
    if (buffer[0] == '0')
        buffer[0] = ' ';
    buffer[1] = (val/10) + '0';
    if ((buffer[1] == '0') && (buffer[0] == ' '))
            buffer[1] = ' ';
    val = val % 10;
    buffer[2] = val + '0';
    buffer[3] = '\0';
}


//-------------------------------------------------------------
//                 STATE1:     Manual Step Motor Control by Joystick
//-------------------------------------------------------------


void Manual_Control(){
    GetJSvals(&Xcenter,&Ycenter); // update the values of X and Y axis measured by ADC10 when JS is centralized
    int JS_Xval;
    int JS_Yval;
    int Xscaled;
    int Yscaled;


    while (state==state1){
        GetJSvals(&JS_Xval,&JS_Yval);
        Xscaled = JS_Xval - Xcenter;
        Yscaled = JS_Yval - Ycenter;
        getReqPos(&Xscaled,&Yscaled);
        if (abs(TargetPos - currStep) >= 16)
            rotateMotor();
    }
    TargetPos = FullRoundSteps;
    rotateMotor();
}




void getReqPos(const int * x,const int * y){
    float atan; // y/x
    if (abs(*x) < 16 && abs(*y) < 16) // Stick is in the middle?
        TargetPos = currStep;
    else if (*x >= 0 && *y >= 0){ // Q: IV [270,360]
        atan = (float)(*y) / (float)(*x);
        if (atan >= 3)
            TargetPos = FullRoundSteps ; //360
        else if (atan >= 1.5)
            TargetPos = (FullRoundSteps >> 4) * 15; //337.5
        else if (atan >= 0.8)
            TargetPos = (FullRoundSteps >> 3) * 7; //315
        else if (atan >= 0.2)
            TargetPos = (FullRoundSteps >> 4) * 13; //292.5
        else
            TargetPos = (FullRoundSteps >> 2) * 3; // 270

    }else if (*x >= 0 && *y <= 0){ // Q: II [180,270] ?
        atan = (float)(abs(*y)) / (float)(*x);
        if (atan <= 0.2)
            TargetPos = (FullRoundSteps >> 2) * 3; // 270
        else if (atan <= 0.8)
            TargetPos = (FullRoundSteps >> 4) * 11; // 247.5
        else if (atan <= 1.5)
            TargetPos = (FullRoundSteps >> 3) * 5; // 225
        else if (atan <= 3)
            TargetPos = (FullRoundSteps >> 4) * 9; // 202.5
        else
            TargetPos = FullRoundSteps >> 1; // 180

    }else if (*x <= 0 && *y <= 0){ // Q: III [90,180] ?
        atan = (float)(abs(*y)) / (float)(abs(*x));
        if (atan >= 3)
            TargetPos = FullRoundSteps >> 1; //180
        else if (atan >= 1.5)
            TargetPos = (FullRoundSteps >> 4) * 7; // 157.5
        else if (atan >= 0.8)
            TargetPos = (FullRoundSteps >> 3) * 3; // 135
        else if (atan >= 0.2)
            TargetPos = (FullRoundSteps >> 4) * 5; //112.5
        else
            TargetPos = (FullRoundSteps >> 2) ; // 90


    }else if (*x < 0 && *y >= 0){ // Q: IV [0,90] ?
        atan = (float)(*y) / (float)(abs(*x));
        if (atan <= 0.2)
            TargetPos = (FullRoundSteps >> 2); // 90
        else if (atan <= 0.8)
            TargetPos = (FullRoundSteps >> 4) * 3; // 67.5
        else if (atan <= 1.5)
            TargetPos = (FullRoundSteps >> 3) * 1; // 45
        else if (atan <= 3)
            TargetPos = (FullRoundSteps >> 4); // 22.5
        else
            TargetPos = 0; // 180

    }
}


void rotateMotor(){
    //compare target pos to current pos and move in 'while' loop
    // Check if clockwise or counter clockwise
    unsigned int MPos = 1;
    if ((TargetPos - currStep + FullRoundSteps) % FullRoundSteps < (currStep - TargetPos + FullRoundSteps) % FullRoundSteps){
         //Go CW
        while (currStep != TargetPos)
        {
            PauseDuration(3); // Timer A0 Pause in milisec
            MotorCWStep(&MPos);
        }
    }
    else
    {
        //Go CCW
        while (currStep != TargetPos){
            PauseDuration(3); // Timer A0 Pause in milisec
            MotorCCWStep(&MPos);
        }
    }

}


//-------------------------------------------------------------
//                 STATE2:     Painter Controlled by Joystick
//-------------------------------------------------------------
void Paint(void){
    int JS_Xval;
    int JS_Yval;

    while (state == state2){
        GetJSvals(&JS_Xval,&JS_Yval);
        //TXsync = 1;
        if (TXcmd)
        {
            PauseDuration(1000);
            PauseDuration(1000);
            PauseDuration(1000);

            UART_TransmitCmd(ToolToggleCommand);
            TXcmd = 0;
        }

        UART_TransmitInt(&JS_Xval);
        UART_TransmitInt(&JS_Yval);
    }
}

//-------------------------------------------------------------
//                 STATE3:     Step Motor Calibration
//-------------------------------------------------------------
void Calibrate(){
    unsigned int MotorPos = 1;
    //GetJSvals(&Xcenter,&Ycenter); // update the values of X and Y axis measured by ADC10 when JS is centralized
    //UART_TransmitInt(&Xcenter);
    //UART_TransmitInt(&Ycenter);

    while (state == 3){
        if (CalibToggle == 2){
            UART_TransmitInt(&count);
            count = 0;
            state = state0;
        }

        stepCount(&MotorPos);
    }
}

void stepCount(unsigned int *MPos){
    PauseDuration(10); // Timer A0 Pause in milisec
    MotorCWStep(MPos);
    count++;
}

void PauseDuration(unsigned int MS){
    Timer0_CCR0 = MS << 7; // SMCLK/8 approx= 2^7 * 10^3
    startTimerA0();
}

//-------------------------------------------------------------
//                 STATE4:     Flash Script Mode
//-------------------------------------------------------------

void SendFlashFiles(void){
    TXFile = 1;
    UART_Transmit();
    UART_Receive();
}



// Function to increment the LCD from 0 to x
void inc_lcd(int x) {
    int i;
    lcd_clear();
    for (i = 0; i <= x; i++) {
        if (i > 99)
        {
            lcd_putchar((i / 100) + '0');
            lcd_putchar((i / 10) + '0');
        }
        else if (i > 9)
        {
            lcd_putchar((i / 10) + '0');
        }
        lcd_putchar(i + '0');
        PauseDuration(d_delay * 10);
        lcd_clear();
    }
}

// Function to decrement the LCD from x to 0
void dec_lcd(int x) {
    int i = x;
    lcd_clear();
    for (i = 0; i >= 0; i--) {
        if (i > 99)
        {
            lcd_putchar((i / 100) + '0');
            lcd_putchar((i / 10) + '0');
        }
        else if (i > 9)
        {
            lcd_putchar((i / 10) + '0');
        }
        lcd_putchar(i + '0');
        PauseDuration(d_delay * 10);
        lcd_clear();
    }
}

// Function to rotate a character on the LCD screen
void rra_lcd(char x) {
    //printf("Rotating right on LCD to index %d...\n", x);
    lcd_clear();
    lcd_home();
    int i;
    for (i = 0; i <= 32; i++) {
        lcd_putchar(x);
        PauseDuration(d_delay);
        lcd_cursor_left();
        lcd_putchar(' ');
        if (i == 15)
            lcd_new_line;
    }
    lcd_clear();
}

// Function to set a delay in the system
void set_delay(int d) {
    char d_string[4];
    Int2String(d , &d_string);
    d_delay = d;
    lcd_clear();
    lcd_puts("New delay value");
    lcd_new_line;
    lcd_cursor_right();
    lcd_cursor_right();
    lcd_puts("d = ");
    lcd_puts(d_string);
    PauseDuration(1000);
    lcd_clear();
}

// Function to clear all LEDs
void clear_all_leds(void) {
    lcd_clear();
}

// Function to set the stepper motor pointer to a specific degree

// Function to scan the stepper motor between two angles
void stepper_scan(int l, int r)
{
    unsigned int MPos = 1;
    lcd_puts("Step-Motor Scan");
    PauseDuration(2000);
    char l_str[4] , r_str[4];

    Int2String(l , l_str);
    Int2String(r , r_str);
    lcd_new_line;
    lcd_puts("l = ");
    lcd_puts(l_str);
    lcd_puts(", r = ");
    lcd_puts(r_str);

    TargetPos = (unsigned int)(l * (FullRoundSteps/360));
    while (currStep != TargetPos)
    {
        PauseDuration(3); // Timer A0 Pause in milisec
        MotorCWStep(&MPos);
    }
    TargetPos = (unsigned int)(r * (FullRoundSteps/360));
    while (currStep != TargetPos)
    {
        PauseDuration(30); // Timer A0 Pause in milisec
        MotorCWStep(&MPos);
    }
}

// Function to set the MCU into sleep mode
void sleep(void) {
    enterLPM();
}


void stepper_deg(int arg_p)
{
    //compare target pos to current pos and move in 'while' loop
    // Check if clockwise or counter clockwise
    TargetPos = (arg_p) * (FullRoundSteps / 360 );
    unsigned int MPos = 1;
    UART_TransmitCmd(ShowDynamicAngle);
    if ((TargetPos - currStep + FullRoundSteps) % FullRoundSteps < (currStep - TargetPos + FullRoundSteps) % FullRoundSteps)
    {
         //Go CW
        while (currStep != TargetPos)
        {
            PauseDuration(20); // Timer A0 Pause in milisec
            MotorCWStep(&MPos);
            if (!(currStep % 15))
            {
                unsigned int Deg = ((currStep) * FullRoundSteps) / 360;
                UART_TransmitInt((int *)&Deg);
            }
        }
    }
    else
    {
        //Go CCW
        while (currStep != TargetPos)
        {
            PauseDuration(3); // Timer A0 Pause in milisec
            MotorCCWStep(&MPos);
            if (!(currStep % 15))
            {
                unsigned int Deg = ((currStep) * FullRoundSteps) / 360;
                UART_TransmitInt((int *)&Deg);
            }

        }
    }
    //send pc stop show int cmd
    UART_TransmitCmd(ShowDynamicAngle);

}

int DecodeOperand(uint8_t* Operand)
{
    int val = 0;
    if ((Operand[0] >= 'A') && (Operand[0] <= 'F'))
        val += (Operand[0] + 1 - 'A') * 16;
    else
        val += (Operand[0] - 48) * 16;

    if ((Operand[1] >= 'A') && (Operand[1] <= 'F'))
        val += Operand[1] - 'A' + 10;
    else
        val += (Operand[1] - '0');
    return val;
}

// Function to decode and execute the program based on the program number
void decode_and_execute(unsigned int program_number) {
    // Array of pointers to the start of each program in flash memory
    //uint8_t *programs[] = {FLASH_FILE0_DATA, FLASH_FILE1_DATA, FLASH_FILE2_DATA};

    // Pointer to the start of the selected program
    uint8_t *data = FLASH_FILE0_DATA + (program_number * FLASH_SEGMENT_SIZE);
    char currOpcode;
    int i = 1;
    // Loop through the program data until encountering a newline character
    // Loop through the program data until encountering a newline character
    while (data[i] != '\n') {
        currOpcode = data[i];  // Assign the current opcode
        switch (currOpcode) {
            case '1':  // inc_lcd
                inc_lcd(DecodeOperand(&data[i + 1]));
                i += 3;
                break;
            case '2':  // dec_lcd
                dec_lcd(DecodeOperand(&data[i + 1]));
                i += 3;
                break;
            case '3':  // rra_lcd
                rra_lcd(DecodeOperand(&data[i + 1]));
                i += 3;
                break;
            case '4':  // set_delay
                set_delay(DecodeOperand(&data[i + 1]));
                i += 3;
                break;
            case '5':  // clear_all_leds
                clear_all_leds();
                i += 2;
                break;
            case '6':  // stepper_deg
                stepper_deg(DecodeOperand(&data[i + 1]));
                i += 3;
                break;
            case '7':  // stepper_scan
                stepper_scan(DecodeOperand(&data[i + 1]), DecodeOperand(&data[i + 3]));
                i += 5;
                break;
            case '8':  // sleep
                sleep();
                i += 1;
                break;
            default:
                i += 1;  // Skip unknown commands
                break;
        }
    }
    lcd_clear();
    if (currStep != FullRoundSteps) // if its not, return motor to its natural position (point up)
    {
        TargetPos = FullRoundSteps;
        rotateMotor();
    }


    RXExecute = 0;
}

void ScriptMode(void){
    while (state == state4)
    {
    SendFlashFiles();
    UART_Receive();
    enterLPM();
    if(TXAck)
        UART_Transmit();
    if(RXExecute)
        decode_and_execute(FileNumber);
    }
}

