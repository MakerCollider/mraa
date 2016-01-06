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
	
	switch(argc){
	case 2:
		index = strtol(argv[1],NULL,10);
		pin = mraa_gpio_init(index);
		if(pin==NULL){
			printf("Fail to init the io data.\n");
			return -1;
		}
		mraa_gpio_dir(pin,MRAA_GPIO_IN);
		value=mraa_gpio_read(pin);
		printf("IO%d value is %d.\n",index,value);	
		break;
	case 3:
		index = strtol(argv[1],NULL,10);
		value = strtol(argv[2],NULL,10);
		printf("Write IO%d to %d.\n",index,value);

		pin = mraa_gpio_init(index);
		if(pin==NULL){
			printf("Fail to init the io data.\n");
			return -1;
		}
		mraa_gpio_dir(pin,MRAA_GPIO_OUT);
		mraa_gpio_write(pin,value);
		break;
	default:
		printf("Usage: mraaio [IO] [value]\n"
		       "eg:mraaio 0 1\n"
		       "   write IO[0] to 1.\n"
		       "Usage: mraaio [IO]\n"
		       "eg:mraaio 0\n"
		       "   read IO[0].\n");
		return -1;
		break;
	}
	#if 0	
	if(pin){
		mraa_gpio_close(pin);
	}
	#endif
  return 0;	
}
