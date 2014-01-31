/* 
 * File:   main.c
 * Author: Karl-Henrik Kihlgren
 *
 * Created on den 29 januari 2014, 16:49
 */

#include <stdio.h>
#include <stdlib.h>

// PIC32MX250F128B Configuration Bit Settings

#include <xc.h>
#include <plib.h>

// DEVCFG3
// USERID = No Setting
#pragma config PMDL1WAY = OFF           // Peripheral Module Disable Configuration (Allow multiple reconfigurations)
#pragma config IOL1WAY = OFF            // Peripheral Pin Select Configuration (Allow multiple reconfigurations)
#pragma config FUSBIDIO = ON            // USB USID Selection (Controlled by the USB Module)
#pragma config FVBUSONIO = ON           // USB VBUS ON Selection (Controlled by USB Module)

// DEVCFG2
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_24         // PLL Multiplier (24x Multiplier)
#pragma config UPLLIDIV = DIV_5         // USB PLL Input Divider (5x Divider)
#pragma config UPLLEN = OFF             // USB PLL Enable (Disabled and Bypassed)
#pragma config FPLLODIV = DIV_2         // System PLL Output Clock Divider (PLL Divide by 2)

// DEVCFG1
#pragma config FNOSC = FRCPLL           // Oscillator Selection Bits (Fast RC Osc with PLL)
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = OFF               // Internal/External Switch Over (Disabled)
#pragma config POSCMOD = HS             // Primary Oscillator Configuration (HS osc mode)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_2           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/2)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1              // Watchdog Timer Postscaler (1:1)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable (Watchdog Timer is in Non-Window Mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))
#pragma config FWDTWINSZ = WISZ_25      // Watchdog Timer Window Size (Window Size is 25%)

// DEVCFG0
#pragma config JTAGEN = OFF             // JTAG Enable (JTAG Disabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)


/*

 * SYSCLK = 8 / FPLLIDIV * FPLLMULT / FPLLODIV = 8 / 2 * 24 / 2 = 48 MHz
 * 8 = 8 MHz = FRC (Internal Clock)


 * 48 Mhz -> 24 MHz Peripheral Clock
 * Timer 3 will count up to 60000
 * Timer 3 Prescaler is 8
 * PWM frequency = Peripheral Clock / Nr of Counts / Prescaler = 24e6 / 60e3 / 8 = 50 Hz
 *
 * The PWM period is 20ms and corresponds to 60000 in Timer 3. This leads to:
 * Min PWM duty cycle: 3000 (corresponds to 1ms pulse)
 * Max PWM duty cycle: 6000 (corresponds to 2ms pulse)
 *
 * However the servo Hitec HS422 requires a different range than 1-2 ms.
 * This is the settings that works IRL
 * Min PWM duty cycle: 2000 (maybe a little less )
 * Max PWM duty cycle: 7000 (maybe a little more )

 */

#define PWM_PERIOD          (60000)
#define PWM_MIN_DC          (2000)      // Min duty cycle
#define PWM_MAX_DC          (7000)      // Max duty cycle
#define PWM_FACTOR          ( (PWM_MAX_DC - PWM_MIN_DC) / 100 )

// Converts the 0-100% signal into the correct value for the
// duty cycle register and sets that rgeister.
#define Set_pwm( duty_cycle_in_percent ) ( OC1RS = PWM_MIN_DC + PWM_FACTOR * (duty_cycle_in_percent) )

char forward = 1;
char pwm_signal = 0;


/*
int main(int argc, char** argv)
{
    mPORTAClearBits(BIT_0);
    mPORTASetPinsDigitalOut(BIT_0);
    
    unsigned int count = 0;
    while(1)
    {
        mPORTAToggleBits(BIT_0);

        while(count)
        {
            count--;
        }
        count = 500000;
    }
    return (EXIT_SUCCESS);
}
*/

/*
int main(int argc, char** argv)
{
    mPORTAClearBits(BIT_0);
    mPORTASetPinsDigitalOut(BIT_0);

    //Calculate the required period
    //with a prescale of 1:64 we need 200 ms so count to 62,500 starting at 0

    OpenTimer1(T1_ON | T1_IDLE_CON | T1_PS_1_64 | T1_SOURCE_INT,  62500);
    while(1)
    {
        //Check the Timer Value Int Flag to check for a match

        if(mT1GetIntFlag())
        {
            //invert the RA0 port state
            mPORTAToggleBits( BIT_0);
            mT1ClearIntFlag();
        }
    }

    CloseTimer1();


    return (EXIT_SUCCESS);
}
*/

int main(int argc, char** argv)
{
    // Peripheral Pin Select
    SYSKEY = 0xAA996655;            // Write Key1 to SYSKEY
    SYSKEY = 0x556699AA;            // Write Key2 to SYSKEY

    //All Pin configurations should start here

    //RPB4Rbits.RPB4R=5;                   //Sets RPB4 as OC1. (pin 11)
    RPA0Rbits.RPA0R=5;

    //All Pin configuration should end here

    SYSKEY = 0;                                   // Locks the pin Configurations

    //End Peripheral Pin Select


    INTEnableSystemMultiVectoredInt();      // Enable system wide interrupt to
                                            // multivectored mode.


    // Configure the Output Compare channels for PWM mode using Timer3
    // setup output compare channel #1 - RD0
    OpenOC1(OC_OFF | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, 0,0);
    

    // Configure Timer3 interrupt. Note that in PWM mode, the
    // corresponding source timer interrupt flag is asserted.
    // OC interrupt is not generated in PWM mode.

    // Interrupt setup for Timer2 (should work for PIC32MX250)
    //IFS0CLR = 0x00000200;           // Clear the T2 interrupt flag
    //IEC0SET = 0x00000200;           // Enable T2 interrupt
    //IPC2SET = 0x0000001C;           // Set T2 interrupt priority to 7

    // Interrupt setup for Timer3
    IFS0CLR = 0x00004000;           // Clear the T3 interrupt flag
    IEC0SET = 0x00004000;           // Enable T3 interrupt
    IPC3SET = 0x0000001C;           // Set T3 interrupt priority to 7
    
    // Configure Timer3
    // Timer3 generates 20ms period for PWM
    OpenTimer3(T3_ON | T3_PS_1_8, PWM_PERIOD);

    OC1CONSET = 0x8000;             // Enable OC1


    


    //Set_pwm(50);
    
    //mPORTAClearBits(BIT_1);
    //mPORTASetPinsDigitalOut(BIT_1);

    unsigned int count = 0;
    while(1)
    {  
        // Doing nothing...

        while (count)
        {
            count--;
        }
        

        count = 4000000;
    }
    return (EXIT_SUCCESS);
}

// Example code for Timer3 ISR
void __ISR(_TIMER_3_VECTOR, ipl7) T3_IntHandler (void)
{
    // Insert user code here
    //mPORTAToggleBits(BIT_1);

    if (forward)
    {
        Set_pwm(pwm_signal++);
        if (pwm_signal >= 100)
        {
            forward = 0;
        }
    }
    else
    {
        Set_pwm(pwm_signal--);
        if (pwm_signal <= 0)
        {
            forward = 1;
        }
    }
    IFS0CLR = 0x4000;                   // Clearing Timer2 interrupt flag
}