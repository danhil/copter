#include "mpu6050.h"

#define MPU6050_ADDRESS 0b11010010 // Address with end write bit
#define MPU6050_RA_XG_OFFS_TC 0x00 //[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_YG_OFFS_TC 0x01 //[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_ZG_OFFS_TC 0x02 //[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_X_FINE_GAIN 0x03 //[7:0] X_FINE_GAIN
#define MPU6050_RA_Y_FINE_GAIN 0x04 //[7:0] Y_FINE_GAIN
#define MPU6050_RA_Z_FINE_GAIN 0x05 //[7:0] Z_FINE_GAIN
#define MPU6050_RA_XA_OFFS_H 0x06 //[15:0] XA_OFFS
#define MPU6050_RA_XA_OFFS_L_TC 0x07
#define MPU6050_RA_YA_OFFS_H 0x08 //[15:0] YA_OFFS
#define MPU6050_RA_YA_OFFS_L_TC 0x09
#define MPU6050_RA_ZA_OFFS_H 0x0A //[15:0] ZA_OFFS
#define MPU6050_RA_ZA_OFFS_L_TC 0x0B
#define MPU6050_RA_XG_OFFS_USRH 0x13 //[15:0] XG_OFFS_USR
#define MPU6050_RA_XG_OFFS_USRL 0x14
#define MPU6050_RA_YG_OFFS_USRH 0x15 //[15:0] YG_OFFS_USR
#define MPU6050_RA_YG_OFFS_USRL 0x16
#define MPU6050_RA_ZG_OFFS_USRH 0x17 //[15:0] ZG_OFFS_USR
#define MPU6050_RA_ZG_OFFS_USRL 0x18
#define MPU6050_RA_SMPLRT_DIV 0x19
#define MPU6050_RA_CONFIG 0x1A
#define MPU6050_RA_GYRO_CONFIG 0x1B
#define MPU6050_RA_ACCEL_CONFIG 0x1C
#define MPU6050_RA_FF_THR 0x1D
#define MPU6050_RA_FF_DUR 0x1E
#define MPU6050_RA_MOT_THR 0x1F
#define MPU6050_RA_MOT_DUR 0x20
#define MPU6050_RA_ZRMOT_THR 0x21
#define MPU6050_RA_ZRMOT_DUR 0x22
#define MPU6050_RA_FIFO_EN 0x23
#define MPU6050_RA_I2C_MST_CTRL 0x24
#define MPU6050_RA_I2C_SLV0_ADDR 0x25
#define MPU6050_RA_I2C_SLV0_REG 0x26
#define MPU6050_RA_I2C_SLV0_CTRL 0x27
#define MPU6050_RA_I2C_SLV1_ADDR 0x28
#define MPU6050_RA_I2C_SLV1_REG 0x29
#define MPU6050_RA_I2C_SLV1_CTRL 0x2A
#define MPU6050_RA_I2C_SLV2_ADDR 0x2B
#define MPU6050_RA_I2C_SLV2_REG 0x2C
#define MPU6050_RA_I2C_SLV2_CTRL 0x2D
#define MPU6050_RA_I2C_SLV3_ADDR 0x2E
#define MPU6050_RA_I2C_SLV3_REG 0x2F
#define MPU6050_RA_I2C_SLV3_CTRL 0x30
#define MPU6050_RA_I2C_SLV4_ADDR 0x31
#define MPU6050_RA_I2C_SLV4_REG 0x32
#define MPU6050_RA_I2C_SLV4_DO 0x33
#define MPU6050_RA_I2C_SLV4_CTRL 0x34
#define MPU6050_RA_I2C_SLV4_DI 0x35
#define MPU6050_RA_I2C_MST_STATUS 0x36
#define MPU6050_RA_INT_PIN_CFG 0x37
#define MPU6050_RA_INT_ENABLE 0x38
#define MPU6050_RA_DMP_INT_STATUS 0x39
#define MPU6050_RA_INT_STATUS 0x3A
#define MPU6050_RA_ACCEL_XOUT_H 0x3B
#define MPU6050_RA_ACCEL_XOUT_L 0x3C
#define MPU6050_RA_ACCEL_YOUT_H 0x3D
#define MPU6050_RA_ACCEL_YOUT_L 0x3E
#define MPU6050_RA_ACCEL_ZOUT_H 0x3F
#define MPU6050_RA_ACCEL_ZOUT_L 0x40
#define MPU6050_RA_TEMP_OUT_H 0x41
#define MPU6050_RA_TEMP_OUT_L 0x42
#define MPU6050_RA_GYRO_XOUT_H 0x43
#define MPU6050_RA_GYRO_XOUT_L 0x44
#define MPU6050_RA_GYRO_YOUT_H 0x45
#define MPU6050_RA_GYRO_YOUT_L 0x46
#define MPU6050_RA_GYRO_ZOUT_H 0x47
#define MPU6050_RA_GYRO_ZOUT_L 0x48
#define MPU6050_RA_EXT_SENS_DATA_00 0x49
#define MPU6050_RA_EXT_SENS_DATA_01 0x4A
#define MPU6050_RA_EXT_SENS_DATA_02 0x4B
#define MPU6050_RA_EXT_SENS_DATA_03 0x4C
#define MPU6050_RA_EXT_SENS_DATA_04 0x4D
#define MPU6050_RA_EXT_SENS_DATA_05 0x4E
#define MPU6050_RA_EXT_SENS_DATA_06 0x4F
#define MPU6050_RA_EXT_SENS_DATA_07 0x50
#define MPU6050_RA_EXT_SENS_DATA_08 0x51
#define MPU6050_RA_EXT_SENS_DATA_09 0x52
#define MPU6050_RA_EXT_SENS_DATA_10 0x53
#define MPU6050_RA_EXT_SENS_DATA_11 0x54
#define MPU6050_RA_EXT_SENS_DATA_12 0x55
#define MPU6050_RA_EXT_SENS_DATA_13 0x56
#define MPU6050_RA_EXT_SENS_DATA_14 0x57
#define MPU6050_RA_EXT_SENS_DATA_15 0x58
#define MPU6050_RA_EXT_SENS_DATA_16 0x59
#define MPU6050_RA_EXT_SENS_DATA_17 0x5A
#define MPU6050_RA_EXT_SENS_DATA_18 0x5B
#define MPU6050_RA_EXT_SENS_DATA_19 0x5C
#define MPU6050_RA_EXT_SENS_DATA_20 0x5D
#define MPU6050_RA_EXT_SENS_DATA_21 0x5E
#define MPU6050_RA_EXT_SENS_DATA_22 0x5F
#define MPU6050_RA_EXT_SENS_DATA_23 0x60
#define MPU6050_RA_MOT_DETECT_STATUS 0x61
#define MPU6050_RA_I2C_SLV0_DO 0x63
#define MPU6050_RA_I2C_SLV1_DO 0x64
#define MPU6050_RA_I2C_SLV2_DO 0x65
#define MPU6050_RA_I2C_SLV3_DO 0x66
#define MPU6050_RA_I2C_MST_DELAY_CTRL 0x67
#define MPU6050_RA_SIGNAL_PATH_RESET 0x68
#define MPU6050_RA_MOT_DETECT_CTRL 0x69
#define MPU6050_RA_USER_CTRL 0x6A
#define MPU6050_RA_PWR_MGMT_1 0x6B
#define MPU6050_RA_PWR_MGMT_2 0x6C
#define MPU6050_RA_BANK_SEL 0x6D
#define MPU6050_RA_MEM_START_ADDR 0x6E
#define MPU6050_RA_MEM_R_W 0x6F
#define MPU6050_RA_DMP_CFG_1 0x70
#define MPU6050_RA_DMP_CFG_2 0x71
#define MPU6050_RA_FIFO_COUNTH 0x72
#define MPU6050_RA_FIFO_COUNTL 0x73
#define MPU6050_RA_FIFO_R_W 0x74
#define MPU6050_RA_WHO_AM_I 0x75
	
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

