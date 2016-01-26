#include "mpu6050.h"

// used for debugging via UART
char filename[80];

/*
   Rasperry Pi + MPU6050
   _______________________
   |:::::::               |
   |      __              |
   |     |  |             |
   |     |  |             |
   |     |__|             |
   |                      |
   |______________________|

   Acc X + \/
   Acc Y + ->
   Acc Z + down

   Gyro X + ->
   Gyro Y + /\
   Gyro Z + counterclockwise


*/

/*
int mSleep( int milliseconds )
{
    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = 1000000 * milliseconds;

    if(nanosleep(&tim , &tim2) < 0 )
    {
        printf("Nano sleep system call failed \n");
        return -1;
    }

    return 0;
}
*/
/*****************************************************************
 * This is the default constructor for the class. It assigns
 * all private variables to default values and calls the openI2C()
 * function to open the default I2C device "/dev/i2c-1".
 *****************************************************************/
MPU6050::MPU6050(void)
{
    // Full scale range of 500 º/s => 65.5 LSB/(º/s)
    // See MPU-6050 Product Specification page 12 for more information.

    GYRO_XSCALE = 65.5;
    GYRO_YSCALE = 65.5;
    GYRO_ZSCALE = 65.5;

    ACCEL_XSCALE = 16384;
    ACCEL_YSCALE = 16384;
    ACCEL_ZSCALE = 16384;

    XANGLE = 0.0;
    YANGLE = 0.0;
    ZANGLE = 0.0;

    // Set the I2C interface used
    this->i2cBusId = I2C1;
    // Set the device address
    this->deviceAddress = 0x68;

    //cout << " Opening I2C Device" << endl;

    // TODO: Fault handling! What to do if function fails?
    I2CInit();
}

/*******************************************************************
 * This is the overloaded constructor. It allows the programmer to
 * specify a custom I2C device & device address
 * The device descriptor is determined by the openI2C() private member
 * function call.
 * *****************************************************************/
MPU6050::MPU6050( I2C_MODULE new_i2cBusId, UINT8 new_devAddress )
{
    // Full scale range of 500 º/s => 65.5 LSB/(º/s)
    // See MPU-6050 Product Specification page 12 for more information.

    GYRO_XSCALE = 65.5;
    GYRO_YSCALE = 65.5;
    GYRO_ZSCALE = 65.5;

    ACCEL_XSCALE = 16384;
    ACCEL_YSCALE = 16384;
    ACCEL_ZSCALE = 16384;

    XANGLE = 0.0;
    YANGLE = 0.0;
    ZANGLE = 0.0;

    // Set the I2C interface used
    //this->i2cBusId = new_i2cBusId;
    this->i2cBusId = new_i2cBusId;
    // Set the device address
    //this->deviceAddress = new_devAddress;
    this->deviceAddress = new_devAddress;

    //cout << " Opening I2C Device" << endl;    
    
    // TODO: Fault handling! What to do if function fails?
    I2CInit();
}

/**********************************************************************
 * This is the class destructor it simply closes the open I2C device
 * by calling the closeI2C() which in turn calls the close() system call
 * *********************************************************************/
MPU6050::~MPU6050(void)
{
    //cout << " Closing I2C Device" << endl;
    //this->closeI2C();
}

char MPU6050::MPU6050_Test_I2C()
{
    UINT8 Data = 0x00;

    readReg( MPU6050_RA_WHO_AM_I, Data );

    if(Data == 0x68)
    {
        //cout << " I2C Read Test Passed, MPU6050 Address: 0x" << hex << (int)Data << endl;
    }
    else
    {
        //cout << " ERROR: I2C Read Test Failed. Data received: 0x" << hex << (int)Data << endl;
    }
    return Data;
}

