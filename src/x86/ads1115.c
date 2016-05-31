#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mraa.h>

#include "ads1115.h"

// I2C definitions.
#define ADS1x1x_I2C_ADDRESS_ADDR_TO_GND  (0x48)
#define ADS1x1x_I2C_ADDRESS_ADDR_TO_VCC  (0x49)
#define ADS1x1x_I2C_ADDRESS_ADDR_TO_SDA  (0x4a)
#define ADS1x1x_I2C_ADDRESS_ADDR_TO_SCL  (0x4b)

// Chip "pointer" registers
#define ADS1x1x_REG_POINTER_MASK  (0x03)
#define ADS1x1x_REG_POINTER_CONVERSION  (0x00)
#define ADS1x1x_REG_POINTER_CONFIG  (0x01)
#define ADS1x1x_REG_POINTER_LO_THRESH  (0x02)
#define ADS1x1x_REG_POINTER_HI_THRESH  (0x03)

// Config register masks
#define ADS1x1x_REG_CONFIG_OS_MASK  (0x8000)
#define ADS1x1x_REG_CONFIG_MULTIPLEXER_MASK  (0x7000)
#define ADS1x1x_REG_CONFIG_PGA_MASK  (0x0E00)
#define ADS1x1x_REG_CONFIG_MODE_MASK  (0x0100)
#define ADS1x1x_REG_CONFIG_DATA_RATE_MASK  (0x00E0)
#define ADS1x1x_REG_CONFIG_COMPARATOR_MODE_MASK  (0x0010)
#define ADS1x1x_REG_CONFIG_COMPARATOR_POLARITY_MASK  (0x0008)
#define ADS1x1x_REG_CONFIG_COMPARATOR_LATCHING_MASK  (0x0004)  // Determines if ALERT/RDY pin latches once asserted
#define ADS1x1x_REG_CONFIG_COMPARATOR_QUEUE_MASK  (0x0003)

// Config register bits
#define ADS1x1x_REG_CONFIG_OS_IDLE  (0x8000)  // Read: bit = 1 when device is not performing a conversion

// The chips that we know of.
typedef enum
{
  ADS1013,
  ADS1014,
  ADS1015,
  ADS1113,
  ADS1114,
  ADS1115
}
ADS1x1x_chip_t;


// The following enums are to be used with the configuration functions.

typedef enum
{
  OS_SINGLE = 0x8000  // Single-conversion
}
ADS1x1x_os_t;

typedef enum
{
  MUX_DIFF_0_1 = 0x0000,  // Differential P = AIN0, N = AIN1 (default)
  MUX_DIFF_0_3 = 0x1000,  // Differential P = AIN0, N = AIN3
  MUX_DIFF_1_3 = 0x2000,  // Differential P = AIN1, N = AIN3
  MUX_DIFF_2_3 = 0x3000,  // Differential P = AIN2, N = AIN3
  MUX_SINGLE_0 = 0x4000,  // Single-ended AIN0
  MUX_SINGLE_1 = 0x5000,  // Single-ended AIN1
  MUX_SINGLE_2 = 0x6000,  // Single-ended AIN2
  MUX_SINGLE_3 = 0x7000   // Single-ended AIN3
}
ADS1x1x_mux_t;

typedef enum
{
  PGA_6144 = 0x0000,  // +/-6.144V range = Gain 2/3
  PGA_4096 = 0x0200,  // +/-4.096V range = Gain 1
  PGA_2048 = 0x0400,  // +/-2.048V range = Gain 2 (default)
  PGA_1024 = 0x0600,  // +/-1.024V range = Gain 4
  PGA_512 = 0x0800,  // +/-0.512V range = Gain 8
  PGA_256 = 0x0A00  // +/-0.256V range = Gain 16
}
ADS1x1x_pga_t;

typedef enum
{
  MODE_CONTINUOUS = 0x0000,  // Continuous conversion mode
  MODE_SINGLE_SHOT = 0x0100  // Power-down single-shot mode (default)
}
ADS1x1x_mode_t;

