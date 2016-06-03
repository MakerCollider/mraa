#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mraa.h>

int main(int argc, char * argv[])
{
	mraa_pwm_context pwm;
	int index=0;
	int value=0;
	float percent=0.0f;
	if(argc<3){
		printf("Usage: mraapwm [pin] [value:[0,100]]\n"
		       "eg:mraafade 8 50\n"
		       "   set IO[8] to 50%.\n");
		return -1;
	}
	index = strtol(argv[1],NULL,10);
	value = strtol(argv[2],NULL,10);
	printf("setting pin:%d to %d.\n",index,value);
	
	//init pwm
	pwm = mraa_pwm_init(index);
	if(pwm==NULL){
		printf("Fail to init the pwm pin data.\n");
		return -1;
	}
	
	//init pwm 
	mraa_pwm_enable(pwm,1);
	percent=(float)value/100.0f;
	mraa_pwm_write(pwm,percent);
  return 0;	
}
