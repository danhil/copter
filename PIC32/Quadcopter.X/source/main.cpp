/* 
 * File:   main.c
 * Author: Karl-Henrik Kihlgren
 *
 * Created on 29 januari 2014, 16:49
 */

#include <stdio.h>
#include <stdlib.h>

// PIC32MX250F128B Configuration Bit Settings

#include <xc.h>
#include <plib.h>

#include "mpu6050.h"

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
 *
 * Timer 2 will count up to 24000
 * Timer 2 Prescaler is 1
 * Interrupt frequency = Peripheral Clock / Prescaler / Nr of Counts = 24e6 / 8 / 24e3 = 1000 Hz
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

#define ISR_PERIOD          (24000)

#define PWM_PERIOD          (60000)
#define PWM_MIN_DC          (2000)      // Min duty cycle
#define PWM_MAX_DC          (7000)      // Max duty cycle
#define PWM_FACTOR          ( (PWM_MAX_DC - PWM_MIN_DC) / 100 )

// Converts the 0-100% signal into the correct value for the
// duty cycle register and sets that register.
#define Set_pwm( duty_cycle_in_percent ) ( OC1RS = PWM_MIN_DC + PWM_FACTOR * (duty_cycle_in_percent) )
#define Set_pwm2( duty_cycle_in_percent ) ( OC2RS = PWM_MIN_DC + PWM_FACTOR * (duty_cycle_in_percent) )
#define Set_pwm3( duty_cycle_in_percent ) ( OC3RS = PWM_MIN_DC + PWM_FACTOR * (duty_cycle_in_percent) )
#define Set_pwm4( duty_cycle_in_percent ) ( OC4RS = PWM_MIN_DC + PWM_FACTOR * (duty_cycle_in_percent) )


#define SYS_CLOCK (48000000L)
//#define GetSystemClock()            (SYS_CLOCK)
#define GetPeripheralClock()        (SYS_CLOCK/2)
//#define I2C_CLOCK_FREQ              400000

// EEPROM Constants
#define MPU6050_I2C_BUS             I2C1
#define MPU6050_ADDRESS             0x68        // 0b1101000 MPU6050 address
//#define EEPROM_ADDRESS            0x50        // 0b1010000 Serial EEPROM address

#define UARTBaudRate                9600


//BOOL StartTransfer( BOOL restart );
//BOOL TransmitOneByte( UINT8 data );
//void StopTransfer( void );

//UINT8 i2c_read( UINT8 regAddress );
//BOOL i2c_write(UINT8 regAddress, UINT8 data);

//void Serial_print(char *buffer);
//char* itoa( INT16 nr, char *string );

MPU6050 MPU6050dev;

char forward = 1;
char pwm_signal = 0;

