/*
 * TwoWire.h - TWI/I2C library for Linux Userspace
 * Copyright (c) 2013 Parav https://github.com/meanbot.
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

extern "C" {
#include <string.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
}

#include "edison_i2c.h"
#include "i2c.h"

#define LINUX_GPIO_DEBUG_ROOT		"/sys/kernel/debug/gpio_debug/"
#define LINUX_GPIO_CURRENT_PINMUX_FMT	LINUX_GPIO_DEBUG_ROOT "gpio%u/current_pinmux"
#define SDA 27
#define SCL 28

#define I2C2 0x4819c000

void EdisonI2C::begin(void)
{
	if(intI2Cpimux()<0)
	{
		return;
	}
	if ((adapter_nr = i2c_getadapter(I2C2)) < 0) {
		return;	
	}
	if ((i2c_fd = i2c_openadapter(adapter_nr)) < 0) {
		return;	
	}

}
void EdisonI2C::end(void)
{
	close(i2c_fd);
}

int EdisonI2C::intI2Cpimux()
{
	FILE* fp;
	char fs_path[128];
	int i2c_pin[]={SDA,SCL};
	int i=0;
	
	for(i=0;i<2;i++){
		/* Set pinmux mode  */
		char modebuffer[8];
		snprintf(fs_path, sizeof(fs_path), LINUX_GPIO_CURRENT_PINMUX_FMT, i2c_pin[i]);
		if (NULL == (fp = fopen(fs_path, "w+"))) {
			printf("[%s,%d]fail to open the mode file.%s.\n",__FILE__,__LINE__,fs_path);
			return -1;
		}
		rewind(fp);
		strcpy(modebuffer, "mode1");
		fwrite(modebuffer, sizeof(char), sizeof(modebuffer), fp);
		fclose(fp);
	}
	return 0;
}
uint8_t EdisonI2C::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop)
{
	if (quantity > BUFFER_LENGTH)
		quantity = BUFFER_LENGTH;

	/* Set slave address via ioctl  */ 
	i2c_setslave(i2c_fd, address);

	if(i2c_transfer) {
	/* Need to perform a combined read/write operation
	 */
		i2c_transfer = 0;
		if (sendStop == false)
			return 0;
		i2c_add_to_buf(address, 1, rxBuffer, quantity);
		if (!i2c_readwrite(i2c_fd)) {
			rxBufferIndex = 0;
			rxBufferLength = quantity;
			i2c_transfer = 0;
			return quantity;
		} else
			return 0;
	}
	if (i2c_readbytes(i2c_fd, rxBuffer, quantity) < 0)
		return 0;
	// set rx buffer iterator vars
	rxBufferIndex = 0;
	rxBufferLength = quantity;

	return quantity;
}

uint8_t EdisonI2C::requestFrom(uint8_t address, uint8_t quantity)
{
	return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) true);
}

uint8_t EdisonI2C::requestFrom(int address, int quantity)
{
	return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) true);
}

uint8_t EdisonI2C::requestFrom(int address, int quantity, int sendStop)
{
	return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) sendStop);
}

void EdisonI2C::beginTransmission(uint8_t address)
{
	if (i2c_fd < 0 || adapter_nr < 0)
		return;
	/* set slave address via ioctl in case we need to perform terminating 
	 * write operation
	 */
	i2c_setslave(i2c_fd, address);
	// save address of target and empty buffer
	txAddress = address;
	txBufferLength = 0;
}

void EdisonI2C::beginTransmission(int address)
{
	beginTransmission((uint8_t) address);
}

//
//	Originally, 'endTransmission' was an f(void) function.
//	It has been modified to take one parameter indicating
//	whether or not a STOP should be performed on the bus.
//	Calling endTransmission(false) allows a sketch to
//	perform a repeated start.
//
//	WARNING: Nothing in the library keeps track of whether
//	the bus tenure has been properly ended with a STOP. It
//	is very possible to leave the bus in a hung state if
//	no call to endTransmission(true) is made. Some I2C
//	devices will behave oddly if they do not see a STOP.
//
uint8_t EdisonI2C::endTransmission(uint8_t sendStop)
{
	int err;
	if (sendStop == true) {	

		// transmit buffer (blocking)
		if (txBufferLength > 1)
			err = i2c_writebytes(i2c_fd, txBuffer, txBufferLength);
		else if (txBufferLength == 1)
			err = i2c_writebyte(i2c_fd, *txBuffer);
		else
		/* FIXME: A zero byte transmit is typically used to check for an
		 * ACK from the slave device. I'm not sure if this is the
		 * correct way to do this.
		 */ 
			err = i2c_readbyte(i2c_fd);
		// empty buffer
		txBufferLength = 0;
		if (err < 0)
			return 2;
		return 0;
	} else {
	/* sendStop = false
	 * pretend we have held the bus while
	 * actually waiting for the next operation
	 */
		i2c_add_to_buf(txAddress, 0, txBuffer, txBufferLength);		
		i2c_transfer = 1;
		return 0;
	}
}

//	This provides backwards compatibility with the original
//	definition, and expected behaviour, of endTransmission
//
uint8_t EdisonI2C::endTransmission(void)
{
	return endTransmission(true);
}

size_t EdisonI2C::write(uint8_t data)
{
	if (txBufferLength >= BUFFER_LENGTH)
		return 0;
	txBuffer[txBufferLength++] = data;
	return 1;
}

size_t EdisonI2C::write(const uint8_t *data, size_t quantity)
{
	for (size_t i = 0; i < quantity; ++i) {
		if (txBufferLength >= BUFFER_LENGTH)
			return i;
		txBuffer[txBufferLength++] = data[i];
	}
	return quantity;
}

int EdisonI2C::available(void)
{
	return rxBufferLength - rxBufferIndex;
}

int EdisonI2C::read(void)
{
	if (rxBufferIndex < rxBufferLength)
		return rxBuffer[rxBufferIndex++];
	return -1;
}

int EdisonI2C::peek(void)
{
	if (rxBufferIndex < rxBufferLength)
		return rxBuffer[rxBufferIndex];
	return -1;
}

void EdisonI2C::flush(void)
{
	// Do nothing, use endTransmission(..) to force
	// data transfer.
}

int
EdisonI2C::setI2Cfreq(mraa_i2c_mode_t mode)
{
    FILE* sysnode = NULL;
    sysnode = fopen("/sys/devices/pci0000:00/0000:00:09.1/i2c_dw_sysnode/mode", "w+");
    if (sysnode == NULL) {
        return -1;
    }

    char bu[5];
    int length;
    switch (mode) {
        case MRAA_I2C_STD:
            length = snprintf(bu, sizeof(bu), "std");
            break;
        case MRAA_I2C_FAST:
            length = snprintf(bu, sizeof(bu), "fast");
            break;
        case MRAA_I2C_HIGH:
            length = snprintf(bu, sizeof(bu), "high");
            break;
        default:
            fclose(sysnode);
            return -1;
    }
	
    if (fwrite(bu, sizeof(char), sizeof(bu), sysnode) == -1) {
        fclose(sysnode);
        return -1;
    }
    fclose(sysnode);
    return 0;
}