void MPU6050::Setup_MPU6050()
{
    sprintf(filename, "Starting Setup_MPU6050().\n");
    putsUART1( filename );

    //mPORTBSetBits(BIT_2);

    //Sets sample rate to 8000/1+7 = 1000Hz
    writeReg(MPU6050_RA_SMPLRT_DIV, 0x07);
    //Disable FSync, 256Hz DLPF

    
    writeReg(MPU6050_RA_CONFIG, 0x00);
    //Disable gyro self tests, scale of 500 degrees/s
    writeReg(MPU6050_RA_GYRO_CONFIG, 0b00001000);
    //Disable accel self tests, scale of +-2g, no DHPF
    writeReg(MPU6050_RA_ACCEL_CONFIG, 0x00);
    //Freefall threshold of |0mg|
    writeReg(MPU6050_RA_FF_THR, 0x00);
    //Freefall duration limit of 0
    writeReg(MPU6050_RA_FF_DUR, 0x00);
    //Motion threshold of 0mg
    writeReg(MPU6050_RA_MOT_THR, 0x00);
    //Motion duration of 0s
    writeReg(MPU6050_RA_MOT_DUR, 0x00);
    //Zero motion threshold
    writeReg(MPU6050_RA_ZRMOT_THR, 0x00);
    //Zero motion duration threshold
    writeReg(MPU6050_RA_ZRMOT_DUR, 0x00);
    //Disable sensor output to FIFO buffer
    writeReg(MPU6050_RA_FIFO_EN, 0x00);

    //AUX I2C setup
    //Sets AUX I2C to single master control, plus other config
    writeReg(MPU6050_RA_I2C_MST_CTRL, 0x00);
    //Setup AUX I2C slaves
    writeReg(MPU6050_RA_I2C_SLV0_ADDR, 0x00);
    writeReg(MPU6050_RA_I2C_SLV0_REG, 0x00);
    writeReg(MPU6050_RA_I2C_SLV0_CTRL, 0x00);
    writeReg(MPU6050_RA_I2C_SLV1_ADDR, 0x00);
    writeReg(MPU6050_RA_I2C_SLV1_REG, 0x00);
    writeReg(MPU6050_RA_I2C_SLV1_CTRL, 0x00);
    writeReg(MPU6050_RA_I2C_SLV2_ADDR, 0x00);
    writeReg(MPU6050_RA_I2C_SLV2_REG, 0x00);
    writeReg(MPU6050_RA_I2C_SLV2_CTRL, 0x00);
    writeReg(MPU6050_RA_I2C_SLV3_ADDR, 0x00);
    writeReg(MPU6050_RA_I2C_SLV3_REG, 0x00);
    writeReg(MPU6050_RA_I2C_SLV3_CTRL, 0x00);
    writeReg(MPU6050_RA_I2C_SLV4_ADDR, 0x00);
    writeReg(MPU6050_RA_I2C_SLV4_REG, 0x00);
    writeReg(MPU6050_RA_I2C_SLV4_DO, 0x00);
    writeReg(MPU6050_RA_I2C_SLV4_CTRL, 0x00);
    writeReg(MPU6050_RA_I2C_SLV4_DI, 0x00);

    //MPU6050_RA_I2C_MST_STATUS //Read-only
    //Setup INT pin and AUX I2C pass through
    writeReg(MPU6050_RA_INT_PIN_CFG, 0x00);
    //Enable data ready interrupt
    writeReg(MPU6050_RA_INT_ENABLE, 0x00);

    //MPU6050_RA_DMP_INT_STATUS        //Read-only
    //MPU6050_RA_INT_STATUS 3A        //Read-only
    //MPU6050_RA_ACCEL_XOUT_H         //Read-only
    //MPU6050_RA_ACCEL_XOUT_L         //Read-only
    //MPU6050_RA_ACCEL_YOUT_H         //Read-only
    //MPU6050_RA_ACCEL_YOUT_L         //Read-only
    //MPU6050_RA_ACCEL_ZOUT_H         //Read-only
    //MPU6050_RA_ACCEL_ZOUT_L         //Read-only
    //MPU6050_RA_TEMP_OUT_H         //Read-only
    //MPU6050_RA_TEMP_OUT_L         //Read-only
    //MPU6050_RA_GYRO_XOUT_H         //Read-only
    //MPU6050_RA_GYRO_XOUT_L         //Read-only
    //MPU6050_RA_GYRO_YOUT_H         //Read-only
    //MPU6050_RA_GYRO_YOUT_L         //Read-only
    //MPU6050_RA_GYRO_ZOUT_H         //Read-only
    //MPU6050_RA_GYRO_ZOUT_L         //Read-only
    //MPU6050_RA_EXT_SENS_DATA_00     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_01     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_02     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_03     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_04     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_05     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_06     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_07     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_08     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_09     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_10     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_11     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_12     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_13     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_14     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_15     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_16     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_17     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_18     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_19     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_20     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_21     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_22     //Read-only
    //MPU6050_RA_EXT_SENS_DATA_23     //Read-only
    //MPU6050_RA_MOT_DETECT_STATUS     //Read-only

    //Slave out, dont care
    writeReg(MPU6050_RA_I2C_SLV0_DO, 0x00);
    writeReg(MPU6050_RA_I2C_SLV1_DO, 0x00);
    writeReg(MPU6050_RA_I2C_SLV2_DO, 0x00);
    writeReg(MPU6050_RA_I2C_SLV3_DO, 0x00);
    //More slave config
    writeReg(MPU6050_RA_I2C_MST_DELAY_CTRL, 0x00);
    //Reset sensor signal paths
    writeReg(MPU6050_RA_SIGNAL_PATH_RESET, 0x00);
    //Motion detection control
    writeReg(MPU6050_RA_MOT_DETECT_CTRL, 0x00);
    //Disables FIFO, AUX I2C, FIFO and I2C reset bits to 0
    writeReg(MPU6050_RA_USER_CTRL, 0x00);
    //Sets clock source to gyro reference w/ PLL
    writeReg(MPU6050_RA_PWR_MGMT_1, 0b00000010);
    //Controls frequency of wakeups in accel low power mode plus the sensor standby modes
    writeReg(MPU6050_RA_PWR_MGMT_2, 0x00);
    //MPU6050_RA_BANK_SEL            //Not in datasheet
    //MPU6050_RA_MEM_START_ADDR        //Not in datasheet
    //MPU6050_RA_MEM_R_W            //Not in datasheet
    //MPU6050_RA_DMP_CFG_1            //Not in datasheet
    //MPU6050_RA_DMP_CFG_2            //Not in datasheet
    //MPU6050_RA_FIFO_COUNTH        //Read-only
    //MPU6050_RA_FIFO_COUNTL        //Read-only
    //Data transfer to and from the FIFO buffer
    writeReg(MPU6050_RA_FIFO_R_W, 0x00);
    //MPU6050_RA_WHO_AM_I             //Read-only, I2C address

    //cout << "MPU6050 Setup Complete" << endl;

    //sprintf(filename, "writeReg() completed! STATUS = %d\n", test);
    //putsUART1( filename );
}

