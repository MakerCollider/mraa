extern "C" {
	#include <stdio.h>
	#include <sys/ioctl.h>
	#include <fcntl.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <string.h>
	#include <math.h>
}	

	//#include "ArduinoWrapper.h"

#include "edison_i2c.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

using namespace std;

int main(int args,char* argv[])
{
	MPU6050 mpu;
	int ret =0;
//set the arduino
	init_arduino();
	Wire.begin();
	
//set the mpu
	printf("Initializing I2C devices...\n");
  mpu.initialize();
	ret = mpu.testConnection();
	printf( ret ? ("MPU6050 connection successful") : ("MPU6050 connection failed"));
  	if(ret){
		printf("\nmpu6050 initialized &  detected.\n");
  	}
exit:	
	Wire.end();
	return ret;
}
