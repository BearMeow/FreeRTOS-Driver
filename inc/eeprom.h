#ifndef EEPROM_H
#define EEPROM_H

#include "def.h"

#define USE_24LC01
#define USE_24LC02
#define USE_24LC04
#define USE_24LC08
#define USE_24LC16
#define USE_24LC32
#define USE_24LC64
#define USE_24LC128
#define USE_24LC256
#define USE_24LC512
#define USE_24LC1025

typedef struct
{
	uint32_t type;               /* EEPROM type to operate e.g. EEPROM_24LC04 */
	I2C_HandleTypeDef *hi2c;     /* I2C peripheral connect with EEPROM */
	SemaphoreHandle_t i2c_mutex; /* mutex for mutually exclusive I2C */
	GPIO_TypeDef *WP_port;			 /* write protect port */
	uint16_t WP_pin;						 /* write protect pin */
}
eeprom;



#endif /* EEPROM_H */
