/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_cdc_acm.h
  * @author  MCD Application Team
  * @brief   USBX Device CDC ACM interface header file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UX_DEVICE_CDC_ACM_H__
#define __UX_DEVICE_CDC_ACM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ux_api.h"
#include "ux_device_class_cdc_acm.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
VOID USBD_CDC_ACM_Activate(VOID *cdc_acm_instance);
VOID USBD_CDC_ACM_Deactivate(VOID *cdc_acm_instance);
VOID USBD_CDC_ACM_ParameterChange(VOID *cdc_acm_instance);

/* USER CODE BEGIN EFP */
VOID usbx_cdc_acm_read_thread_entry(ULONG thread_input);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define USB_UART_BAUDRATE	9600
#define MIN_BAUDRATE		9600
#define VCP_WORDLENGTH8		8
#define APP_RX_DATA_SIZE	64
#define APP_TX_DATA_SIZE	64

#define START_CHAR			0x24
#define SLEEP_CMD			0x1
#define WAKE_CMD			0x2
#define TEST_SENSORS_CMD	0x3
#define READ_SENSOR_CMD		0x4
#define WRITE_SENSOR_CMD	0x5
#define SIM_CMD				0x6

/* USER CODE END PD */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif  /* __UX_DEVICE_CDC_ACM_H__ */
