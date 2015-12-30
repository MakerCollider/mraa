extern "C" {
	#include <stdio.h>
	#include <sys/ioctl.h>
	#include <fcntl.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <string.h>
	#include <math.h>
}	

#include "edison_i2c.h"
#include "MS561101BA.h"

int main(int args,char* argv[])
{
	int i;
	MS561101BA baro = MS561101BA();	
	Wire.begin();
	delay(100);
	uint8_t addr=0x77;
	int err_count=0;
	float temp=0.0f;
	float press=0.0f;
	int ret=0;

	printf("i2c device address is 0x%x\n",addr);
	// Embedded adventures module CSB is VCC so HIGH
	baro.init(addr); 
	//speed and power
	Wire.setI2Cfreq(MRAA_I2C_HIGH);
	system("echo on >  /sys/devices/pci0000:00/0000:00:09.1/power/control");

	for(i=0;i<5;i++){
		temp = baro.getTemperature(MS561101BA_OSR_4096);
		press = baro.getPressure(MS561101BA_OSR_4096);
		printf("[%2d]temp:%.4f, press:%.4f, ",i, temp,press);
		if(temp==0 || press<=0 || 
		   temp>100 || temp <-10 || 
		   press>3000){
			printf(" FAIL!!\n");
			err_count++;
		}else{
			printf("OK\n");
		}
		delay(50);
	}
	if(err_count>3){
		printf("5611chip Error %d, test FAIL!!\n",err_count);
		ret=0;
	}else{	
		printf("5611chip test OK.\n",err_count);
		ret=1;
	}
	
	//speed and power
	Wire.setI2Cfreq(MRAA_I2C_FAST);
	system("echo auto >  /sys/devices/pci0000:00/0000:00:09.1/power/control");
exit:	
	return ret;
}