void MPU6050::Calibrate_Gyros()
{
    int	GYRO_XOUT_OFFSET_1000SUM = 0;
    int	GYRO_YOUT_OFFSET_1000SUM = 0;
    int	GYRO_ZOUT_OFFSET_1000SUM = 0;

    int a;

    for(int i = 0; i < 1000; i++)
    {
        readReg((unsigned char)MPU6050_RA_GYRO_XOUT_H, GYRO_XOUT_H);
        readReg((unsigned char)MPU6050_RA_GYRO_XOUT_L, GYRO_XOUT_L);
        readReg((unsigned char)MPU6050_RA_GYRO_YOUT_H, GYRO_YOUT_H);
        readReg((unsigned char)MPU6050_RA_GYRO_YOUT_L, GYRO_YOUT_L);
        readReg((unsigned char)MPU6050_RA_GYRO_ZOUT_H, GYRO_ZOUT_H);
        readReg((unsigned char)MPU6050_RA_GYRO_ZOUT_L, GYRO_ZOUT_L);

        GYRO_XOUT_OFFSET = ((GYRO_XOUT_H<<8)|GYRO_XOUT_L);
        GYRO_YOUT_OFFSET = ((GYRO_YOUT_H<<8)|GYRO_YOUT_L);
        GYRO_ZOUT_OFFSET = ((GYRO_ZOUT_H<<8)|GYRO_ZOUT_L);

        // Two's complement. Needs to be converted to make sense.
        // For info: (http://en.wikipedia.org/wiki/Two's_complement)
        if ( GYRO_XOUT_OFFSET > 0x7FFF ) 	// 0x7FFF = 32767
            GYRO_XOUT_OFFSET = GYRO_XOUT_OFFSET - 0xFFFF;
        if ( GYRO_YOUT_OFFSET > 0x7FFF ) 	// 0x7FFF = 32767
            GYRO_YOUT_OFFSET = GYRO_YOUT_OFFSET - 0xFFFF;
        if ( GYRO_ZOUT_OFFSET > 0x7FFF ) 	// 0x7FFF = 32767
            GYRO_ZOUT_OFFSET = GYRO_ZOUT_OFFSET - 0xFFFF;

        GYRO_XOUT_OFFSET_1000SUM += GYRO_XOUT_OFFSET;
        GYRO_YOUT_OFFSET_1000SUM += GYRO_YOUT_OFFSET;
        GYRO_ZOUT_OFFSET_1000SUM += GYRO_ZOUT_OFFSET;
        
        a = 0;
        while( a < 1000 )
        {
            a++;
        }
    }
    GYRO_XOUT_OFFSET = GYRO_XOUT_OFFSET_1000SUM / 1000;
    GYRO_YOUT_OFFSET = GYRO_YOUT_OFFSET_1000SUM / 1000;
    GYRO_ZOUT_OFFSET = GYRO_ZOUT_OFFSET_1000SUM / 1000;    
}

