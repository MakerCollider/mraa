/**************************************************************************/
/*!
    @file     ADS1x1x.h
    @author   CPV (Elektor)
    @license  BSD (see license.txt)

    Driver for the ADS101x/ADS111x Sigma-Delta Analog/Digital Converters.

    This started out as a port of the Adafruit library, but ended
    up largely rewritten.

    v1.0 - First release
*/
/**************************************************************************/

#ifndef __ADS1115_H__
#define __ADS1115_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mraa.h>


#define ADC_MRAA_IO_MAX_INDEX 3
#define ADC_MRAA_IO_SDA 14
#define ADC_MRAA_IO_SCL 13

#define ADC_MRAA_RANGE 0x7fff
#define ADC_MRAA_VOLTAGE 4.096f
/*
@purpose:read the index value
@index, the ADC pin, from 0~3
@return adc vlaue.
*/
int mraa_nbext_adc_read(int index,int16_t *p_val);
int mraa_nbext_adc_read_float(int index,float *p_val);


#ifdef __cplusplus
}
#endif

#endif // __ADS1x1x_H__
