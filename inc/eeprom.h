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

#if defined(USE_24LC01)
#define EEPROM_24LC01 128
#endif
#if defined(USE_24LC02)
#define EEPROM_24LC02 256
#endif
#if defined(USE_24LC04)
#define EEPROM_24LC04 512
#endif
#if defined(USE_24LC08)
#define EEPROM_24LC08 1024
#endif
#if defined(USE_24LC16)
#define EEPROM_24LC16 2048
#endif
#if defined(USE_24LC32)
#define EEPROM_24LC32 4096
#endif
#if defined(USE_24LC64)
#define EEPROM_24LC64 8192
#endif
#if defined(USE_24LC128)
#define EEPROM_24LC128 16384
#endif
#if defined(USE_24LC256)
#define EEPROM_24LC256 32768
#endif
#if defined(USE_24LC512)
#define EEPROM_24LC512 65536
#endif
#if defined(USE_24LC1025)
#define EEPROM_24LC1025 131072
#endif

typedef struct
{
  I2C_HandleTypeDef *hi2c;      /* I2C peripheral connect with EEPROM */
  SemaphoreHandle_t i2c_mutex;  /* mutex for mutually exclusive I2C */
  GPIO_TypeDef *WP_port;        /* write protect port */
  uint16_t WP_pin;              /* write protect pin */
  uint8_t chip_select;          /* only for 24LCXX which XX > 16 */
  uint8_t page;                 /* EEPROM page size */
  uint32_t capacity;            /* EEPROM maximum capacity e.g. 24LC16 = 2000 byte */
}
eeprom;

Status eeprom_init(eeprom *device,
                   uint32_t type,
                   uint8_t chip_select,
                   I2C_HandleTypeDef *hi2c,
                   GPIO_TypeDef *WP_port,
                   uint16_t WP_pin);

void eeprom_deinit(eeprom *device);

Status eeprom_write(eeprom *device,
                    uint32_t address,
                    uint8_t *buffer,
                    uint32_t size,
                    uint32_t timeout);

Status eeprom_read(eeprom *device,
                   uint32_t address,
                   uint8_t *buffer,
                   uint32_t size,
                   uint32_t timeout);
#endif /* EEPROM_H */
