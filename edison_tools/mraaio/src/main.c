#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mraa.h>

int main(int argc, char * argv[])
{
	mraa_gpio_context pin;
	int index=0;
	int value=0;
	if(argc<3){
		printf("Usage: mraaio [IO] [value]\n"
		       "eg:mraaio 0 1\n"
		       "   set IO[0] to 1.\n");
		return -1;
	}
	index = strtol(argv[1],NULL,10);
	value = strtol(argv[2],NULL,10);
	printf("setting IO%d to %d.\n",index,value);
	
	pin = mraa_gpio_init(index);
	if(pin==NULL){
		printf("Fail to init the io data.\n");
		return -1;
	}
	mraa_gpio_dir(pin,MRAA_GPIO_OUT);
	mraa_gpio_write(pin,value);
	
	if(pin){
		mraa_gpio_close(pin);
	}
  return 0;	
}
