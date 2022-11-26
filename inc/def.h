#ifndef DEF_H
#define DEF_H

#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"

typedef enum
{
  DRIVER_SUCCESS = 0,
  DRIVER_ERROR,
  DRIVER_TIMEOUT,
  DRIVER_MUTEX_TIMEOUT,
}
Status;

#endif /* DEF_H */
