/* 
 * File:   main.c
 * Author: Karl-Henrik Kihlgren
 *
 * Created on 29 januari 2014, 16:49
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <plib.h>

#include "config.h"
#include "mpu6050.h"

MPU6050 MPU6050dev;

char forward = 1;
short int pwm_signal = 0;

unsigned int test_var = 0;

int main(int argc, char** argv)
{
    // Peripheral Pin Select
    SYSKEY = 0xAA996655; // Write Key1 to SYSKEY
    SYSKEY = 0x556699AA; // Write Key2 to SYSKEY

    //All Pin configurations should start here 
    RPA0Rbits.RPA0R = 5; //Sets RPA0 (pin 2) as OC1 (PWM1)
    RPA1Rbits.RPA1R = 5; //Sets RPA1 (pin 3) as OC2 (PWM2)
    RPB0Rbits.RPB0R = 5; //Sets RPB0 (pin 4) as OC3 (PWM3)
    RPB1Rbits.RPB1R = 5; //Sets RPB1 (pin 5) as OC4 (PWM4)
    RPB4Rbits.RPB4R = 1; //Sets RPB4 (pin 11) as U1TX (UART TX)
    U1RXRbits.U1RXR = 2; //Sets RPA4 (pin 12) as U1RX (UART RX)
    //All Pin configuration should end here

    SYSKEY = 0; // Locks the pin Configurations

    //End of Peripheral Pin Select

    // Variable declarations
    UINT8 i2cData[10];
    I2C_7_BIT_ADDRESS SlaveAddress;
    int Index;
    int DataSz;
    UINT32 actualClock;
    BOOL Acknowledged;
    BOOL Success = TRUE;
    UINT8 i2cbyte;

    unsigned int count = 0; // loop variable

    //float AccX, AccY, AccZ;
    //float GyroX, GyroY, GyroZ;
    float AngleY = 0;

    char DebugStr[12];
    int i;
    char CR[2] = {13, '\0'};
    char LF[2] = {10, '\0'};

    UINT8 kh_test;
    char filename[55]; //Array of 50 chars

    // End of Variable declarations


    // Configure the Output Compare channels for PWM mode using Timer3
    // setup output compare channel #1 - RD0
    OpenOC1(OC_OFF | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, 0,0);
    OpenOC2(OC_OFF | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, 0,0);
    OpenOC3(OC_OFF | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, 0,0);
    OpenOC4(OC_OFF | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, 0,0);


    // Configure Timer3 interrupt. Note that in PWM mode, the
    // corresponding source timer interrupt flag is asserted.
    // OC interrupt is not generated in PWM mode.

    // Interrupt setup for Timer2 (should work for PIC32MX250)
    IFS0CLR = 0x00000200;           // Clear the T2 interrupt flag
    IEC0SET = 0x00000200;           // Enable T2 interrupt
    IPC2SET = 0x0000001C;           // Set T2 interrupt priority to 7

    // Interrupt setup for Timer3
    //IFS0CLR = 0x00004000;           // Clear the T3 interrupt flag
    //IEC0SET = 0x00004000;           // Enable T3 interrupt
    //IPC3SET = 0x0000001C;           // Set T3 interrupt priority to 7

    // Configure Timer2
    // Timer2 generates 1ms period
    // Timer2 should not start until i2c gyro device is initialized.
    OpenTimer2(T2_OFF | T2_PS_1_1, ISR_PERIOD);

    // Configure Timer3
    // Timer3 generates 20ms period for PWM
    OpenTimer3(T3_ON | T3_PS_1_8, PWM_PERIOD);

    OC1CONSET = 0x8000;             // Enable OC1
    OC2CONSET = 0x8000;             // Enable OC2
    OC3CONSET = 0x8000;             // Enable OC3
    OC4CONSET = 0x8000;             // Enable OC4


     INTEnableSystemMultiVectoredInt(); // Enable system wide interrupt to multivectored mode.
    

    //Initilize UART1
    OpenUART1(UART_EN | UART_BRGH_FOUR, UART_RX_ENABLE | UART_TX_ENABLE, GetPeripheralClock() / (4 * UARTBaudRate) - 1);
    //END UART1 Initialization 

    sprintf(filename, "Debug interface via UART/USB started.\n");
    putsUART1(filename);

    // Initialize 2 outputs used for debugging
    mPORTBClearBits(BIT_2);
    mPORTBSetPinsDigitalOut(BIT_2);

    mPORTBClearBits(BIT_3);
    mPORTBSetPinsDigitalOut(BIT_3);

    mPORTBClearBits(BIT_5);
    mPORTBSetPinsDigitalOut(BIT_5);

    // i2c communication is not working if this delay is removed. 
    // The loop time may very well be shortened (not tested).
    // The MPU6050 probably need some time to initialized after power on.
    while (count < 4000000) {
        ++count;
    }

    sprintf(filename, "Startup delay complete\n");
    putsUART1(filename);

    //MPU6050 MPU6050dev(MPU6050_I2C_BUS, MPU6050_ADDRESS);
    //MPU6050 MPU6050dev;

    kh_test = MPU6050dev.MPU6050_Test_I2C();

    sprintf(filename, "I2C address: %d\n", kh_test);
    putsUART1(filename);

    MPU6050dev.Setup_MPU6050();

    sprintf(filename, "\n\n\nMPU6050 Setup completed!\n\n\n");
    putsUART1(filename);

    MPU6050dev.Calibrate_Gyros();

    // i2c gyro device is now initialized and Timer2 can be started.
    OpenTimer2(T2_ON, ISR_PERIOD);

    count = 0;
    while (1) {
        // Doing nothing...

        while (count) {
            count--;
        }   

        /*while(U1STAbits.URXDA)
        {
            getsUART1(50, filename, 123);       //Receives up to 50 characters from UART 1 and stores it in ?filename? array
            U1STAbits.URXDA =0;                 //Clears the URXDA bit so the UART continues to receive
            U1STAbits.OERR = 0;                 //Clears the OERR bit so UART clears buffer and receives new characters
            putsUART1(filename);                //send the buffer, filename, over UART1
            putsUART1("Hej");
            memset(filename,0,50*sizeof(char)); //Clears the buffer
            mPORTBToggleBits(BIT_2);
        }*/

        count = 400000;
    }
    return (EXIT_SUCCESS);
}

