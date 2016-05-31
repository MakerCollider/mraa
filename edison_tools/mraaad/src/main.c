#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mraa.h>

int main(int argc, char * argv[])
{
	mraa_aio_context pin;
	int index=0;
	float value=0;
	int value_int=0;
	if(argc<2){
		printf("Usage: mraaad [AIO]\n"
		       "eg:mraaad 0\n"
		       "   set read A0 pin value.\n");
		return -1;
	}
	index = strtol(argv[1],NULL,10);
	printf("Reading A%d:\n",index);
	
	pin = mraa_aio_init(index);
	if(pin==NULL){
		printf("Fail to init the io data.\n");
		return -1;
	}
	value = mraa_aio_read_float(pin);
	value_int = mraa_aio_read(pin);
	printf("A%d value is [%f / 4.096v] [0x%x / 0x7fff].\n",index,value,value_int);
  	return 0;	
}
