/* 
 * File:   config.h
 * Author: Karl-Henrik Kihlgren
 *
 * Created on 16 dec 2016
 */

//#include <stdio.h>
//#include <stdlib.h>

// PIC32MX250F128B Configuration Bit Settings

#include <xc.h>
#include <plib.h>

//#include "mpu6050.h"

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
#pragma config ICESEL = ICS_PGx2        // ICE/ICD Comm Channel Select (Communicate on PGEC2/PGED2)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)


/*

 * SYSCLK = 8 / FPLLIDIV * FPLLMULT / FPLLODIV = 8 / 2 * 24 / 2 = 48 MHz
 * 8 = 8 MHz = FRC (Internal Clock)
 * 48 Mhz -> 24 MHz Peripheral Clock
 *
 * Timer 2 will count up to 24000
 * Timer 2 Prescaler is 1
 * Interrupt frequency = Peripheral Clock / Prescaler / Nr of Counts = 24e6 / 1 / 24e3 = 1000 Hz
 *
 * * Timer 2 will count up to 48000
 * Timer 2 Prescaler is 1
 * Interrupt frequency = Peripheral Clock / Prescaler / Nr of Counts = 48e6 / 1 / 24e3 = 500 Hz
 *
 * Timer 3 will count up to 60000
 * Timer 3 Prescaler is 8
 * PWM frequency = Peripheral Clock / Prescaler / Nr of Counts = 24e6 / 8 / 60e3 = 50 Hz
 *
 * The PWM period is 20ms and corresponds to 60000 in Timer 3. This leads to:
 * Min PWM duty cycle: 3000 (corresponds to 1ms pulse)
 * Max PWM duty cycle: 6000 (corresponds to 2ms pulse)
 *
 * However the servo Hitec HS422 requires a different range than 1-2 ms.
 * This is the settings that works IRL
 * Min PWM duty cycle: 2000 (maybe a little less )
 * Max PWM duty cycle: 7000 (maybe a little more )

 *
 // If 20 MHz oscillator should be used
 * SYSCLK = 20 / FPLLIDIV * FPLLMULT / FPLLODIV = 20 / 4 * 20 / 2 = 50 MHz
 * 20 = 20 MHz = Extern clock
 
 * 50 Mhz -> 25 MHz Peripheral Clock
 * Timer 3 will count up to 62500
 * Timer 3 Prescaler is 8
 * PWM frequency = Peripheral Clock / Nr of Counts / Prescaler = 25e6 / 62,5e3 / 8 = 50 Hz
  
 */

//#define ISR_PERIOD          (24000)
#define ISR_PERIOD          (48000)

#define PWM_PERIOD          (60000)
#define PWM_MIN_DC          (2000)      // Min duty cycle
#define PWM_MAX_DC          (7000)      // Max duty cycle
#define PWM_FACTOR          ( (PWM_MAX_DC - PWM_MIN_DC) / 1000 )

// Converts the 0-1000 permille signal into the correct value for the
// duty cycle register and sets that register.
#define Set_pwm1( duty_cycle_in_permille ) ( OC1RS = PWM_MIN_DC + PWM_FACTOR * (duty_cycle_in_permille) )
#define Set_pwm2( duty_cycle_in_permille ) ( OC2RS = PWM_MIN_DC + PWM_FACTOR * (duty_cycle_in_permille) )
#define Set_pwm3( duty_cycle_in_permille ) ( OC3RS = PWM_MIN_DC + PWM_FACTOR * (duty_cycle_in_permille) )
#define Set_pwm4( duty_cycle_in_permille ) ( OC4RS = PWM_MIN_DC + PWM_FACTOR * (duty_cycle_in_permille) )


#define SYS_CLOCK (48000000L)
//#define GetSystemClock()            (SYS_CLOCK)
#define GetPeripheralClock()        (SYS_CLOCK/2)
//#define I2C_CLOCK_FREQ              400000

// EEPROM Constants
#define MPU6050_I2C_BUS             I2C1
#define MPU6050_ADDRESS             0x68        // 0b1101000 MPU6050 address
//#define EEPROM_ADDRESS            0x50        // 0b1010000 Serial EEPROM address

#define UARTBaudRate                9600