unsigned int test_var = 0;


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
    //OpenOC1(OC_OFF | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, 0,0);
    //OpenOC2(OC_OFF | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, 0,0);
    //OpenOC3(OC_OFF | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, 0,0);
    //OpenOC4(OC_OFF | OC_TIMER_MODE16 | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, 0,0);


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
    //OpenTimer3(T3_ON | T3_PS_1_8, PWM_PERIOD);

    //OC1CONSET = 0x8000;             // Enable OC1
    //OC2CONSET = 0x8000;             // Enable OC2
    //OC3CONSET = 0x8000;             // Enable OC3
    //OC4CONSET = 0x8000;             // Enable OC4


     INTEnableSystemMultiVectoredInt(); // Enable system wide interrupt to multivectored mode.


    // Configure UART1
    //UARTConfigure(UART1, UART_ENABLE_PINS_TX_RX_ONLY);
    //UARTSetLineControl(UART1, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    //UARTSetLineControl(UART1, 0 | UART_PARITY_NONE | UART_STOP_BITS_1);

    //UARTSetDataRate(UART1, GetPeripheralClock(), UARTBaudRate);
    //UARTEnable(UART1, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

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

    //fastnar i setup-funktionen. fels�k denna funktion m.h.a PORTB bit2/3.
    // Verkar fungera nu...
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

        //kh_test = i2c_read(0x75);

        //mPORTBSetBits(BIT_3);




        /*
                if( kh_test != 0x68 )
                {
                    putsUART1( "Address is not 0x68  \0" );
                }
                else
                {
                    putsUART1( "BAddress is 0x68  \0" );
                }
         */
        /*i = 0;
        while(i < 55 )
        {
            putcUART1(filename[i]);      //Sends the Array over UART1
            i = i + 1;
        }*/

        //mPORTBToggleBits(BIT_2);




        //MPU6050dev.Get_Accel_Values();
        //MPU6050dev.Get_Accel_Angles();

        //MPU6050dev.Get_Gyro_Rates();

        //value = MPU6050dev.GetAngle();
        /*
                AccX = MPU6050dev.GetAccX();
                AccY = MPU6050dev.GetAccY();
                AccZ = MPU6050dev.GetAccZ();

                GyroX = MPU6050dev.GetGyroX();
                GyroY = MPU6050dev.GetGyroY();	// 177	// ska inte vara 177 l�ngre. Har �ndra skalningen s� att GyroY ska vara grader/s
                GyroZ = MPU6050dev.GetGyroZ();

                //AccY = ( -AccX + 800 ) / 177;		// 177 	// 93.6
                AccY = ( -AccX );

                AngleY = (0.98)*(AngleY + GyroY/117) + (0.02)*( (AccY * 90) + 5.7 );	// Loopen g�r i ca 117 Hz
         */
        //kh_test = MPU6050dev.MPU6050_Test_I2C();
        //sprintf(filename, "Angle: %f\n", AccX );
        //putsUART1( filename );

        //kh_test = MPU6050dev.MPU6050_Test_I2C();


        //AngleY = MPU6050dev.GetAngleY();

        //sprintf(filename, "Y Angle: %f\n", AngleY);
        //putsUART1(filename);


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


