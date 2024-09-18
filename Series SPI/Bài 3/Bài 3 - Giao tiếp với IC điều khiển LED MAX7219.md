# Bài 3: Giao tiếp với IC điều khiển LED MAX7219

**MAX7219** là IC điều khiển LED sử dụng giao tiếp truyền thông nối tiếp (SPI) để điều khiển tối đa 64 đèn LED. Mạch được thiết kế để điều khiển LED 7 đoạn, LED ma trận 8x8, LED thanh.

Bài viết này hướng dẫn cách giao tiếp với **MAX7219** dùng SPI của STM32F103.

## Mục lục
- [Bài 3: Giao tiếp với IC điều khiển LED MAX7219](#bài-3-giao-tiếp-với-ic-điều-khiển-led-max7219)
	- [Mục lục](#mục-lục)
	- [Kiến thức cần có](#kiến-thức-cần-có)
	- [Mục tiêu bài học](#mục-tiêu-bài-học)
	- [MAX7219](#max7219)
	- [Sơ đồ mạch điện](#sơ-đồ-mạch-điện)
	- [Project: Hiển thị số trên LED 7 đoạn dùng MAX7219](#project-hiển-thị-số-trên-led-7-đoạn-dùng-max7219)
		- [Tạo project mới, cấu hình SPI và GPIO](#tạo-project-mới-cấu-hình-spi-và-gpio)
		- [Thêm thư viện](#thêm-thư-viện)
		- [Các hàm sử dụng](#các-hàm-sử-dụng)
		- [Code](#code)
	- [Câu hỏi và bài tập](#câu-hỏi-và-bài-tập)
		- [Câu hỏi](#câu-hỏi)
		- [Bài tập](#bài-tập)
	- [Tài liệu tham khảo](#tài-liệu-tham-khảo)

## Kiến thức cần có

- Series Cơ bản
- Bài 1: Giới thiệu về giao thức SPI

## Mục tiêu bài học

- Học được cách in số lên LED 7 đoạn sử dụng IC MAX7219

## MAX7219

MAX7219 là IC điều khiển LED 7 đoạn (lên tới 8 ký tự), LED ma trận 8x8 (lên tới hai ma trận), hoặc điều khiển 64 LED đơn. MAX7219 sử dụng giao thức truyền thông nối tiếp tương thích với SPI.

Tham khảo [MAX7219 datasheet](MAX7219-MAX7221.pdf).

![alt text](<images/Screenshot 2024-09-16 at 20.36.36.png>)

Trên thị trường thường bán module kết hợp LED 7 đoạn và MAX7219. Các chân gồm:

- VCC: Chân nguồn. Cấp nguồn từ 4.0V - 5.5V.
- GND: Chân nguồn. Nối 0V.
- DIN: Chân dữ liệu ngõ vào của SPI
- CS: Chân CS của SPI
- CLK: Chân CLK của SPI
- DOUT: Chân dữ liệu ngõ ra. Dùng để kết nối nhiều MAX7219 với nhau (DOUT nối với DIN của IC kế)

![alt text](<images/Screenshot 2024-09-16 at 20.38.36.png>)

## Sơ đồ mạch điện

Để sử dụng MAX7219, ta mắc mạch điện như sau:

- PB15 (MOSI) <---> DIN
- PB13 (SCK) <---> CLK
- PB1 (CS) <---> CS

![alt text](<images/Screenshot 2024-09-16 at 22.45.40.png>)

## Project: Hiển thị số trên LED 7 đoạn dùng MAX7219

### Tạo project mới, cấu hình SPI và GPIO

Tạo project mới trên STM32CubeIDE. Trong giao diện của STM32CubeMX, bật SPI2 bằng cách chọn Mode là Half-Duplex Master. Các thông số còn lại để mặc định:

![alt text](<images/Screenshot 2024-09-16 at 14.55.21.png>)

Chọn chân PB1, chọn GPIO_Output. Trong bảng cấu hình của GPIO (System Core > GPIO), chọn chân PB1, sau đó chọn GPIO output level thành HIGH. Mục đích để khi vừa khởi tạo GPIO, ngõ ra của chân PB1 luôn ở mức 1:

![alt text](<images/Screenshot 2024-09-16 at 22.49.34.png>)

Lưu lại và tạo code.

### Thêm thư viện

Thêm `max7219.h` vào `Core/Inc`. Thêm `max7219.c` vào `Core/Src`.

### Các hàm sử dụng

```c++
/* Cấu trúc điều khiển MAX7219 */
typedef struct
{
	GPIO_TypeDef *csPinPort;
	uint16_t csPin;
	SPI_HandleTypeDef *hspi;
} MAX7219;

/* 
* Khởi tạo MAX7219 
* hspi: con trỏ tới cấu trúc điều khiển SPI
* csPinPort: cổng GPIO của chân CS
* csPin: chân CS
*/
void MAX7219_Init(MAX7219 *dev, SPI_HandleTypeDef *hspi, GPIO_TypeDef *csPinPort, uint16_t csPin);

/*
* In một chữ số
* position: vị trí. Chọn từ DIGIT_1, DIGIT_2,..., DIGIT_7
* number: chữ số muốn in
*/
void MAX7219_PrintDigit(MAX7219 *dev, MAX7219_Digits position, MAX7219_Numeric number);

/* 
* In số
* number: số muốn in
*/
void MAX7219_PrintNumber(MAX7219 *dev, uint64_t number);

/* Thay đổi độ sáng */
void MAX7219_ChangeIntensity(MAX7219 *dev, uint8_t intensity);

/* Xóa màn hình */
void MAX7219_Clear(MAX7219 *dev);
```

### Code
Code trong `main.c`:

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
#include "max7219.h"

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
SPI_HandleTypeDef hspi2;

/* USER CODE BEGIN PV */
/* Khai báo biến max7219 */
MAX7219 max7219;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
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
	MX_SPI2_Init();
	/* USER CODE BEGIN 2 */

	/* Khởi tạo max7219 sử dụng SPI2 và PB1 làm chân CS */
	MAX7219_Init(&max7219, &hspi2, GPIOB, GPIO_PIN_1);

	/* Đặt độ sáng vừa phải */
	MAX7219_ChangeIntensity(&max7219, 0x08);

	/* In số 12345678 lên LED 7 đoạn */
	MAX7219_PrintNumber(&max7219, 12345678);

	HAL_Delay(5000);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		/* Đoạn code in số a tăng dần từ 0 */
		static int a = 0;
		MAX7219_PrintNumber(&max7219, a);
		HAL_Delay(200);
		a++;
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief SPI2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI2_Init(void)
{

	/* USER CODE BEGIN SPI2_Init 0 */

	/* USER CODE END SPI2_Init 0 */

	/* USER CODE BEGIN SPI2_Init 1 */

	/* USER CODE END SPI2_Init 1 */
	/* SPI2 parameter configuration*/
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER;
	hspi2.Init.Direction = SPI_DIRECTION_1LINE;
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI2_Init 2 */

	/* USER CODE END SPI2_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

	/*Configure GPIO pin : PB1 */
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
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
#include "max7219.h"

/* USER CODE END Includes */
...
```

Khai báo biến:
```c++
...
/* USER CODE BEGIN PV */
/* Khai báo biến max7219 */
MAX7219 max7219;

/* USER CODE END PV */
...
```

Trong hàm `main()`:

```c++
...
/* USER CODE BEGIN 2 */

/* Khởi tạo max7219 sử dụng SPI2 và PB1 làm chân CS */
MAX7219_Init(&max7219, &hspi2, GPIOB, GPIO_PIN_1);

/* Đặt độ sáng vừa phải */
MAX7219_ChangeIntensity(&max7219, 0x08);

/* In số 12345678 lên LED 7 đoạn */
MAX7219_PrintNumber(&max7219, 12345678);

HAL_Delay(5000);

/* USER CODE END 2 */
...
```

Trong khối `while()`:

```c++
...
/* Infinite loop */
/* USER CODE BEGIN WHILE */
while (1)
{
	/* USER CODE END WHILE */

	/* USER CODE BEGIN 3 */
	/* Đoạn code in số a tăng dần từ 0 */
	static int a = 0;
	MAX7219_PrintNumber(&max7219, a);
	HAL_Delay(200);
	a++;
}
/* USER CODE END 3 */
...
```

## Câu hỏi và bài tập

### Câu hỏi

1. MAX7219 sử dụng giao tiếp gì?
2. MAX7219 có thể điều khiển tối đa bao nhiêu LED ?

### Bài tập

1. In số 0 chạy từ trái sang phải (từ chữ số thứ 8 về chữ số thứ 1) rồi lặp lại.
2. (Khó) In số thập phân lên LED 7 đoạn. Gợi ý: Tách thành phần nguyên và phần thập phân, in từng phần lên màn hình. Sử dụng hàm `MAX7219_PrintDot(&max7219, position, 1)` để in dấu chấm tại `position`;

## Tài liệu tham khảo

[1] STMicroelectronics, "Medium-density performance line Arm®-based 32-bit MCU with 64 or 128 KB Flash, USB, CAN, 7 timers, 2 ADCs, 9 com. interfaces", STM32F103Cx8/B Datasheet, Sep. 2023.

[2] Maxim Integrated, "Serially Interfaced, 8-Digit LED Display Drivers", MAX7219-MAX7221 Datasheet, Rev August. 2021