//Gets raw accelerometer data from MPU6050 and converts values to g
void MPU6050::Get_Accel_Values()
{
    int ACCEL_XOUT;
    int ACCEL_YOUT;
    int ACCEL_ZOUT;

    readReg((unsigned char)MPU6050_RA_ACCEL_XOUT_H, ACCEL_XOUT_H);
    readReg((unsigned char)MPU6050_RA_ACCEL_XOUT_L, ACCEL_XOUT_L);
    readReg((unsigned char)MPU6050_RA_ACCEL_YOUT_H, ACCEL_YOUT_H);
    readReg((unsigned char)MPU6050_RA_ACCEL_YOUT_L, ACCEL_YOUT_L);
    readReg((unsigned char)MPU6050_RA_ACCEL_ZOUT_H, ACCEL_ZOUT_H);
    readReg((unsigned char)MPU6050_RA_ACCEL_ZOUT_L, ACCEL_ZOUT_L);

    ACCEL_XOUT = ((ACCEL_XOUT_H<<8)|ACCEL_XOUT_L);
    ACCEL_YOUT = ((ACCEL_YOUT_H<<8)|ACCEL_YOUT_L);
    ACCEL_ZOUT = ((ACCEL_ZOUT_H<<8)|ACCEL_ZOUT_L);

    // Two's complement. Needs to be converted to make sense.
    // For info: (http://en.wikipedia.org/wiki/Two's_complement)
    if ( ACCEL_XOUT > 0x7FFF ) 	// 0x7FFF = 32767
        ACCEL_XOUT -= 0xFFFF;
    if ( ACCEL_YOUT > 0x7FFF ) 	// 0x7FFF = 32767
        ACCEL_YOUT = ACCEL_YOUT - 0xFFFF;
    if ( ACCEL_ZOUT > 0x7FFF ) 	// 0x7FFF = 32767
        ACCEL_ZOUT = ACCEL_ZOUT - 0xFFFF;

    // Converts raw data into g. 
    // See MPU-6050 Product Specification.pdf page 13 for more information.
    ACCEL_XFORCE = (float)ACCEL_XOUT / ACCEL_XSCALE;
    ACCEL_YFORCE = (float)ACCEL_YOUT / ACCEL_YSCALE;
    ACCEL_ZFORCE = (float)ACCEL_ZOUT / ACCEL_ZSCALE;
}

//Converts the already acquired accelerometer data into 3D euler angles
void MPU6050::Get_Accel_Angles()
{
    // 180 / pi = 57.296    
    ACCEL_XANGLE = 57.296 * atan( ACCEL_YFORCE / sqrt( ACCEL_ZFORCE * ACCEL_ZFORCE + ACCEL_XFORCE * ACCEL_XFORCE));
    ACCEL_YANGLE = 57.296 * atan( ACCEL_XFORCE / sqrt( ACCEL_ZFORCE * ACCEL_ZFORCE + ACCEL_YFORCE * ACCEL_YFORCE));
}

