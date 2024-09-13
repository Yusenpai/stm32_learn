# Bài 3 - Giao tiếp với module thời gian thực DS1307

**DS1307** là IC đồng hồ thời gian thực dùng để cung cấp thông tin về thời gian hiện tại (giờ phút giây, ngày tháng) và có khả năng duy trì dữ liệu này ngay cả khi mất nguồn bằng cách sử dụng pin dự phòng. Bài này sẽ hướng dẫn sử dụng STM32F103 để giao tiếp với IC DS1307.

## Mục lục

- [Bài 3 - Giao tiếp với module thời gian thực DS1307](#bài-3---giao-tiếp-với-module-thời-gian-thực-ds1307)
	- [Mục lục](#mục-lục)
	- [Kiến thức cần có](#kiến-thức-cần-có)
	- [Mục tiêu bài học](#mục-tiêu-bài-học)
	- [DS1307](#ds1307)
	- [Sơ đồ mạch điện](#sơ-đồ-mạch-điện)
	- [Project: Giao tiếp với DS1307 và in thời gian qua Serial](#project-giao-tiếp-với-ds1307-và-in-thời-gian-qua-serial)
		- [Tạo project, cấu hình I2C và UART](#tạo-project-cấu-hình-i2c-và-uart)
		- [Thêm thư viện](#thêm-thư-viện)
		- [Một số hàm sử dụng](#một-số-hàm-sử-dụng)
		- [Code](#code)
		- [Kết quả](#kết-quả)
	- [Câu hỏi và bài tập](#câu-hỏi-và-bài-tập)
		- [Câu hỏi](#câu-hỏi)
		- [Bài tập](#bài-tập)
	- [Tài liệu tham khảo](#tài-liệu-tham-khảo)


## Kiến thức cần có

- Bài 1: Giới thiệu về giao thức I2C
- Series cơ bản

## Mục tiêu bài học

- Hiểu cách đọc và thay đổi (ghi) thời gian lên IC DS1307.

## DS1307

**DS1307** là IC đồng hồ thời gian thực dùng để cung cấp thông tin về thời gian hiện tại (giờ phút giây, ngày tháng) và có khả năng duy trì dữ liệu này ngay cả khi mất nguồn bằng cách sử dụng pin dự phòng.

![ds1307_module](https://cdn-shop.adafruit.com/970x728/3296-03.jpg)

IC này sử dụng giao thức truyền thông **I2C** để đọc/ghi dữ liệu. Trên thị trường có nhiều loại module sử dụng DS1307, nhìn chung chúng đều có các chân:

- 5V hoặc VCC hoặc VDD: chân cấp nguồn cho mạch. Nối với nguồn 5V.
- GND hoặc VSS: chân nối đất (0V).
- SDA: chân SDA của giao thức I2C.
- SCL: chân SCL của giao thức I2C.
- SQW hoặc SQWE: chân này là tùy chọn, có thể lập trình để tạo ra xung tần số 1Hz, 4.096kHz, 8.192kHz, 32.768kHz.

> Lưu ý: DS1307 được thiết kế để hoạt động với 5V. Cấp nguồn thấp hơn, IC có thể không hoạt động.

##  Sơ đồ mạch điện

Sơ đồ nối dây giữa STM32F103 và module DS1307:

![schematic](./images/schematic.png)

> Lưu ý: điện trở R1, R2 là điện trở kéo lên trong giao thức I2C. Phần lớn các module đều có gắn điện trở này, do đó có thể bỏ qua.

## Project: Giao tiếp với DS1307 và in thời gian qua Serial

Dùng STM32F103 để đọc thời gian và in thời gian qua UART để hiện thị trên máy tính.

### Tạo project, cấu hình I2C và UART

Tạo project mới trên STM32CubeIDE. Trong giao diện của STM32CubeMX, bật I2C1 (mode = I2C) và USART1 (mode = Asynchronous). Cấu hình mặc định. Khi đó các chân được sử dụng gồm:
- PB6 - I2C1_SCL
- PB7 - I2C1_SDA
- PA9 - USART1_TX
- PA10 - USART1_RX

![alt text](./images/config.png)

Sau đó lưu lại và để phần mềm tự động tạo code.

### Thêm thư viện

Thư viện giao tiếp với DS1307 mình đã viết sẵn vào các file *ds1307.h* và *ds1307.c*. Thêm thư viện vào project như sau:

- *ds1307.h* thêm vào *Core/Inc*.
- *ds1307.c* thêm vào *Core/Src*.

### Một số hàm sử dụng

Các hàm và cấu trúc sử dụng trong thư viện DS1307:

```c++
/* Cấu trúc dùng để lưu thời gian */
typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
	I2C_HandleTypeDef *i2cHandle;
} DS1307;

/* Khởi tạo thư viện */
void DS1307_Init(DS1307 *dev, I2C_HandleTypeDef *hi2c);

/* Đọc thời gian */
void DS1307_ReadTime(DS1307 *dev);

/* Đặt thời gian */
void DS1307_SetTime(DS1307 *dev);

/* Bật/Tắt chức năng tạo xung ở chân SQW */
void DS1307_SqwEnable(DS1307 *dev, uint8_t enable);
```

### Code
main.c:
```c++
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* Thêm thư viện */
#include "ds1307.h"
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Khai báo biến lưu thời gian */
DS1307 ds1307;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */

	/* Khởi tạo thư viện DS1307 */
	DS1307_Init(&ds1307, &hi2c1);

	/* Lưu thời gian muốn thay đổi vào biến */
	ds1307.hours = 15;
	ds1307.minutes = 30;
	ds1307.seconds = 25;
	ds1307.date = 8;
	ds1307.month = 8;
	ds1307.year = 24;

	/* Sửa thời gian và lưu vào DS1307 */
	DS1307_SetTime(&ds1307);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	char msg[21];

	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		/* Đọc thời gian */
		DS1307_ReadTime(&ds1307);

		sprintf(msg, "%02d:%02d:%02d %02d/%02d/20%02d\n", ds1307.hours,
				ds1307.minutes, ds1307.seconds, ds1307.date, ds1307.month,
				ds1307.year);

		/* In lên Serial */
		HAL_UART_Transmit(&huart1, (uint8_t *)msg, 20, 100);

		HAL_Delay(500);
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
```

Giải thích:

Thêm thư viện:
```c++
...
/* USER CODE BEGIN Includes */
/* Thêm thư viện */
#include "ds1307.h"
#include <stdio.h>

/* USER CODE END Includes */
...
```

Khai báo biến `ds1307`:
```c++
...
/* USER CODE BEGIN PV */
/* Khai báo biến lưu thời gian */
DS1307 ds1307;

/* USER CODE END PV */
...
```

Trong hàm `main()`:

```c++
...
/* USER CODE BEGIN 2 */

/* Khởi tạo thư viện DS1307 */
DS1307_Init(&ds1307, &hi2c1);

/* Lưu thời gian muốn thay đổi vào biến */
ds1307.hours = 15;
ds1307.minutes = 30;
ds1307.seconds = 25;
ds1307.date = 8;
ds1307.month = 8;
ds1307.year = 24;

/* Sửa thời gian và lưu vào DS1307 */
DS1307_SetTime(&ds1307);

/* USER CODE END 2 */
...
```

Trong khối `while()`:
```c++
...
/* Infinite loop */
/* USER CODE BEGIN WHILE */
char msg[21]; //buffer chứa chuỗi ký tự gửi lên Serial

while (1) {
	/* USER CODE END WHILE */

	/* USER CODE BEGIN 3 */
	/* Đọc thời gian */
	DS1307_ReadTime(&ds1307);

	/* Ghi thời gian, ngày tháng vào msg  */
	sprintf(msg, "%02d:%02d:%02d %02d/%02d/20%02d\n", ds1307.hours,
			ds1307.minutes, ds1307.seconds, ds1307.date, ds1307.month,
			ds1307.year);

	/* In msg lên Serial */
	HAL_UART_Transmit(&huart1, (uint8_t *)msg, 20, 100);

	HAL_Delay(500);
}
/* USER CODE END 3 */
...
```

### Kết quả

Mở ứng dụng giao tiếp với cổng Serial bất kỳ, kết nối với baudrate 115200.

![alt text](./images/result.png)

## Câu hỏi và bài tập

### Câu hỏi

1. IC DS1307 được dùng để làm gì? Giao thức dùng để giao tiếp với DS1307
2. Vì sao có thể bỏ điện trở kéo lên khi nối dây tới module DS1307?
3. Chân SQW của DS1307 để làm gì?

### Bài tập

1. In thêm ngày trong tuần (thứ Hai, thứ Ba,...) lên Serial
> Gợi ý: sử dụng `ds1307.day`, có giá trị từ 1-7. Gán ngày trong tuần tương ứng với số.
2. (Khó) Dùng 2 macro `__TIME__` và `__DATE__` để **lấy thời gian tại thời điểm biên dịch**, xử lý và lưu vào DS1307.
3. (Khó) Đọc datasheet và tự viết một thư viện giao tiếp với DS1307.

## Tài liệu tham khảo

[1] STMicroelectronics, "Medium-density performance line Arm®-based 32-bit MCU with 64 or 128 KB Flash, USB, CAN, 7 timers, 2 ADCs, 9 com. interfaces", STM32F103Cx8/B Datasheet, Sep. 2023.

[2] Maxim Integrated, "64 x 8, Serial, I2C Real-Time Clock", DS1307 Datasheet, 03/2015.