typedef enum
{
  // ADS101x
  DATA_RATE_ADS101x_128 = 0x0000,  // 128 samples per second
  DATA_RATE_ADS101x_250 = 0x0020,  // 250 samples per second
  DATA_RATE_ADS101x_490 = 0x0040,  // 490 samples per second
  DATA_RATE_ADS101x_920 = 0x0060,  // 920 samples per second
  DATA_RATE_ADS101x_1600 = 0x0080,  // 1600 samples per second (default)
  DATA_RATE_ADS101x_2400 = 0x00a0,  // 2400 samples per second
  DATA_RATE_ADS101x_3300 = 0x00c0,  // 3300 samples per second
  // ADS111x
  DATA_RATE_ADS111x_8 = 0x0000,  // 8 samples per second
  DATA_RATE_ADS111x_16 = 0x0020,  // 16 samples per second
  DATA_RATE_ADS111x_32 = 0x0040,  // 32 samples per second
  DATA_RATE_ADS111x_64 = 0x0060,  // 64 samples per second
  DATA_RATE_ADS111x_128 = 0x0080,  // 128 samples per second (default)
  DATA_RATE_ADS111x_250 = 0x00a0,  // 250 samples per second
  DATA_RATE_ADS111x_475 = 0x00c0,  // 475 samples per second
  DATA_RATE_ADS111x_860 = 0x00e0  // 860 samples per second
}
ADS1x1x_data_rate_t;

typedef enum
{
  COMPARATOR_MODE_TRADITIONAL = 0x0000,  // Traditional comparator with hysteresis (default)
  COMPARATOR_MODE_WINDOW = 0x0010  // Window comparator
}
ADS1x1x_comparator_mode_t;

typedef enum
{
  COMPARATOR_POLARITY_ACTIVE_LO = 0x0000,  // ALERT/RDY pin is low when active (default)
  COMPARATOR_POLARITY_ACTIVE_HI = 0x0008  // ALERT/RDY pin is high when active
}
ADS1x1x_comparator_polarity_t;

typedef enum
{
  COMPARATOR_NON_LATCHING = 0x0000,  // Non-latching comparator (default)
  COMPARATOR_LATCHING = 0x0004  // Latching comparator
}
ADS1x1x_comparator_latching_t;

typedef enum
{
  COMPARATOR_QUEUE_1 = 0x0000,  // Assert ALERT/RDY after one conversions
  COMPARATOR_QUEUE_2 = 0x0001,  // Assert ALERT/RDY after two conversions
  COMPARATOR_QUEUE_4 = 0x0002,  // Assert ALERT/RDY after four conversions
  COMPARATOR_QUEUE_NONE = 0x0003  // Disable the comparator and put ALERT/RDY in high state (default)
}
ADS1x1x_comparator_queue_t;


// You must declare this structure somewhere to use the ADC.
// You can have as many as you like.
typedef struct
{
  ADS1x1x_chip_t chip;
  uint8_t i2c_address;
  uint16_t config;
}
ADS1x1x_config_t;

int g_adc_is_started=0;
mraa_gpio_context g_sda;
mraa_gpio_context g_scl;
ADS1x1x_config_t g_adc;

// Easy usage:
// Call this at startup:
uint8_t ADS1x1x_init(ADS1x1x_config_t *p_config, ADS1x1x_chip_t chip, uint8_t i2c_address, ADS1x1x_mux_t input, ADS1x1x_pga_t gain);
// Then call:
void ADS1x1x_start_conversion(ADS1x1x_config_t *p_config);
// ... and wait a bit before calling this:
int16_t ADS1x1x_read(ADS1x1x_config_t *p_config);