//Function to read the gyroscope rate data and convert it into degrees/s
void MPU6050::Get_Gyro_Rates()
{
    int GYRO_XOUT;
    int GYRO_YOUT;
    int GYRO_ZOUT;

    readReg((unsigned char)MPU6050_RA_GYRO_XOUT_H, GYRO_XOUT_H);
    readReg((unsigned char)MPU6050_RA_GYRO_XOUT_L, GYRO_XOUT_L);
    readReg((unsigned char)MPU6050_RA_GYRO_YOUT_H, GYRO_YOUT_H);
    readReg((unsigned char)MPU6050_RA_GYRO_YOUT_L, GYRO_YOUT_L);
    readReg((unsigned char)MPU6050_RA_GYRO_ZOUT_H, GYRO_ZOUT_H);
    readReg((unsigned char)MPU6050_RA_GYRO_ZOUT_L, GYRO_ZOUT_L);

    GYRO_XOUT = ((GYRO_XOUT_H<<8)|GYRO_XOUT_L);
    GYRO_YOUT = ((GYRO_YOUT_H<<8)|GYRO_YOUT_L);
    GYRO_ZOUT = ((GYRO_ZOUT_H<<8)|GYRO_ZOUT_L);

    // Two's complement. Needs to be converted to make sense.
    // For info: (http://en.wikipedia.org/wiki/Two's_complement)
    if ( GYRO_XOUT > 0x7FFF ) 	// 0x7FFF = 32767
        GYRO_XOUT = GYRO_XOUT - 0xFFFF;
    if ( GYRO_YOUT > 0x7FFF ) 	// 0x7FFF = 32767
        GYRO_YOUT = GYRO_YOUT - 0xFFFF;
    if ( GYRO_ZOUT > 0x7FFF ) 	// 0x7FFF = 32767
        GYRO_ZOUT = GYRO_ZOUT - 0xFFFF;

    GYRO_XOUT -= GYRO_XOUT_OFFSET;
    GYRO_YOUT -= GYRO_YOUT_OFFSET;
    GYRO_ZOUT -= GYRO_ZOUT_OFFSET;

    // Converts raw data into degrees/s.
    // See MPU-6050 Product Specification.pdf page 12 for more information.
    GYRO_XRATE = (float)GYRO_XOUT / GYRO_XSCALE;
    GYRO_YRATE = (float)GYRO_YOUT / GYRO_YSCALE;
    GYRO_ZRATE = (float)GYRO_ZOUT / GYRO_ZSCALE;
}

void MPU6050::CalcAngleX()
{
    XANGLE = 0.998 * (XANGLE + GYRO_XRATE / 1000) + 0.002 * ACCEL_XANGLE;
}

void MPU6050::CalcAngleY()
{
    YANGLE = 0.998 * (YANGLE + GYRO_YRATE / 1000) + 0.002 * ACCEL_YANGLE;
}

/**********************************************************************
 * This function opens the I2C device by simply calling the open system
 * call on the I2C device specified in the i2cFileName string. The I2C
 * device is opened for writing and reading. The i2cDescriptor private
 * variable is set by the return value of the open() system call.
 * This variable will be used to reference the opened I2C device by the
 * ioctl() & close() system calls.
 * ********************************************************************/
/*int MPU6050::openI2C()
{
    this->i2cDescriptor = open(i2cFileName.c_str(), O_RDWR);
    if(this->i2cDescriptor < 0)
    {

        perror("Could not open file (1)");

        exit(1);
    }

    return i2cDescriptor;
}

*/

BOOL MPU6050::I2CInit()
{
    INT32 actualClock;
    BOOL Success = TRUE;

    // Added but not tested yet. This is not included in example from Microchip.
    //However it is included in the following example:
    //https://gobotronics.wordpress.com/2010/12/09/i2c-eeprom-pic32/
    //I2CConfigure(this->i2cBusId, I2C_ENABLE_SLAVE_CLOCK_STRETCHING | I2C_ENABLE_HIGH_SPEED);
    I2CConfigure(this->i2cBusId, I2C_ENABLE_HIGH_SPEED);
    
    // Set the I2C baudrate
    actualClock = I2CSetFrequency( this->i2cBusId, GetPeripheralClock(), I2C_CLOCK_FREQ );
    if ( abs(actualClock-I2C_CLOCK_FREQ) > I2C_CLOCK_FREQ/10 )
    {
        //DBPRINTF("Error: I2C1 clock frequency (%u) error exceeds 10%%.\n", (unsigned)actualClock);
        sprintf(filename, "Error: I2C1 clock frequency (%u) error exceeds 10%%.\n", (unsigned)actualClock );
        Success = FALSE;
    }
    else
    {
        sprintf(filename, "I2CInit(): I2C1 clock frequency OK.\n");
    }

    // this function locks the application! Why??!!
    //putsUART1( filename );

    // Enable the I2C bus
    I2CEnable( this->i2cBusId, TRUE );

    return Success;
}


/*********************************************************************
 * This function closes the I2C device by calling the close() system call
 * on the I2C device descriptor.
 *
 *******************************************************************/
