#include "eeprom.h"

#define ADDR_PATTERN 0x50  /* 1010XXX */
#define ADDR_MASK    0x78  /* 1111000 */

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
                    TickType_t timeout)
{
  uint8_t i2c_addr, write_n, bound_1st;
  uint16_t mem_addr, mem_format;
  uint32_t remaining = size;
  TickType_t tick = xTaskGetTickCount();

  if(address + size >= device->capacity)
  {
    /* memory space to write exceed EEPROM capacity */
    return DRIVER_ERROR;
  }

  if(xSemaphoreTake(device->i2c_mutex, timeout) == pdFALSE)
  {
    /* can not get peripheral before timeout */
    return DRIVER_MUTEX_TIMEOUT;
  }

  /* upper bound of page where address locate */
  bound_1st = ((address / device->page) +1) * device->page;

  if(address + remaining > bound_1st)
  {
    /* address + write buffer size exceed page of address */
    write_n = bound_1st - address;
  }
  else
  {
    /* address + write buffer size in page of address */
    write_n = remaining;
  }


  do
  {
    if(device->capacity <= EEPROM_24LC16)
    {
      /* address high byte combine with control code 1010 */
      i2c_addr = ((uint8_t)(address >> 8) & (~ADDR_MASK)) | ADDR_PATTERN;
      /* address low byte */
      mem_addr = (uint8_t)address;
      mem_format = I2C_MEMADD_SIZE_8BIT;
    }
    else
    {
      /* chip select bit combine with control code 1010 */
      i2c_addr = (device->chip_select & (~ADDR_MASK)) | ADDR_PATTERN;
      if(device->capacity == EEPROM_24LC1025)
      {
        /* 24LC1025 I2C address format = 1010[B0][A1][A0] */
        i2c_addr |= (uint8_t)(((address >> 16) & 0x01) << 2);
      }
      /* address high + low byte */
      mem_addr = (uint16_t)address;
      mem_format = I2C_MEMADD_SIZE_16BIT;
    }

    while(HAL_I2C_Master_Transmit(device->hi2c, i2c_addr, NULL, 0, HAL_MAX_DELAY) != HAL_OK)
    {
      /* polling to check EEPROM has no write cycle not complete yet */
    }

    if(HAL_I2C_Mem_Write_DMA(&hi2c, i2c_addr << 1, mem_addr, mem_format, buffer, write_n) != HAL_OK)
    {
      xSemaphoreGive(device->i2c_mutex);
      return DRIVER_ERROR;
    }


    address += write_n;
    buffer += write_n;
    remaining -= write_n;

    if(remaining >= device->page)
    {
      write_n = device->page;
    }
    else
    {
      write_n = remaining;
    }
  }
  while(remaining > 0);

  xSemaphoreGive(device->i2c_mutex);

  return DRIVER_SUCCESS;
}