// Configuration methods to call before calling ADS1x1x_start_conversion.
void ADS1x1x_set_threshold_lo(ADS1x1x_config_t *p_config, uint16_t value);
void ADS1x1x_set_threshold_hi(ADS1x1x_config_t *p_config, uint16_t value);
void ADS1x1x_set_os(ADS1x1x_config_t *p_config, ADS1x1x_os_t value);
void ADS1x1x_set_multiplexer(ADS1x1x_config_t *p_config, ADS1x1x_mux_t value);
void ADS1x1x_set_pga(ADS1x1x_config_t *p_config, ADS1x1x_pga_t value);
void ADS1x1x_set_mode(ADS1x1x_config_t *p_config, ADS1x1x_mode_t value);
void ADS1x1x_set_data_rate(ADS1x1x_config_t *p_config, ADS1x1x_data_rate_t value);
void ADS1x1x_set_comparator_mode(ADS1x1x_config_t *p_config, ADS1x1x_comparator_mode_t value);
void ADS1x1x_set_comparator_polarity(ADS1x1x_config_t *p_config, ADS1x1x_comparator_polarity_t value);
void ADS1x1x_set_comparator_latching(ADS1x1x_config_t *p_config, ADS1x1x_comparator_latching_t value);
void ADS1x1x_set_comparator_queue(ADS1x1x_config_t *p_config, ADS1x1x_comparator_queue_t value);

// Full control access.
void ADS1x1x_write_register(uint8_t i2c_address, uint8_t reg, uint16_t value);
uint16_t ADS1x1x_read_register(uint8_t i2c_address, uint8_t reg);
void ADS1x1x_set_config_bitfield(ADS1x1x_config_t *p_config, uint16_t value, uint16_t mask);

// Stubs to be implemented by you (the user).
// You must provide this function.
void soft_i2c_delay(void)
{
  usleep(5);
}

// You must provide this function.
void soft_i2c_sda_mode(uint8_t value)
{
  if (value==0) {
	mraa_gpio_dir(g_sda,MRAA_GPIO_IN);
  }else{
	mraa_gpio_dir(g_sda,MRAA_GPIO_OUT);
  }
}

// You must provide this function.
void soft_i2c_sda_write(uint8_t value)
{
	mraa_gpio_write(g_sda,value);
}

// You must provide this function.
uint8_t soft_i2c_sda_read(void)
{
  return mraa_gpio_read(g_sda);
}

// You must provide this function.
void soft_i2c_scl_write(uint8_t value)
{
	mraa_gpio_write(g_scl,value);
}


void soft_i2c_init(void);
void soft_i2c_start(void);
void soft_i2c_stop(void);
uint8_t soft_i2c_start_write(uint8_t address);
uint8_t soft_i2c_write(uint8_t value);
uint8_t soft_i2c_start_read(uint8_t address, unsigned int count);
unsigned int soft_i2c_available(void);
uint8_t soft_i2c_read(void);
void soft_i2c_write_bit(uint8_t bit);
uint8_t soft_i2c_read_bit(void);


// You must provide this function.
uint8_t ADS1x1x_i2c_start_write(uint8_t i2c_address)
{
  return soft_i2c_start_write(i2c_address);
}

// You must provide this function.
uint8_t ADS1x1x_i2c_write(uint8_t x)
{
  return soft_i2c_write(x);
}

// You must provide this function.
uint8_t ADS1x1x_i2c_start_read(uint8_t i2c_address, uint16_t bytes_to_read)
{
  return soft_i2c_start_read(i2c_address,bytes_to_read);
}

// You must provide this function.
uint8_t ADS1x1x_i2c_read(void)
{
  return soft_i2c_read();
}

// You must provide this function.
uint8_t ADS1x1x_i2c_stop(void)
{
  soft_i2c_stop();
  return 0;
}

/**************************************************************************/
/*!
    @brief  Writes 16 bits to the specified destination register.
*/
/**************************************************************************/
void ADS1x1x_write_register(uint8_t i2c_address, uint8_t reg, uint16_t value)
{
  ADS1x1x_i2c_start_write(i2c_address);
  ADS1x1x_i2c_write(reg);
  ADS1x1x_i2c_write((uint8_t)(value>>8));
  ADS1x1x_i2c_write((uint8_t)(value&0xff));
  ADS1x1x_i2c_stop();
}