/*int MPU6050::closeI2C()
{

    int retVal = -1;

    retVal = close(this->i2cDescriptor);
    if(retVal < 0)
    {

        perror("Could not close file (1)");

        exit(1);
    }
    return retVal;
}
*/
/********************************************************************
 *This function writes a byte of data "data" to a specific register
 *"reg_addr" in the I2C device This involves sending these two bytes
 *in order to the i2C device by means of the ioctl() command. Since
 *both bytes are written (no read/write switch), both pieces
 *of information can be sent in a single message (i2c_msg structure)
 ********************************************************************/
/*int MPU6050::writeReg(unsigned char reg_addr, unsigned char data)
{

    unsigned char buff[2];
    int retVal = -1;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[1];

    buff[0] = reg_addr;
    buff[1] = data;

    messages[0].addr = deviceAddress;
    messages[0].flags = 0;
    messages[0].len = sizeof(buff);
    messages[0].buf = buff;

    packets.msgs = messages;
    packets.nmsgs = 1;

    retVal = ioctl(this->i2cDescriptor, I2C_RDWR, &packets);
    if(retVal < 0)

        perror("Write to I2C Device failed");

    return retVal;
}
*/
/********************************************************************
 *This function reads a byte of data "data" from a specific register
 *"reg_addr" in the I2C device. This involves sending the register address
 *byte "reg_Addr" with "write" asserted and then instructing the
 *I2C device to read a byte of data from that address ("read asserted").
 *This necessitates the use of two i2c_msg structs. One for the register
 *address write and another for the read from the I2C device i.e.
 *I2C_M_RD flag is set. The read data is then saved into the reference *variable "data".
 ********************************************************************/
/*int MPU6050::readReg(unsigned char reg_addr, unsigned char &data)
{

    unsigned char *inbuff, outbuff;
    int retVal = -1;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    outbuff = reg_addr;
    messages[0].addr = deviceAddress;
    messages[0].flags= 0;
    messages[0].len = sizeof(outbuff);
    messages[0].buf = &outbuff;

    inbuff = &data;
    messages[1].addr = deviceAddress;
    messages[1].flags = I2C_M_RD;
    messages[1].len = sizeof(inbuff);
    messages[1].buf = inbuff;

    packets.msgs = messages;
    packets.nmsgs = 2;

    retVal = ioctl(this->i2cDescriptor, I2C_RDWR, &packets);
    if(retVal < 0)

        perror("Read from I2C Device failed");

    return retVal;
}
*/


BOOL MPU6050::readReg( UINT8 regAddress, UINT8 &data )
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
    I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, this->deviceAddress, I2C_WRITE);
    i2cData[0] = SlaveAddress.byte;
    i2cData[1] = regAddress;                 // MPU6050 data address to read (0x75 = WHO_AM_I which contains 0x68)
    DataSz = 2;


    // Start the transfer
    if( !StartTransfer(FALSE) )
    {
        //while(1);
        Success = FALSE;

        sprintf( filename, "Error in #1 StartTransfer(): when reading address %u.\n", (unsigned)regAddress );
        putsUART1( filename );
    }

    // Address the device.
    Index = 0;
    while( Success & (Index < DataSz) )
    {
        // Transmit a byte
        if (TransmitOneByte(i2cData[Index]))
            Index++;
        else
        {
            Success = FALSE;

            sprintf( filename, "Error in #1 TransmitOneByte(): when reading address %u.\n", (unsigned)regAddress );
            putsUART1( filename );
        }
        // Verify that the byte was acknowledged
        if(!I2CByteWasAcknowledged( this->i2cBusId ))
        {
            //DBPRINTF("Error: Sent byte was not acknowledged\n");
            Success = FALSE;

            sprintf( filename, "Error in #1 I2CByteWasAcknowledged(): when reading address %u.\n", (unsigned)regAddress );
            putsUART1( filename );
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

            sprintf( filename, "Error in #2 StartTransfer(): when reading address %u.\n", (unsigned)regAddress );
            putsUART1( filename );
        }

        // Transmit the address with the READ bit set
        I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, this->deviceAddress, I2C_READ);
        if (TransmitOneByte(SlaveAddress.byte))
        {
            // Verify that the byte was acknowledged
            if(!I2CByteWasAcknowledged( this->i2cBusId ))
            {
                //DBPRINTF("Error: Sent byte was not acknowledged\n");
                Success = FALSE;

                sprintf( filename, "Error in #2 I2CByteWasAcknowledged(): when reading address %u.\n", (unsigned)regAddress );
                putsUART1( filename );
            }
        }
        else
        {
            Success = FALSE;

            sprintf( filename, "Error in #2 TransmitOneByte(): when reading address %u.\n", (unsigned)regAddress );
            putsUART1( filename );
        }
    }

    //i2cbyte = 9;

    // Read the data from the desired address
    if(Success)
    {
        if(I2CReceiverEnable( this->i2cBusId , TRUE) == I2C_RECEIVE_OVERFLOW)
        {
            //DBPRINTF("Error: I2C Receive Overflow\n");
            Success = FALSE;

            sprintf( filename, "Error I2CReceiverEnable(): when reading address %u. I2C Receive Overflow.\n", (unsigned)regAddress );
            putsUART1( filename );
        }
        else
        {
            while(!I2CReceivedDataIsAvailable( this->i2cBusId ));
            i2cbyte = I2CGetByte( this->i2cBusId );
        }
    }

    // End the transfer
    StopTransfer();

    data = i2cbyte;

    if(!Success)
    {

        //mPORTBSetBits(BIT_2);
        //mPORTBClearBits(BIT_3);

        sprintf( filename, "Error in readReg(): when reading address %u.\n", (unsigned)regAddress );
        putsUART1( filename );
    }    

    return Success;
}

