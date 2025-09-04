#ifndef MPU6050_H
#define MPU6050_H

#include <mbed.h>

using mbed::I2C;

// registers: 

#define SLAVE_ADDR 0x68 << 1

#define GYRO_CONFIG 0x1B 

#define INT_STATUS 0x3A
#define ACCEL_XOUT_H 0x3B 
#define ACCEL_XOUT_L 0x3C 
#define ACCEL_YOUT_H 0x3D 
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41 
#define TEMP_OUT_L 0x42 
#define GYRO_XOUT_H 0x43 
#define GYRO_XOUT_L 0x44 
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46 
#define GYRO_ZOUT_H 0x47 
#define GYRO_ZOUT_L 0x48 
#define PWR_MGMT_1 0x6B 

const char MUX_CMD_ = 0x01; // default MUX addr
const char MUX_ADDR_ = 0xEE;

const static char MUX_ADDR_LINES_[4] = {0x01, 0x02, 0x04, 0x08};

static bool AVAILABLE_IR_SLOTS_[4] = {true, true, true, true};


class MPU6050{

    public: 

       MPU6050(PinName SDA, PinName SCL);

       void setupPort(int port_choice); 

       void switchPort(int port_num);

       void read(int slave_addr_read, char *data_arr, int data_read_length); 
	  
	   void write(int slave_addr_write, char *cmd_arr, int data_write_length);

       void initialise();

       void readRawDataGyro();

       void setX(int16_t x);

       void setY(int16_t y); 

       void setZ(int16_t z);

       int getSensitivity();

       int getX(); 

       int getY();

       int getZ(); 
 
    private:

       I2C i2c; 

       int16_t x; 

       int16_t y; 

       int16_t z;  

};

#endif