/*****************************************************************
 * This is the default constructor for the class. It assigns
 * all private variables to default values and calls the openI2C()
 * function to open the default I2C device "/dev/i2c-1".
 *****************************************************************/
MPU6050::MPU6050(void)
{    
	gyro_xsensitivity = 1.0;
	gyro_ysensitivity = 65.5;
	gyro_zsensitivity = 1.0;
	
	GYRO_XANGLE = 0.0;
	GYRO_YANGLE = 0.0;
	GYRO_ZANGLE = 0.0;
	
	this->i2cFileName = "/dev/i2c-1";
	
	this->deviceAddress = 0;
	
	this->i2cDescriptor = -1;
	
	cout << " Opening I2C Device" << endl;
	
	this->openI2C();
} 

/*******************************************************************
 * This is the overloaded constructor. It allows the programmer to
 * specify a custom I2C device & device address
 * The device descriptor is determined by the openI2C() private member 
 * function call.
 * *****************************************************************/
MPU6050::MPU6050(unsigned char dev_addr, std::string i2c_file_name)
{    
	gyro_xsensitivity = 1.0;
	gyro_ysensitivity = 65.5;
	gyro_zsensitivity = 1.0;
	
	GYRO_XANGLE = 0.0;
	GYRO_YANGLE = 0.0;
	GYRO_ZANGLE = 0.0;
	
	this->i2cFileName = i2c_file_name;    
	this->deviceAddress = dev_addr;

	this->i2cDescriptor = -1;
	
	cout << " Opening I2C Device" << endl;    
	this->openI2C();
}