BOOL MPU6050::writeReg(UINT8 regAddress, UINT8 data)
{
    UINT8               i2cData[10];
    I2C_7_BIT_ADDRESS   SlaveAddress;
    int                 Index;
    int                 DataSz;
    BOOL                Acknowledged = FALSE;
    BOOL                Success = TRUE;    

    //sprintf(filename, "Starting writeReg().\n");
    //putsUART1( filename );

    // Initialize the data buffer
    I2C_FORMAT_7_BIT_ADDRESS(SlaveAddress, this->deviceAddress, I2C_WRITE);
    i2cData[0] = SlaveAddress.byte;
    i2cData[1] = regAddress;        // Register Address to write
    i2cData[2] = data;              // Data to write
    DataSz = 3;

    

    // Start the transfer
    if( !StartTransfer(FALSE) )
    {
        Success = FALSE;
        sprintf( filename, "Error in #1 StartTransfer(): when writing address %u.\n", (unsigned)regAddress );
        putsUART1( filename );
    }

    // Transmit all data
    Index = 0;
    while( Success && (Index < DataSz) )
    {
        // Transmit a byte
        if (TransmitOneByte(i2cData[Index++]))
        {   
            // Verify that the byte was acknowledged
            if(!I2CByteWasAcknowledged( this->i2cBusId ))
            {               
                Success = FALSE;

                sprintf( filename, "Error in #1 I2CByteWasAcknowledged(): when writing address %u.\n", (unsigned)regAddress );
                putsUART1( filename );
            }
        }
        else
        {
            Success = FALSE;

            sprintf( filename, "Error in #1 TransmitOneByte(): when writing address %u.\n", (unsigned)regAddress );
            putsUART1( filename );
        }
    }    

    //sprintf(filename, "Before StopTransfer()\n");
    //putsUART1( filename );

    // End the transfer
    StopTransfer();

    //sprintf(filename, "After StopTransfer()\n");
    //putsUART1( filename );

    // Wait for device to complete write process, by polling the ack status.
    while(Acknowledged != TRUE && Success != FALSE)
    {
        //sprintf(filename, "Inside the loop\n");
        //putsUART1( filename );

        // Start the transfer
        if( StartTransfer(FALSE) )
        {            
            // Transmit just the device's address
            if (TransmitOneByte(SlaveAddress.byte))
            {
                // Check to see if the byte was acknowledged
                Acknowledged = I2CByteWasAcknowledged( this->i2cBusId );

                /*if( !Acknowledged )
                {
                    sprintf( filename, "!Acknowledged %u.\n", (unsigned)regAddress );
                    putsUART1( filename );
                }*/
            }
            else
            {
                Success = FALSE;

                sprintf( filename, "Error in #2 TransmitOneByte() - !starttranfer : when writing address %u.\n", (unsigned)regAddress );
                putsUART1( filename );


            }
                // End the transfer
            StopTransfer();
        }
        else
        {
            Success = FALSE;

            sprintf( filename, "Error in #2 StartTransfer(): when writing address %u.\n", (unsigned)regAddress );
            putsUART1( filename );
        }
    }

    //sprintf(filename, "After the loop\n");
    //putsUART1( filename );

    if( !Success )
    {
        sprintf( filename, "Error in writeReg(): when writing to address %u.\n", (unsigned)regAddress );
        putsUART1( filename );
    }
   
    return Success;
}


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