/**************************************************************************/
/*!
    @brief  Read 16 bits from the specified destination register.
*/
/**************************************************************************/
uint16_t ADS1x1x_read_register(uint8_t i2c_address, uint8_t reg)
{
  uint16_t result = 0;
  ADS1x1x_i2c_start_write(i2c_address);
  ADS1x1x_i2c_write(reg);
  ADS1x1x_i2c_stop();
  ADS1x1x_i2c_start_read(i2c_address,2);
  result = ADS1x1x_i2c_read() << 8;
  result |= ADS1x1x_i2c_read();
  ADS1x1x_i2c_stop();
  return result;
}


/**************************************************************************/
/*!
    @brief  Initialise a user-provided ADS1X15 configuration structure.
            The user must provide a valid pointer to an empty
            ADS1x1x_config_t structure.
*/
/**************************************************************************/
uint8_t ADS1x1x_init(ADS1x1x_config_t *p_config, ADS1x1x_chip_t chip, uint8_t i2c_address, ADS1x1x_mux_t input, ADS1x1x_pga_t gain)
{
  uint8_t result = 0;

  if (p_config!=0)
  {
    // Set generic parameters.
    p_config->chip = chip;
    p_config->i2c_address = i2c_address;

    // Set configuration bits for default operation.
    p_config->config = 0;
    ADS1x1x_set_os(p_config,OS_SINGLE);
    ADS1x1x_set_multiplexer(p_config,input);
    ADS1x1x_set_pga(p_config,gain);
    ADS1x1x_set_mode(p_config,MODE_SINGLE_SHOT);
    if (p_config->chip==ADS1013 || p_config->chip==ADS1014 || p_config->chip==ADS1015)
    {
      ADS1x1x_set_data_rate(p_config,DATA_RATE_ADS101x_1600);
    }
    else
    {
      ADS1x1x_set_data_rate(p_config,DATA_RATE_ADS111x_128);
    }
    ADS1x1x_set_comparator_mode(p_config,COMPARATOR_MODE_TRADITIONAL);
    ADS1x1x_set_comparator_polarity(p_config,COMPARATOR_POLARITY_ACTIVE_LO);
    ADS1x1x_set_comparator_latching(p_config,COMPARATOR_NON_LATCHING);
    ADS1x1x_set_comparator_queue(p_config,COMPARATOR_QUEUE_NONE);
    
    result = 1;
  }

  return result;
}


/**************************************************************************/
/*!
    @brief  Start an ADC conversion cycle.
            The user must provide a valid pointer to a
            correctly filled ADS1x1x_config_t structure.
*/
/**************************************************************************/
void ADS1x1x_start_conversion(ADS1x1x_config_t *p_config)
{
  // Write configuration to the ADC.
  ADS1x1x_write_register(p_config->i2c_address,ADS1x1x_REG_POINTER_CONFIG,p_config->config);
}


/**************************************************************************/
/*!
    @brief  Read the ADC conversion result.
            The user must provide a valid pointer to a
            correctly filled ADS1x1x_config_t structure.
*/
/**************************************************************************/
int16_t ADS1x1x_read(ADS1x1x_config_t *p_config)
{
  // Read the conversion result.
  int16_t result = (int16_t)ADS1x1x_read_register(p_config->i2c_address,ADS1x1x_REG_POINTER_CONVERSION);
  // Adjust for ADC resolution if needed.
  if (p_config->chip==ADS1013 || p_config->chip==ADS1014 || p_config->chip==ADS1015)
  {
    result >>= 4;
  }
  return result;
}


void ADS1x1x_set_threshold_lo(ADS1x1x_config_t *p_config, uint16_t value)
{
  if (p_config->chip==ADS1013 || p_config->chip==ADS1014 || p_config->chip==ADS1015)
  {
    value <<= 4;
  }
  ADS1x1x_write_register(p_config->i2c_address,ADS1x1x_REG_POINTER_LO_THRESH,value);
}