/**********************************************************************
 * This is the class destructor it simply closes the open I2C device
 * by calling the closeI2C() which in turn calls the close() system call
 * *********************************************************************/ 
MPU6050::~MPU6050(void)
{
	cout << " Closing I2C Device" << endl;    
	this->closeI2C();
} 

void MPU6050::MPU6050_Test_I2C()
{
    unsigned char Data = 0x00;
    
    readReg( MPU6050_RA_WHO_AM_I, Data );
 
    if(Data == 0x68)
    {
		cout << " I2C Read Test Passed, MPU6050 Address: 0x" << hex << (int)Data << endl;
		
    }
    else
    {
		cout << " ERROR: I2C Read Test Failed. Data received: 0x" << hex << (int)Data << endl;
    } 
}

void MPU6050::Setup_MPU6050()
{
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
 
	cout << "MPU6050 Setup Complete" << endl;
}

void MPU6050::Calibrate_Gyros()
{	
	int	GYRO_XOUT_OFFSET_1000SUM = 0;
	int	GYRO_YOUT_OFFSET_1000SUM = 0;
	int	GYRO_ZOUT_OFFSET_1000SUM = 0;

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
		
		// Two's complement. Needs to be converted to make sense. For info: (http://en.wikipedia.org/wiki/Two's_complement)
		if ( GYRO_XOUT_OFFSET > 0x7FFF ) 	// 0x7FFF = 32767	
			GYRO_XOUT_OFFSET = GYRO_XOUT_OFFSET - 0xFFFF;
		if ( GYRO_YOUT_OFFSET > 0x7FFF ) 	// 0x7FFF = 32767	
			GYRO_YOUT_OFFSET = GYRO_YOUT_OFFSET - 0xFFFF;
		if ( GYRO_ZOUT_OFFSET > 0x7FFF ) 	// 0x7FFF = 32767	
			GYRO_ZOUT_OFFSET = GYRO_ZOUT_OFFSET - 0xFFFF;	
  
		GYRO_XOUT_OFFSET_1000SUM += GYRO_XOUT_OFFSET;
		GYRO_YOUT_OFFSET_1000SUM += GYRO_YOUT_OFFSET;
		GYRO_ZOUT_OFFSET_1000SUM += GYRO_ZOUT_OFFSET;
  
		//__delay_ms(1);
		//mSleep(1);		// function not accurate but it´s OK for now..
	}	
	GYRO_XOUT_OFFSET = GYRO_XOUT_OFFSET_1000SUM / 1000;
	GYRO_YOUT_OFFSET = GYRO_YOUT_OFFSET_1000SUM / 1000;
	GYRO_ZOUT_OFFSET = GYRO_ZOUT_OFFSET_1000SUM / 1000;
 	
	cout << "Gyro X offset sum: " << GYRO_XOUT_OFFSET_1000SUM << " Gyro X offset: " << GYRO_XOUT_OFFSET << endl;
	cout << "Gyro Y offset sum: " << GYRO_YOUT_OFFSET_1000SUM << " Gyro Y offset: " << GYRO_YOUT_OFFSET << endl;
	cout << "Gyro Z offset sum: " << GYRO_ZOUT_OFFSET_1000SUM << " Gyro Z offset: " << GYRO_ZOUT_OFFSET << endl;
}	

//Gets raw accelerometer data, performs no processing
void MPU6050::Get_Accel_Values()
{
	readReg((unsigned char)MPU6050_RA_ACCEL_XOUT_H, ACCEL_XOUT_H);
	readReg((unsigned char)MPU6050_RA_ACCEL_XOUT_L, ACCEL_XOUT_L);
	readReg((unsigned char)MPU6050_RA_ACCEL_YOUT_H, ACCEL_YOUT_H);
	readReg((unsigned char)MPU6050_RA_ACCEL_YOUT_L, ACCEL_YOUT_L);
	readReg((unsigned char)MPU6050_RA_ACCEL_ZOUT_H, ACCEL_ZOUT_H);
	readReg((unsigned char)MPU6050_RA_ACCEL_ZOUT_L, ACCEL_ZOUT_L);
 
	RAW_ACCEL_XOUT = ((ACCEL_XOUT_H<<8)|ACCEL_XOUT_L);
	RAW_ACCEL_YOUT = ((ACCEL_YOUT_H<<8)|ACCEL_YOUT_L);
	RAW_ACCEL_ZOUT = ((ACCEL_ZOUT_H<<8)|ACCEL_ZOUT_L);

	// Two's complement. Needs to be converted to make sense. For info: (http://en.wikipedia.org/wiki/Two's_complement)
	if ( RAW_ACCEL_XOUT > 0x7FFF ) 	// 0x7FFF = 32767	
		RAW_ACCEL_XOUT -= 0xFFFF;
	if ( RAW_ACCEL_YOUT > 0x7FFF ) 	// 0x7FFF = 32767	
		RAW_ACCEL_YOUT = RAW_ACCEL_YOUT - 0xFFFF;
	if ( RAW_ACCEL_ZOUT > 0x7FFF ) 	// 0x7FFF = 32767	
		RAW_ACCEL_ZOUT = RAW_ACCEL_ZOUT - 0xFFFF;

	// Converts raw data into g. See MPU-6050 Product Specification.pdf page 13 for more information.
	ACCEL_XOUT = (float)RAW_ACCEL_XOUT / 16384;
	ACCEL_YOUT = (float)RAW_ACCEL_YOUT / 16384;
	ACCEL_ZOUT = (float)RAW_ACCEL_ZOUT / 16384;	
}