BOOL MPU6050::StartTransfer( BOOL restart )
{
    I2C_STATUS status = I2C_START;
    //UINT16 count = 0;

    //sprintf(filename, "Starting StartTransfer(), status = %d.\n", status);
    //putsUART1( filename );

    // Send the Start (or Restart) signal
    if(restart)
    {
        I2C_RESULT res = I2C_SUCCESS;
        if((res = I2CRepeatStart( this->i2cBusId )) != I2C_SUCCESS)
        {
            sprintf(filename, "Repeat start, status = %d.\n",res);
            putsUART1( filename );
            // Do not return, try to connect anyway and fail
        }
    }
    else
    {
        // Wait for the bus to be idle, then start the transfer
        //while( !I2CBusIsIdle(MPU6050_I2C_BUS) );

        // Checks if the bus is idle, and starts the transfer if so
        if( I2CBusIsIdle( this->i2cBusId ) )
        {
            if(I2CStart( this->i2cBusId ) != I2C_SUCCESS)
            {
                //DBPRINTF("Error: Bus collision during transfer Start\n");

                sprintf( filename, "Error in I2CStart(). Bus collision on bus %u during transfer Start.\n", (unsigned)this->i2cBusId );
                putsUART1( filename );

                return FALSE;
            }
        }
        else
        {
            sprintf( filename, "Error in I2CBusIsIdle(). Bus %u is not idle.\n", (unsigned)this->i2cBusId );
            putsUART1( filename );

            return FALSE;
        }
    }

    //sprintf( filename, "StartTransfer(). Checking for Start response...\n" );
    //putsUART1( filename );
    unsigned int max_tries = 64000, count = 0;
    // Wait for the signal to complete or until tries are out
    do
    {
        status = I2CGetStatus( this->i2cBusId );
        //sprintf( filename, "StartTransfer(). Status is %u \n", status & I2C_START );
        //putsUART1( filename );
    } while (!(status & I2C_START) && ++count < max_tries);
    
    if( count >= max_tries )
    {
        sprintf( filename, "Error in StartTransfer(). Timeout!\n" );
        putsUART1( filename );  

        return FALSE;
    }

    //sprintf( filename, "StartTransfer(). Function successfully completed!\n" );
    //putsUART1( filename );

    return TRUE;
}


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

BOOL MPU6050::TransmitOneByte( UINT8 data )
{
    UINT16 count = 0;

    // Wait for the transmitter to be ready
    while( !I2CTransmitterIsReady( this->i2cBusId ) && count < 64000 )
    {
        count++;
    }

    if( I2CTransmitterIsReady( this->i2cBusId ) )
    {
        // Transmit the byte
        if(I2CSendByte( this->i2cBusId, data) == I2C_MASTER_BUS_COLLISION)
        {
            sprintf( filename, "Error in TransmitOneByte(). I2C Master Bus Collision.\n" );
            putsUART1( filename );

            //DBPRINTF("Error: I2C Master Bus Collision\n");
            return FALSE;
        }

        count = 0;
        // Wait for the transmission to finish
        while( !I2CTransmissionHasCompleted( this->i2cBusId ) && count < 64000 )
        {
            count++;
        }

        if( count >= 64000 )
        {
            sprintf( filename, "Error: TransmitOneByte(). Loop timeout for I2CTransmissionHasCompleted().\n" );
            putsUART1( filename );
            return FALSE;
        }
    }
    else
    {
        sprintf( filename, "Error: TransmitOneByte(). Loop timeout for I2CTransmitterIsReady().\n" );
        putsUART1( filename );
        return FALSE;
    }
    return TRUE;
}


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

void MPU6050::StopTransfer( void )
{
    I2C_STATUS  status;
    UINT8 count = 0;

    // Send the Stop signal
    I2CStop( this->i2cBusId );

    // Wait for the signal to complete
    do
    {
        status = I2CGetStatus( this->i2cBusId );
        count++;

    } while ( !(status & I2C_STOP) && count < 200); 
}