#ifdef __cplusplus
extern "C" {
#endif

// Timer2 ISR
//void __ISR(_TIMER_2_VECTOR, ipl7) T2_IntHandler (void)

void __ISR(_TIMER_2_VECTOR, ipl7) Timer2Handler(void)
{
    char filename[55]; //Array of 50 chars

    MPU6050dev.Get_Accel_Values();
    MPU6050dev.Get_Accel_Angles();
    MPU6050dev.Get_Gyro_Rates();

    MPU6050dev.CalcAngleX();
    MPU6050dev.CalcAngleY();

    test_var++;

    mPORTBToggleBits(BIT_5);

    if (test_var >= 100)
    {
        test_var = 0;

        sprintf(filename, "Angle X: %f    Angle Y: %f\n", MPU6050dev.GetAngleX(), MPU6050dev.GetAngleY());
        //sprintf(filename, "Y Angle: %f\n", MPU6050dev.GetAngleY());
        //sprintf(filename, ": %f\n", MPU6050dev.GetAccX());

        putsUART1(filename);

       //mPORTBToggleBits(BIT_5);

       /*
       // Test code for PWM
       if (forward)
        {
            Set_pwm1(pwm_signal);
            Set_pwm2(pwm_signal);
            Set_pwm3(pwm_signal);
            Set_pwm4(pwm_signal);

            //pwm_signal++;
            pwm_signal = pwm_signal + 20;

            if (pwm_signal >= 1000) {
                forward = 0;
            }
        }
        else
        {
            Set_pwm1(pwm_signal);
            Set_pwm2(pwm_signal);
            Set_pwm3(pwm_signal);
            Set_pwm4(pwm_signal);

            //pwm_signal--;
            pwm_signal = pwm_signal - 20;

            if (pwm_signal <= 0) {
                forward = 1;
            }
        }
        */

    }

    //mT2ClearIntFlag();
    IFS0CLR = 0x0200; // Clearing Timer2 interrupt flag
}

#ifdef __cplusplus
}
#endif

/*
// Timer3 ISR
void __ISR(_TIMER_3_VECTOR, ipl7) T3_IntHandler(void)
{
    //mPORTAToggleBits(BIT_1);

    if (forward)
    {
        Set_pwm1(pwm_signal);
        Set_pwm2(pwm_signal);
        Set_pwm3(pwm_signal);
        Set_pwm4(pwm_signal);

        pwm_signal++;

        if (pwm_signal >= 100) {
            forward = 0;
        }
    }
    else
    {
        Set_pwm1(pwm_signal);
        Set_pwm2(pwm_signal);
        Set_pwm3(pwm_signal);
        Set_pwm4(pwm_signal);

        pwm_signal--;

        if (pwm_signal <= 0) {
            forward = 1;
        }
    }
  
    IFS0CLR = 0x4000; // Clearing Timer3 interrupt flag
}

*/

/*
char* itoa( INT16 nr, char *string )
{	
    //char string[6];
    INT16 nr2;
	
    char index = 0;
    char size = 0;

    if (nr == 0) // deals with the special case of convertng a 0.
    {
        string[ index++ ] = '0';
    }
    else if (nr < 0) // if negative number, add negative sign in string and remove negative sign in the number
    {
        string[ index++ ] = '-';
        nr = -nr;
    }

    nr2 = nr;

    while (nr2 > 0)
    {
        nr2 /= 10;
        size++;
    }

    string[ index + size ] = '\0';
    index = size - 1;

    while (size > 0)
    {
        string[ index-- ] = nr % 10 + 48;
        nr /= 10;
        size--;
    }

    string[ index ] = '\0';

    return string;
}
 */