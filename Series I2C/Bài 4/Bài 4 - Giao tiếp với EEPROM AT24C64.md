# Bài 4: Giao tiếp với EEPROM AT24C64

**AT24C64** là một bộ nhớ **EEPROM** (Electrically Erasable Programmable Read-Only Memory) có khả năng lưu trữ dữ liệu, vẫn được giữ lại ngay cả khi mất nguồn điện. AT24C64 thuộc dòng bộ nhớ EEPROM có giao tiếp I2C.

Bài viết này sẽ hướng dẫn sử dụng STM32F103 để giao tiếp với AT24C64.

## Mục lục

- [Bài 4: Giao tiếp với EEPROM AT24C64](#bài-4-giao-tiếp-với-eeprom-at24c64)
	- [Mục lục](#mục-lục)
	- [Kiến thức cần có](#kiến-thức-cần-có)
	- [Mục tiêu bài học](#mục-tiêu-bài-học)
	- [AT24C64](#at24c64)
	- [Sơ đồ mạch điện](#sơ-đồ-mạch-điện)
	- [Project: Giao tiếp với EEPROM AT24C64 và in thời gian qua Serial](#project-giao-tiếp-với-eeprom-at24c64-và-in-thời-gian-qua-serial)
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

- Hiểu được cách đọc, ghi lên EEPROM AT24C64.

## AT24C64

AT24C64 là IC EEPROM, có dung lượng lưu trữ 64Kbit (8KB). IC sử dụng giao tiếp I2C, với tốc độ clock lên tới 1MHz. Điện áp hoạt động từ 1.7V đến 5.5V, khiến nó phù hợp với cả hệ thống 3.3V và 5V.

Trên thị trường có bán các loại module chứa AT24C64, và thường đi kèm với IC thời gian thực DS1307.

![alt text](<images/Screenshot 2024-09-13 at 22.47.11.png>)

Các chân của AT24C64:
- VCC: Chân cấp nguồn, từ 1.7V - 5.5V
- GND: Chân nối 0V
- SDA, SCL: Các chân của giao tiếp I2C
- WP: Write Protection - chân bảo vệ. Khi chân này ở mức cao, IC sẽ không cho phép ghi lên nửa sau của IC. Trên module, chân này không nối ra ngoài, mặc định nối 0V.
- A0, A1, A2: Các chân này sẽ thay đổi địa chỉ I2C của chip. Trên module, các chân này không nối ra ngoài, mặc định nối 0V (địa chỉ I2C là 0x50)

## Sơ đồ mạch điện

Cấp nguồn cho IC AT24C64 với 3.3V hoặc 5V. Nối các chân SCL (PB6) và SDA (PB7) của STM32F103 tương ứng với các chân SDA, SCL của IC.

![alt text](<images/Screenshot 2024-09-13 at 23.03.30.png>)

> Lưu ý: tương tự DS1307, khi dùng module sẽ có sẵn R1 và R2 nên không cần gắn các điện trở này.

## Project: Giao tiếp với EEPROM AT24C64 và in thời gian qua Serial

Dùng STM32F103 để ghi dòng chữ "hello" vào EEPROM, sau đó đọc lại và gửi qua UART để hiện thị trên máy tính.

### Tạo project, cấu hình I2C và UART

Tạo project mới trên STM32CubeIDE. Trong giao diện của STM32CubeMX, bật I2C1 (mode = I2C) và USART1 (mode = Asynchronous). Cấu hình mặc định. Khi đó các chân được sử dụng gồm:
- PB6 - I2C1_SCL
- PB7 - I2C1_SDA
- PA9 - USART1_TX
- PA10 - USART1_RX

![alt text](<images/Screenshot 2024-09-13 at 23.10.41.png>)

Sau đó lưu lại và để phần mềm tự động tạo code.

### Thêm thư viện

Thư viện giao tiếp với AT24C64 mình đã viết sẵn vào các file `at24c64.h` và `at24c64.c`. Thêm thư viện vào project như sau:

- `at24c64.h` thêm vào `Core/Inc`.
- `at24c64.c` thêm vào `Core/Src`.

### Một số hàm sử dụng

Các hàm và cấu trúc sử dụng:

```c++
/* Cấu trúc chứa thông tin địa chỉ và I2C handle cho mỗi EEPROM */
typedef struct
{
	I2C_HandleTypeDef *hi2c;
	uint16_t deviceAddress;
} AT24C64;

/* 
* Khởi tạo EEPROM.
* eeprom: con trỏ tới cấu trúc của EEPROM
* hi2c: con trỏ tới cấu trúc điều khiển I2C
* deviceAddress: địa chỉ của EEPROM
*/
void AT24C64_Init(AT24C64 *eeprom, I2C_HandleTypeDef *hi2c, uint8_t deviceAddress);

/*
* Ghi dữ liệu vào địa chỉ bất kỳ trong EEPROM
* eeprom: con trỏ tới cấu trúc của EEPROM
* MemAddress: địa chỉ muốn ghi vào
* pData: con trỏ chỉ tới dữ liệu muốn ghi
* Size: số byte dữ liệu muốn ghi
*/
void AT24C64_Write(AT24C64 *eeprom, uint16_t MemAddress, uint8_t *pData, uint16_t Size);

/*
* Đọc dữ liệu tại địa chỉ bất kỳ trong EEPROM
* eeprom: con trỏ tới cấu trúc của EEPROM
* MemAddress: địa chỉ muốn đọc ra
* pData: con trỏ chỉ tới nơi sẽ nhận dữ liệu
* Size: số byte dữ liệu muốn đọc
*/
void AT24C64_Read(AT24C64 *eeprom, uint16_t MemAddress, uint8_t *pData, uint16_t Size);

```

## Code

`main.c`:

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
#include "at24c64.h"

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
/* Khai báo một struct điều khiển AT24C64 */
AT24C64 eeprom;

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
	/* Khởi tạo eeprom */
	AT24C64_Init(&eeprom, &hi2c1, 0x50);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	/* msg1 sẽ được ghi vào EEPROM. msg2 là nơi chứa khi đọc dữ liệu về */
	char msg1[] = "hello";
	char msg2[10];
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		/* Ghi msg1 vào địa chỉ 0x10 của EEPROM, số lượng byte là 5 */
		AT24C64_Write(&eeprom, 0x10, (uint8_t*) msg1, 5);

		/* Đọc lại 5 ký tự ở 0x10 và lưu vào msg2 */
		AT24C64_Read(&eeprom, 0x10, (uint8_t*) msg2, 5);

		/* In lên  Serial*/
		HAL_UART_Transmit(&huart1, (uint8_t*) msg2, 5, 100);

		/* Xóa hết dữ liệu ở msg2 */
		memset(msg2, 0, sizeof(msg2));

		HAL_Delay(1000);
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
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* Thêm thư viện */
#include "at24c64.h"

/* USER CODE END Includes */
...
```

Khai báo biến `eeprom`:

```c++
...
/* USER CODE BEGIN PV */
/* Khai báo một struct điều khiển AT24C64 */
AT24C64 eeprom;

/* USER CODE END PV */
...
```

Khởi tạo EEPROM trong `main()`:

```c++
...
	/* USER CODE BEGIN 2 */
	/* Khởi tạo eeprom */
	AT24C64_Init(&eeprom, &hi2c1, 0x50);

	/* USER CODE END 2 */
...
```

Khối `while()`:

```c++
...
/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	/* msg1 sẽ được ghi vào EEPROM. msg2 là nơi chứa khi đọc dữ liệu về */
	char msg1[] = "hello";
	char msg2[10];
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		/* Ghi msg1 vào địa chỉ 0x10 của EEPROM, số lượng byte là 5 */
		AT24C64_Write(&eeprom, 0x10, (uint8_t*) msg1, 5);

		/* Đọc lại 5 ký tự ở 0x10 và lưu vào msg2 */
		AT24C64_Read(&eeprom, 0x10, (uint8_t*) msg2, 5);

		/* In lên  Serial*/
		HAL_UART_Transmit(&huart1, (uint8_t*) msg2, 5, 100);

		/* Xóa hết dữ liệu ở msg2 */
		memset(msg2, 0, sizeof(msg2));

		HAL_Delay(1000);
	}
	/* USER CODE END 3 */
...
```

## Kết quả

Mở ứng dụng giao tiếp với cổng Serial nối với STM32F103, kết nối với baudrate 115200.

![alt text](<images/Screenshot 2024-09-13 at 23.53.07.png>)

## Câu hỏi và bài tập

### Câu hỏi

1. AT24C64 là bộ nhớ có dung lượng 8KByte. Địa chỉ tối đa có thể truy cập ?

### Bài tập

1. Tạo một biến. Mỗi lần lưu vào EEPROM thì tăng biến thêm 1 đơn vị. Sau đó đọc biến đó từ EEPROM, in lên Serial, rồi lặp lại.
2. Tạo một cấu trúc gồm 2 số nguyên kiểu `int` (`uint32_t`): nhiệt độ và độ ẩm. Lưu vào EEPROM rồi đọc ra. Gợi ý: ép kiểu con trỏ tới cấu trúc thành con trỏ `uint8_t *` mỗi khi đọc ghi. Dùng hàm `sizeof()` để trả về kích thước (số byte) của cấu trúc.
3. (Khó) Đọc cảm biến nhiệt độ, độ ẩm DHT11, lưu vào EEPROM mỗi 10s. Đọc đủ 3 lần thì in hết dữ liệu đã lưu lên Serial.

## Tài liệu tham khảo

[1] STMicroelectronics, "Medium-density performance line Arm®-based 32-bit MCU with 64 or 128 KB Flash, USB, CAN, 7 timers, 2 ADCs, 9 com. interfaces", STM32F103Cx8/B Datasheet, Sep. 2023.

[2] Microchip, "I2C-Compatible (Two-Wire) Serial EEPROM 64-Kbit (8,192 x 8)", AT24C64D Datasheet, Dec. 2020