void ADS1x1x_set_threshold_hi(ADS1x1x_config_t *p_config, uint16_t value)
{
  if (p_config->chip==ADS1013 || p_config->chip==ADS1014 || p_config->chip==ADS1015)
  {
    value <<= 4;
  }
  ADS1x1x_write_register(p_config->i2c_address,ADS1x1x_REG_POINTER_HI_THRESH,value);
}


void ADS1x1x_set_config_bitfield(ADS1x1x_config_t *p_config, uint16_t value, uint16_t mask)
{
  p_config->config &= ~mask;
  p_config->config |= (value & mask);
}


void ADS1x1x_set_os(ADS1x1x_config_t *p_config, ADS1x1x_os_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_OS_MASK);
}


void ADS1x1x_set_multiplexer(ADS1x1x_config_t *p_config, ADS1x1x_mux_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_MULTIPLEXER_MASK);
}


void ADS1x1x_set_pga(ADS1x1x_config_t *p_config, ADS1x1x_pga_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_PGA_MASK);
}


void ADS1x1x_set_mode(ADS1x1x_config_t *p_config, ADS1x1x_mode_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_MODE_MASK);
}


void ADS1x1x_set_data_rate(ADS1x1x_config_t *p_config, ADS1x1x_data_rate_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_DATA_RATE_MASK);
}


void ADS1x1x_set_comparator_mode(ADS1x1x_config_t *p_config, ADS1x1x_comparator_mode_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_COMPARATOR_MODE_MASK);
}


void ADS1x1x_set_comparator_polarity(ADS1x1x_config_t *p_config, ADS1x1x_comparator_polarity_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_COMPARATOR_POLARITY_MASK);
}


void ADS1x1x_set_comparator_latching(ADS1x1x_config_t *p_config, ADS1x1x_comparator_latching_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_COMPARATOR_LATCHING_MASK);
}


void ADS1x1x_set_comparator_queue(ADS1x1x_config_t *p_config, ADS1x1x_comparator_queue_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_COMPARATOR_QUEUE_MASK);
}

#define SOFT_I2C_INPUT  (0)
#define SOFT_I2C_OUTPUT  (1)
#define SOFT_I2C_LOW  (0)
#define SOFT_I2C_HIGH  (1)

static uint8_t soft_i2c_started;
static unsigned int soft_i2c_bytes_to_read;


void soft_i2c_init(void)
{
  // Make SDA an output.
  // SCL is supposed to be always an output.
  soft_i2c_sda_mode(SOFT_I2C_OUTPUT);
  // Put I2C bus in idle mode.
  soft_i2c_scl_write(SOFT_I2C_HIGH);
  soft_i2c_sda_write(SOFT_I2C_HIGH);
  // Initialise state variables.
  soft_i2c_started = 0;
  soft_i2c_bytes_to_read = 0;
}


void soft_i2c_start(void)
{
  soft_i2c_sda_mode(SOFT_I2C_OUTPUT);
  if (soft_i2c_started) 
  {
    // Already started, so send a restart condition.
    soft_i2c_sda_write(SOFT_I2C_HIGH);
    soft_i2c_scl_write(SOFT_I2C_HIGH);
    soft_i2c_delay();
  }
  soft_i2c_sda_write(SOFT_I2C_LOW);
  soft_i2c_delay();
  soft_i2c_scl_write(SOFT_I2C_LOW);
  soft_i2c_delay();
  soft_i2c_started = 1;
}


void soft_i2c_stop(void)
{
  soft_i2c_sda_mode(SOFT_I2C_OUTPUT);
  soft_i2c_sda_write(SOFT_I2C_LOW);
  soft_i2c_scl_write(SOFT_I2C_HIGH);
  soft_i2c_delay();
  soft_i2c_sda_write(SOFT_I2C_HIGH);
  soft_i2c_delay();
  soft_i2c_started = 0;
}


uint8_t soft_i2c_start_write(uint8_t address)
{
  soft_i2c_start();
  if (soft_i2c_write(address<<1)==0) return 0;
  return 1;
}


uint8_t soft_i2c_write(uint8_t value)
{
  uint8_t mask = 0x80;
  while (mask!=0) 
  {
    soft_i2c_write_bit((value&mask)!=0);
    mask >>= 1;
  }
  if (soft_i2c_read_bit()!=0) return 0;
  return 1;
}


