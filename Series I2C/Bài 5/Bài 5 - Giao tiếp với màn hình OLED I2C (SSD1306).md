# Bài 5: Giao tiếp với màn hình OLED I2C (SSD1306)

**Màn hình OLED 0.96inch** là một màn hình nhỏ, có độ tương phản cao, thường được sử dụng hiển thị trong các mạch điện tử. Nó sử dụng công nghệ **OLED** - mỗi điểm ảnh là một LED nhỏ, do đó màn hình không cần đèn chiếu sau, giúp tiết kiệm năng lượng và góc nhìn tốt hơn so với màn hình LCD truyền thống.

Màn hình này sử dụng IC SSD1306 để điều khiển. Bài viết này sẽ hướng dẫn giao tiếp với SSD1306 qua giao tiếp I2C.

## Mục lục
- [Bài 5: Giao tiếp với màn hình OLED I2C (SSD1306)](#bài-5-giao-tiếp-với-màn-hình-oled-i2c-ssd1306)
	- [Mục lục](#mục-lục)
	- [Kiến thức cần có](#kiến-thức-cần-có)
	- [Mục tiêu bài học](#mục-tiêu-bài-học)
	- [Màn hình OLED 0.96 inch](#màn-hình-oled-096-inch)
	- [Sơ đồ mạch điện](#sơ-đồ-mạch-điện)
	- [Project: In chữ lên màn hình OLED](#project-in-chữ-lên-màn-hình-oled)
		- [Tạo project và cấu hình I2C](#tạo-project-và-cấu-hình-i2c)
		- [Thêm thư viện](#thêm-thư-viện)
		- [Một số hàm sử dụng](#một-số-hàm-sử-dụng)
		- [Code](#code)
		- [Kết quả](#kết-quả)
	- [Câu hỏi và bài tập](#câu-hỏi-và-bài-tập)
		- [Câu hỏi](#câu-hỏi)
		- [Bài tập](#bài-tập)
	- [Tài liệu tham khảo](#tài-liệu-tham-khảo)

## Kiến thức cần có

- Bài 1: Giới thiệu về giao thức I2C.

## Mục tiêu bài học

- Hiểu được cách in chữ, vẽ pixel lên màn hình OLED.
  
## Màn hình OLED 0.96 inch

Đây là màn hình có kích thước rất bé. Kích thước đường chéo của màn hình là 0.96 inch. Màn hình có 128x64 điểm ảnh, mỗi điểm ảnh là một LED. Mỗi LED chỉ sáng được 1 màu, tuy nhiên có nhưng phiên bản sử dụng led RGB để hiển thị được màu sắc.

![alt text](<images/Screenshot 2024-09-14 at 15.46.22.png>)

Màn hình được điều khiển bằng IC SSD1306, sử dụng giao tiếp I2C. Module OLED có các chân:

- VCC/VDD: Chân cấp nguồn. Nguồn 3.3V - 5V
- GND: Chân cấp nguồn. Nối 0V
- SCL/SCK và SDA: 2 chân của giao tiếp I2C

## Sơ đồ mạch điện

Sơ đồ kết nối như hình dưới:

![alt text](<images/Screenshot 2024-09-14 at 15.53.02.png>)

> Lưu ý: có thể bỏ qua R1 và R2 do trên module đã có sẵn những điện trở này.

## Project: In chữ lên màn hình OLED

Project này sử dụng thư viện để in chữ và vẽ hình lên màn hình OLED.

### Tạo project và cấu hình I2C

Tạo project mới trên STM32CubeIDE. Trong giao diện của STM32CubeMX, bật I2C1 (mode = I2C)Cấu hình mặc định. Khi đó các chân được sử dụng gồm:
- PB6 - I2C1_SCL
- PB7 - I2C1_SDA

![alt text](<images/Screenshot 2024-09-14 at 16.01.54.png>)

### Thêm thư viện

Thư viện SSD1306 gồm 2 phần: 
- `ssd1306.h` và `ssd1306.c`: chứa các hàm và cấu trúc để giao tiếp với SSD1306
- `fonts.h` và `fonts.c`: chứa các font mẫu dùng để in chữ

Thêm thư viện vào các đường dẫn như sau:
- `ssd1306.h` và `fonts.h` thêm vào `Core/Inc`
- `ssd1306.c` và `fonts.c` thêm vào `Core/Src`

### Một số hàm sử dụng

Trong `ssd1306.h`:
```c++
/* Cấu trúc dùng để điều khiển oled */
typedef struct
{
    uint16_t currentX;
    uint16_t currentY;
    uint8_t inverted;
    uint8_t initialized;
    I2C_HandleTypeDef *hi2c;
    uint8_t buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
} SSD1306;

/* Khởi tạo oled */
void SSD1306_Init(SSD1306 *dev, I2C_HandleTypeDef *hi2c);

/* Phủ kín màn hình bằng màu trắng hoặc đen */
void SSD1306_Fill(SSD1306 *dev, SSD1306_COLOR color);

/* Cập nhật lên oled. Sau mỗi lần vẽ hoặc in chữ, phải gọi hàm này thì oled mới hiển thị */
void SSD1306_UpdateScreen(SSD1306 *dev);

/* Vẽ 1 điểm ảnh tại tọa độ x, y */
void SSD1306_DrawPixel(SSD1306 *dev, uint8_t x, uint8_t y, SSD1306_COLOR color);

/* Vẽ một chuỗi ký tự tại con trỏ vẽ */
char SSD1306_WriteString(SSD1306 *dev, const char *str, FontDef Font, SSD1306_COLOR color);

/* Đặt con trỏ vẽ ở tọa độ x, y */
void SSD1306_SetCursor(SSD1306 *dev, uint8_t x, uint8_t y);
```

Trong `fonts.h`:
```c++
...
/* Kích thước chữ cái 7x10 pixel */
extern FontDef Font_7x10;

/* Kích thước chữ cái 11x18 pixel */
extern FontDef Font_11x18;

/* Kích thước chữ cái 16x26 pixel */
extern FontDef Font_16x26;
...
```

### Code
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
#include "ssd1306.h"

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

/* USER CODE BEGIN PV */
/* Khai báo biến oled */
SSD1306 oled;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
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
	MX_I2C1_Init();
	/* USER CODE BEGIN 2 */
	/* Khởi tạo oled, tô màu đen lên màn hình */
	SSD1306_Init(&oled, &hi2c1);
	SSD1306_Fill(&oled, SSD1306_BLACK);
	SSD1306_UpdateScreen(&oled);

	/* Đợi màn hình khởi tạo xong */
	HAL_Delay(100);

	/* Viết "Hello World", font chữ 11x18, ở vị trí 0,0 trên màn hình */
	SSD1306_SetCursor(&oled, 0, 0);
	SSD1306_WriteString(&oled, "Hello World", Font_11x18, SSD1306_WHITE);

	/* Viết "Welcome", font chữ 11x18, ở vị trí 0,36 trên màn hình */
	SSD1306_SetCursor(&oled, 0, 36);
	SSD1306_WriteString(&oled, "Welcome", Font_11x18, SSD1306_WHITE);

	/* Vòng for để vẽ hình chữ nhật 28x64 lên màn hình */
	for (uint8_t i = 0; i < 28; i++)
		for (uint8_t j = 0; j < 64; j++)
		{
			SSD1306_DrawPixel(&oled, 100 + i, j, SSD1306_WHITE);
		}

	/* Cập nhật thay đổi lên màn hình */
	SSD1306_UpdateScreen(&oled);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
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
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void)
{

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
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
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
#include "ssd1306.h"

/* USER CODE END Includes */
...
```

Khai báo biến `oled`:

```c++
...
/* USER CODE BEGIN PV */
/* Khai báo biến oled */
SSD1306 oled;

/* USER CODE END PV */
...
```

Trong hàm `main()`

```c++
...
/* USER CODE BEGIN 2 */
	/* Khởi tạo oled, tô màu đen lên màn hình */
	SSD1306_Init(&oled, &hi2c1);
	SSD1306_Fill(&oled, SSD1306_BLACK);
	SSD1306_UpdateScreen(&oled);

	/* Đợi màn hình khởi tạo xong */
	HAL_Delay(100);

	/* Viết "Hello World", font chữ 11x18, ở vị trí 0,0 trên màn hình */
	SSD1306_SetCursor(&oled, 0, 0);
	SSD1306_WriteString(&oled, "Hello World", Font_11x18, SSD1306_WHITE);

	/* Viết "Welcome", font chữ 11x18, ở vị trí 0,36 trên màn hình */
	SSD1306_SetCursor(&oled, 0, 36);
	SSD1306_WriteString(&oled, "Welcome", Font_11x18, SSD1306_WHITE);

	/* Vòng for để vẽ hình chữ nhật 28x64 lên màn hình */
	for (uint8_t i = 0; i < 28; i++)
		for (uint8_t j = 0; j < 64; j++)
			SSD1306_DrawPixel(&oled, 100 + i, j, SSD1306_WHITE);

	/* Cập nhật thay đổi lên màn hình */
	SSD1306_UpdateScreen(&oled);
	/* USER CODE END 2 */
...
```

### Kết quả

![alt text](images/Image.jpeg)

## Câu hỏi và bài tập

### Câu hỏi

1. Số điểm ảnh (chiều dài, chiều rộng) của màn hình OLED 0.96 inch là bao nhiêu ?
2. Màn hình có thể hiển thị được nhiều màu sắc không ?

### Bài tập

1. Vẽ hình tròn
2. Đọc nhiệt độ độ ẩm và in lên OLED. Gợi ý: dùng `sprint()` để ép số vào một chuỗi rồi in lên màn hình
3. (Khó) Tự tạo một hàm vẽ hình bất kỳ lên LCD. Gợi ý: dùng `SSD1306_DrawPixel()` vẽ từng pixel.

## Tài liệu tham khảo

[1] STMicroelectronics, "Medium-density performance line Arm®-based 32-bit MCU with 64 or 128 KB Flash, USB, CAN, 7 timers, 2 ADCs, 9 com. interfaces", STM32F103Cx8/B Datasheet, Sep. 2023.

[2] Solomon Systech, "128 x 64 Dot Matrix OLED/PLED Segment/Common Driver with Controller", SSD1306 datasheet, Apr. 2008. 