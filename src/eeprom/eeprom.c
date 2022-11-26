#include "eeprom.h"

Status eeprom_init(eeprom *device,
                   uint32_t type,
                   uint8_t chip_select,
                   I2C_HandleTypeDef *hi2c,
                   GPIO_TypeDef *WP_port,
                   uint16_t WP_pin)
{
  switch(type)  /* determine page and capacity by EERPROM device type */
  {
#if defined(USE_24LC01)
  case EEPROM_24LC01:
    device->page = 8;
    device->capacity = EEPROM_24LC01;
    break;
#endif
#if defined(USE_24LC02)
  case EEPROM_24LC02:
    device->page = 8;
    device->capacity = EEPROM_24LC02;
    break;
#endif
#if defined(USE_24LC04)
  case EEPROM_24LC04:
    device->page = 16;
    device->capacity = EEPROM_24LC04;
    break;
#endif
#if defined(USE_24LC08)
  case EEPROM_24LC08:
    device->page = 16;
    device->capacity = EEPROM_24LC08;
    break;
#endif
#if defined(USE_24LC16)
  case EEPROM_24LC16:
    device->page = 16;
    device->capacity = EEPROM_24LC16;
    break;
#endif
#if defined(USE_24LC32)
  case EEPROM_24LC32:
    device->page = 32;
    device->capacity = EEPROM_24LC32;
    break;
#endif
#if defined(USE_24LC64)
  case EEPROM_24LC64:
    device->page = 32;
    device->capacity = EEPROM_24LC64;
    break;
#endif
#if defined(USE_24LC128)
  case EEPROM_24LC128:
    device->page = 64;
    device->capacity = EEPROM_24LC128;
    break;
#endif
#if defined(USE_24LC256)
  case EEPROM_24LC256:
    device->page = 64;
    device->capacity = EEPROM_24LC256;
    break;
#endif
#if defined(USE_24LC512)
  case EEPROM_24LC512:
    device->page = 128;
    device->capacity = EEPROM_24LC512;
    break;
#endif
#if defined(USE_24LC1025)
  case EEPROM_24LC1025:
    device->page = 128;
    device->capacity = EEPROM_24LC1025;
    break;
#endif
  default:
    return DRIVER_ERROR;  /* unknown or undefined EEPROM type */
  }

  if(device->i2c_mutex == NULL)
  {
    return DRIVER_ERROR;  /* heap has no enough memory to allocate */
  }

  device->i2c_mutex = xSemaphoreCreateMutex();
  device->hi2c = hi2c;
  device->WP_port = WP_port;
  device->WP_pin = WP_pin;
  device->chip_select = chip_select;

  return DRIVER_SUCCESS;
}

void eeprom_deinit(eeprom *device)
{
  vSemaphoreDelete(device->i2c_mutex);
}

Status eeprom_write(eeprom *device,
                    uint32_t address,
                    uint8_t *buffer,
                    uint32_t size,
                    uint32_t timeout)
{
  if(xSemaphoreTake(device->i2c_mutex, timeout) == pdFALSE)
  {
    /* can not get peripheral before timeout */
    return DRIVER_MUTEX_TIMEOUT;
  }

  xSemaphoreGive(device->i2c_mutex);
}