#ifdef __cplusplus
extern "C" {
#endif

// Timer2 ISR
//void __ISR(_TIMER_2_VECTOR, ipl7) T2_IntHandler (void)

void __ISR(_TIMER_2_VECTOR, ipl7) Timer2Handler(void)
{
    char filename[55]; //Array of 50 chars

    MPU6050dev.Get_Accel_Values();
    MPU6050dev.Get_Gyro_Rates();

    MPU6050dev.CalcAngleY();

    test_var++;

    if (test_var >= 1000)
    {
        test_var = 0;

        sprintf(filename, "Y Angle: %f\n", MPU6050dev.GetAngleY());
        putsUART1(filename);

        mPORTBToggleBits(BIT_2);
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
        Set_pwm(pwm_signal);
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
        Set_pwm(pwm_signal);
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

/*******************************************************************************
  Function:
    BOOL StartTransfer( BOOL restart )

  Summary:
    Starts (or restarts) a transfer to/from the EEPROM.

  Description:
    This routine starts (or restarts) a transfer to/from the EEPROM, waiting (in
    a blocking loop) until the start (or re-start) condition has completed.

  Precondition:
    The I2C module must have been initialized.

  Parameters:
    restart - If FALSE, send a "Start" condition
            - If TRUE, send a "Restart" condition
    
  Returns:
    TRUE    - If successful
    FALSE   - If a collision occured during Start signaling
    
  Example:
    <code>
    StartTransfer(FALSE);
    </code>
 
 *****************************************************************************/
/*
BOOL StartTransfer( BOOL restart )
{
    I2C_STATUS  status;
    UINT8 count = 0;

    // Send the Start (or Restart) signal
    if(restart)
    {
        I2CRepeatStart(MPU6050_I2C_BUS);
    }
    else
    {
        // Wait for the bus to be idle, then start the transfer
        //while( !I2CBusIsIdle(MPU6050_I2C_BUS) );

        // Checks if the bus is idle, and starts the transfer if so
        if( I2CBusIsIdle(MPU6050_I2C_BUS) )
        {
            if(I2CStart(MPU6050_I2C_BUS) != I2C_SUCCESS)
            {
                //DBPRINTF("Error: Bus collision during transfer Start\n");
                return FALSE;
            }
        }
        else
            return FALSE;
    }

    // Wait for the signal to complete or until time out
    do
    {
        status = I2CGetStatus(MPU6050_I2C_BUS);
        count++;

    } while ( !(status & I2C_START) && count < 200);

    if( count >= 200 )
        return FALSE;

    return TRUE;
}
 */

/*******************************************************************************
  Function:
    BOOL TransmitOneByte( UINT8 data )

  Summary:
    This transmits one byte to the EEPROM.

  Description:
    This transmits one byte to the EEPROM, and reports errors for any bus
    collisions.

  Precondition:
    The transfer must have been previously started.

  Parameters:
    data    - Data byte to transmit

  Returns:
    TRUE    - Data was sent successfully
    FALSE   - A bus collision occured

  Example:
    <code>
    TransmitOneByte(0xAA);
    </code>
  
 *****************************************************************************/
/*
BOOL TransmitOneByte( UINT8 data )
{
    UINT16 count = 0;

    // Wait for the transmitter to be ready
    while( !I2CTransmitterIsReady(MPU6050_I2C_BUS) && count < 64000 )
    {
        count++;
    }

    if( I2CTransmitterIsReady(MPU6050_I2C_BUS) )
    {
        // Transmit the byte
        if(I2CSendByte(MPU6050_I2C_BUS, data) == I2C_MASTER_BUS_COLLISION)
        {
            //DBPRINTF("Error: I2C Master Bus Collision\n");
            return FALSE;
        }

        // Wait for the transmission to finish
        while( !I2CTransmissionHasCompleted(MPU6050_I2C_BUS) && count < 5000 )
        {
            count++;
        }

        if( count >= 5000 )
            return FALSE;
    }
    else
        return FALSE;

    return TRUE;
}
 */

/*******************************************************************************
  Function:
    void StopTransfer( void )

  Summary:
    Stops a transfer to/from the EEPROM.

  Description:
    This routine Stops a transfer to/from the EEPROM, waiting (in a 
    blocking loop) until the Stop condition has completed.

  Precondition:
    The I2C module must have been initialized & a transfer started.

  Parameters:
    None.
    
  Returns:
    None.
    
  Example:
    <code>
    StopTransfer();
    </code>
 
 *****************************************************************************/
/*
void StopTransfer( void )
{
    I2C_STATUS  status;
    UINT8 count = 0;

    // Send the Stop signal
    I2CStop(MPU6050_I2C_BUS);

    // Wait for the signal to complete
    do
    {
        status = I2CGetStatus(MPU6050_I2C_BUS);
        count++;

    } while ( !(status & I2C_STOP) && count < 200);
}
 */
/*******************************************************************************
  Function:
    UINT8 i2c_read( UINT8 regAddress )

  Description:
    Reads a register in the i2c device.

  Precondition:
    The I2C module must have been initialized.

  Parameters:
    8-bit address for the register to read.

  Returns:
    Content of the register that was read.

 *****************************************************************************/
/*
UINT8 i2c_read( UINT8 regAddress )
{
    // Variable declarations
    UINT8               i2cData[10];
    I2C_7_BIT_ADDRESS   SlaveAddress;
    int                 Index;
    int                 DataSz;    
    BOOL                Acknowledged;
    BOOL                Success = TRUE;
    UINT8               i2cbyte;


    // Initialize the data buffer
    I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, MPU6050_ADDRESS, I2C_WRITE);
    i2cData[0] = SlaveAddress.byte;
    i2cData[1] = regAddress;                 // MPU6050 data address to read (0x75 = WHO_AM_I which contains 0x68)
    DataSz = 2;


    // Start the transfer
    if( !StartTransfer(FALSE) )
    {
        //while(1);
        Success = FALSE;
    }

    // Address the device.
    Index = 0;
    while( Success & (Index < DataSz) )
    {
        // Transmit a byte
        if (TransmitOneByte(i2cData[Index]))
            Index++;
        else
            Success = FALSE;

        // Verify that the byte was acknowledged
        if(!I2CByteWasAcknowledged(MPU6050_I2C_BUS))
        {
            //DBPRINTF("Error: Sent byte was not acknowledged\n");
            Success = FALSE;
        }
    }

    // Restart and send the device's internal address to switch to a read transfer
    if(Success)
    {
        // Send a Repeated Started condition
        if( !StartTransfer(TRUE) )
        {
            //while(1);
            Success = FALSE;
        }

        // Transmit the address with the READ bit set
        I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, MPU6050_ADDRESS, I2C_READ);
        if (TransmitOneByte(SlaveAddress.byte))
        {
            // Verify that the byte was acknowledged
            if(!I2CByteWasAcknowledged(MPU6050_I2C_BUS))
            {
                //DBPRINTF("Error: Sent byte was not acknowledged\n");
                Success = FALSE;
            }
        }
        else
        {
            Success = FALSE;
        }
    }

    i2cbyte = 9;

    // Read the data from the desired address
    if(Success)
    {
        if(I2CReceiverEnable(MPU6050_I2C_BUS, TRUE) == I2C_RECEIVE_OVERFLOW)
        {
            //DBPRINTF("Error: I2C Receive Overflow\n");
            Success = FALSE;
        }
        else
        {
            while(!I2CReceivedDataIsAvailable(MPU6050_I2C_BUS));
            i2cbyte = I2CGetByte(MPU6050_I2C_BUS);
        }
    }

    // End the transfer
    StopTransfer();
    if(!Success)
    {
        //while(1);

        //mPORTBSetBits(BIT_2);
        //mPORTBClearBits(BIT_3);
    }

    if( i2cbyte == 0x68 )
    {
        mPORTBSetBits(BIT_2);
        mPORTBSetBits(BIT_3);
    }
    if( i2cbyte == 9 )
    {
        //mPORTBSetBits(BIT_2);
        //mPORTBClearBits(BIT_3);
    }

    return i2cbyte;
}
 */
/*
BOOL i2c_write(UINT8 regAddress, UINT8 data)
{
    UINT8               i2cData[10];
    I2C_7_BIT_ADDRESS   SlaveAddress;
    int                 Index;
    int                 DataSz;
    BOOL                Acknowledged = FALSE;
    BOOL                Success = TRUE;
    
    // Initialize the data buffer
    I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, MPU6050_ADDRESS, I2C_WRITE);
    i2cData[0] = SlaveAddress.byte;
    i2cData[1] = regAddress;        // Register Address to write
    i2cData[3] = data;              // Data to write
    DataSz = 3;

    // Start the transfer
    if( !StartTransfer(FALSE) )
    {
        //while(1);
        Success = FALSE;
    }

    // Transmit all data
    Index = 0;
    while( Success && (Index < DataSz) )
    {
        // Transmit a byte
        if (TransmitOneByte(i2cData[Index]))
        {
            // Advance to the next byte
            Index++;

            // Verify that the byte was acknowledged
            if(!I2CByteWasAcknowledged(MPU6050_I2C_BUS))
            {
                //DBPRINTF("Error: Sent byte was not acknowledged\n");
                Success = FALSE;
            }
        }
        else
        {
            Success = FALSE;
        }
    }

    // End the transfer
    StopTransfer();
    
    // Wait for device to complete write process, by polling the ack status.    
    while (Acknowledged != TRUE && Success != FALSE);
    {
        // Start the transfer
        if( StartTransfer(FALSE) )
        {  
            // Transmit just the device's address
            if (TransmitOneByte(SlaveAddress.byte))
            {
                // Check to see if the byte was acknowledged
                Acknowledged = I2CByteWasAcknowledged(MPU6050_I2C_BUS);
            }
            else           
                Success = FALSE;            

            // End the transfer
            StopTransfer();
        }
        else
          Success = FALSE;
    }
    return Success;
}
 */
/*
void Serial_print(char *buffer)
{
   while(*buffer != (char)0)
   {
      while(!UARTTransmitterIsReady(UART1));
      UARTSendDataByte(UART1, *buffer++);
   }
    while(!UARTTransmissionHasCompleted(UART1));
   UARTSendDataByte(UART1, '\r');
   UARTSendDataByte(UART1, '\n');
}
 */