uint8_t soft_i2c_start_read(uint8_t address, unsigned int count)
{
  soft_i2c_start();
  if (soft_i2c_write((address<<1)|0x01)==0)
  {
    soft_i2c_bytes_to_read = 0;
    return 0;
  }
  soft_i2c_bytes_to_read = count;
  return 1;
}


unsigned int soft_i2c_available(void)
{
  return soft_i2c_bytes_to_read;
}


uint8_t soft_i2c_read(void)
{
  uint8_t i;
  uint8_t value = 0;
  // Read 8 bits
  for (i=0; i<8; i++)
  {
    value = (value<<1) | soft_i2c_read_bit();
  }
  if (soft_i2c_bytes_to_read>1) 
  {
    // More bytes left to read - send an ACK.
    soft_i2c_write_bit(0);
    soft_i2c_bytes_to_read -= 1;
  } 
  else 
  {
    // Last byte - send the NACK and a stop condition.
    soft_i2c_write_bit(1);
    soft_i2c_stop();
    soft_i2c_bytes_to_read = 0;
  }
  return value;
}


void soft_i2c_write_bit(uint8_t bit)
{
  soft_i2c_sda_mode(SOFT_I2C_OUTPUT);
  soft_i2c_sda_write(bit);
  soft_i2c_delay();
  soft_i2c_scl_write(SOFT_I2C_HIGH);
  soft_i2c_delay();
  soft_i2c_scl_write(SOFT_I2C_LOW);
  soft_i2c_delay();
}


uint8_t soft_i2c_read_bit(void)
{
  soft_i2c_sda_mode(SOFT_I2C_INPUT);
  soft_i2c_sda_write(SOFT_I2C_HIGH);
  soft_i2c_scl_write(SOFT_I2C_HIGH);
  uint8_t bit = soft_i2c_sda_read();
  soft_i2c_delay();
  soft_i2c_scl_write(SOFT_I2C_LOW);
  soft_i2c_delay();
  return bit;
}

int mraa_nbext_adc_read(int index,int16_t * p_val)
{
	int adc=0x3000+index*0x1000;
	
	if(index>=ADC_MRAA_IO_MAX_INDEX || index<0 || p_val==0){
		return -1;
	}
	
	//init gpio & adc
	if(!g_adc_is_started){
		//init gpio
		g_sda= mraa_gpio_init(ADC_MRAA_IO_SDA);
		g_scl= mraa_gpio_init(ADC_MRAA_IO_SCL);
		if(g_sda==NULL || g_scl==NULL){
			printf("Fail to init the I2C gpio.\n");
			return -1;
		}
		mraa_gpio_dir(g_sda,MRAA_GPIO_OUT);
		mraa_gpio_dir(g_scl,MRAA_GPIO_OUT);

		//init ads1115
		if (ADS1x1x_init(&g_adc,ADS1115,ADS1x1x_I2C_ADDRESS_ADDR_TO_GND,MUX_SINGLE_0,PGA_4096)==0)
		{
		   printf("Could not initialise ADC structure.");
		}
		
		g_adc_is_started=1;
	}

	//read adc
	if(g_adc_is_started){
		ADS1x1x_set_multiplexer(&g_adc,(ADS1x1x_mux_t)adc);
		ADS1x1x_start_conversion(&g_adc);
		// Default sample rate is 128 samples/s, i.e. one sample every 7.8 ms.
		usleep(6000);
		// Display result as voltage (remember, the PGA was set to 4.096 V
		*p_val = ADS1x1x_read(&g_adc);
		return 0;
	}

	return -1;
}

int mraa_nbext_adc_read_float(int index,float * p_val)
{
	int16_t val=0;
	if(mraa_nbext_adc_read(index,&val)){
		return -1;
	}
	*p_val = (float)val*ADC_MRAA_VOLTAGE/ADC_MRAA_RANGE;
	return 0;	
}