//Converts the already acquired accelerometer data into 3D euler angles
void MPU6050::Get_Accel_Angles()
{
	ACCEL_XANGLE = 57.295*atan((float)ACCEL_YOUT/ sqrt(pow((float)ACCEL_ZOUT,2)+pow((float)ACCEL_XOUT,2)));
	ACCEL_YANGLE = 57.295*atan((float)-ACCEL_XOUT/ sqrt(pow((float)ACCEL_ZOUT,2)+pow((float)ACCEL_YOUT,2)));	
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

	// Two's complement. Needs to be converted to make sense. For info: (http://en.wikipedia.org/wiki/Two's_complement)
	if ( GYRO_XOUT > 0x7FFF ) 	// 0x7FFF = 32767	
		GYRO_XOUT = GYRO_XOUT - 0xFFFF;
	if ( GYRO_YOUT > 0x7FFF ) 	// 0x7FFF = 32767	
		GYRO_YOUT = GYRO_YOUT - 0xFFFF;
	if ( GYRO_ZOUT > 0x7FFF ) 	// 0x7FFF = 32767	
		GYRO_ZOUT = GYRO_ZOUT - 0xFFFF;	
	
	GYRO_XOUT -= GYRO_XOUT_OFFSET;
	GYRO_YOUT -= GYRO_YOUT_OFFSET;
	GYRO_ZOUT -= GYRO_ZOUT_OFFSET;		
		
	GYRO_XRATE = (float)GYRO_XOUT / gyro_xsensitivity;
	GYRO_YRATE = (float)GYRO_YOUT / gyro_ysensitivity;
	GYRO_ZRATE = (float)GYRO_ZOUT / gyro_zsensitivity;
	
	
	//GYRO_XANGLE += GYRO_XRATE * 0.01;
	
	
}

/**********************************************************************
 * This function opens the I2C device by simply calling the open system
 * call on the I2C device specified in the i2cFileName string. The I2C
 * device is opened for writing and reading. The i2cDescriptor private 
 * variable is set by the return value of the open() system call.
 * This variable will be used to reference the opened I2C device by the 
 * ioctl() & close() system calls.
 * ********************************************************************/  
 int MPU6050::openI2C()
 {    
	this->i2cDescriptor = open(i2cFileName.c_str(), O_RDWR);
	if(this->i2cDescriptor < 0)
	{
		
		perror("Could not open file (1)");
		
		exit(1);
	}
	
	return i2cDescriptor;
} 

/*********************************************************************
 * This function closes the I2C device by calling the close() system call 
 * on the I2C device descriptor.
*
 *******************************************************************/ 
int MPU6050::closeI2C()
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
   
/********************************************************************
 *This function writes a byte of data "data" to a specific register 
 *"reg_addr" in the I2C device This involves sending these two bytes 
 *in order to the i2C device by means of the ioctl() command. Since  
 *both bytes are written (no read/write switch), both pieces
 *of information can be sent in a single message (i2c_msg structure)  
 ********************************************************************/
int MPU6050::writeReg(unsigned char reg_addr, unsigned char data)
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

/********************************************************************
 *This function reads a byte of data "data" from a specific register 
 *"reg_addr" in the I2C device. This involves sending the register address 
 *byte "reg_Addr" with "write" asserted and then instructing the 
 *I2C device to read a byte of data from that address ("read asserted"). 
 *This necessitates the use of two i2c_msg structs. One for the register 
 *address write and another for the read from the I2C device i.e. 
 *I2C_M_RD flag is set. The read data is then saved into the reference *variable "data".  
 ********************************************************************/ 
 int MPU6050::readReg(unsigned char reg_addr, unsigned char &data)
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






