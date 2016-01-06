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

#include "HMC5883L.h"


int main(int args,char* argv[])
{
	HMC5883L mag;
	int ret=0;
	
	Wire.begin();
	mag.initialize();
	printf("Initializing I2C devices...\n");
	
	ret = mag.testConnection();
	printf( ret ? ("HM5883 connection successful") : ("HM5883 connection failed"));  
	if(ret){
		printf("\nHM5883 initialized &  detected.\n");
	}
exit:	
	Wire.end();
	return ret;
}
