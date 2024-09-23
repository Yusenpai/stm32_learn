# Bài 5: Timer trên vi điều khiển STM32F103

Timer là một ngoại vi dùng để định thời, tạo dạng sóng và đo thời gian một cách chính xác. Bài viết này sẽ hướng dẫn sử dụng Timer trên vi điều khiển STM32F103

## Mục lục
- [Bài 5: Timer trên vi điều khiển STM32F103](#bài-5-timer-trên-vi-điều-khiển-stm32f103)
	- [Mục lục](#mục-lục)
	- [Kiến thức cần có](#kiến-thức-cần-có)
	- [Mục tiêu bài học](#mục-tiêu-bài-học)
	- [Timer](#timer)
	- [Timer trên STM32F103C8T6](#timer-trên-stm32f103c8t6)
	- [Chế độ đếm cơ bản của TIM2](#chế-độ-đếm-cơ-bản-của-tim2)
	- [Thư viện HAL\_TIM](#thư-viện-hal_tim)
	- [Project: Nháy đèn đúng 1s dùng Timer](#project-nháy-đèn-đúng-1s-dùng-timer)
		- [Tạo project mới, cấu hình TIM2](#tạo-project-mới-cấu-hình-tim2)
		- [Các hàm sử dụng](#các-hàm-sử-dụng)
	- [Code](#code)
	- [Câu hỏi và bài tập](#câu-hỏi-và-bài-tập)
		- [Câu hỏi](#câu-hỏi)
		- [Bài tập](#bài-tập)
	- [Tài liệu tham khảo](#tài-liệu-tham-khảo)

## Kiến thức cần có

- [Bài 1 - Giới thiệu vi điều khiển STM32, sử dụng STM32CubeIDE và thư viện HAL.md](<../Bài 1/Bài 1 - Giới thiệu vi điều khiển STM32, sử dụng STM32CubeIDE và thư viện HAL.md>)
- [Bài 2 - Lập trình GPIO cho vi điều khiển STM32F103C8T6](<../Bài 2/Bài 2 - Lập trình GPIO cho vi điều khiển STM32F103C8T6.md>)
- [Bài 4 - Ngắt trên vi điều khiển STM32F103](<../Bài 4/Bài 4 - Ngắt trên vi điều khiển STM32F103.md>)

## Mục tiêu bài học

## Timer

Timer là một ngoại vi dùng để định thời, tạo dạng sóng và đo thời gian một cách chính xác. 

Timer đơn giản nhất là một mạch đếm nhị phân (đếm lên hoặc đếm xuống). Mạch đếm này được cấp xung clock từ nhiều nguồn (bên trong hoặc bên ngoài). Khi mạch đếm lên bị tràn số (mạch đếm vượt qua giá trị lớn nhất và quay trở về 0), thì một ngắt xảy ra. Nhờ ngắt này mà ta có thể lập trình cho CPU thực hiện một tác vụ lặp lại theo chu kỳ.

Các Timer phức tạp có thêm các khối:

- Prescaler, Divider: Prescaler và Divider để chia tần số; prescaler chia theo số mũ của 2 (2, 4, 8,...); divider chia theo số bất kỳ. Xung clock sẽ đi qua các khối này rồi mới tới mạch đếm.
- Khối lọc đối với xung từ bên ngoài
- Khối Output Compare (OC): Là một thanh ghi có thể thay đổi được. Giá trị trong thanh ghi luôn được so sánh với giá trị trong mạch đếm, khi bằng nhau sẽ xảy ra ngắt, thay đổi chân GPIO,...
- Khối Input Capture (IC): Là khối được nối với chân GPIO. Khi chân GPIO có sự thay đổi, mốc thời gian (giá trị mạch đếm) sẽ được lưu lại.

## Timer trên STM32F103C8T6

Có tất cả 7 Timer trên vi điều khiển STM32F103C8T6:

- TIM1: Timer 16 bit nâng cao, các chức năng chuyên dùng để điều khiển động cơ
- TIM2,3,4: Timer 16 bit thông dụng. Mỗi Timer có 4 kênh OC, 4 kênh IC, 4 kênh PWM ngõ ra, 4 kênh ngõ vào cho Encoder và cảm biến hall
- WWDT và IWDT: Window Watchdog Timer và Independent Watchdog Timer
- SysTick: Timer 24 bit đếm xuống, dành cho hàm `HAL_Delay()`

Tham khảo các loại Timer ở [UM1850](um1850.pdf)

## Chế độ đếm cơ bản của TIM2

TIM2 là timer 16 bit thông dụng, tuy vậy cấu tạo vẫn hết sức phức tạp.

![alt text](<images/Screenshot 2024-09-22 at 19.30.26.png>)

Ta sẽ tìm hiểu chế độ hoạt động cơ bản nhất: chế độ đếm lên.

Ở chế độ này, TIM2 đếm lên từ 0. Khi TIM2 đếm tới giá trị nằm trong thanh ghi ***auto-reload***, mạch đếm sẽ reset về 0, gây ra *ngắt tràn số*.

Khi TIM2 còn hoạt động thì ngắt sẽ xảy ra với chu kỳ chính xác. Ta có thể dùng ngắt này để thực hiện các tác vụ lặp lại với yêu cầu chính xác về thời gian; ví dụ nháy LED, tăng số kim giây của đồng hồ,...

## Thư viện HAL_TIM

Thư viện HAL_TIM là một phần của thư viện HAL cho STM32, cung cấp các cấu trúc và hàm để cấu hình và điều khiển Timer trên STM32F103C8. Một số hàm và cấu trúc của thư viện:

```c++
TIM_HandleTypeDef
```

## Project: Nháy đèn đúng 1s dùng Timer

Project này sử dụng Timer2 ở chế độ đếm lên, tạo ngắt mỗi lần tràn số. Dùng ngắt để nháy đèn chính xác 1s.

### Tạo project mới, cấu hình TIM2

Tạo project mới. Trong giao diện STM32CubeMX, trong [Pinout & Configuration] > [Timers] > [TIM2], ở dòng *Clock Source*, chọn Internal Clock.

![alt text](<images/Screenshot 2024-09-23 at 16.20.27.png>)

Khi chọn Internal Clock sẽ lấy xung clock từ bên trong vi điều khiển cấp cho Timer. Clock mặc định là 8MHz. Clock này có thể thay đổi lên tới 72MHz, sẽ được hướng dẫn ở bài sau.

Trong bảng *Configuration*, tab *Parameter Settings* chứa các thông số của TIM2, gồm:

- Prescaler: bộ chia tần số 16 bit. Giá trị từ 0 - 65535
- Couter Mode: chế độ đếm. Chọn đếm lên (Up)
- Counter Period: Giá trị trong thanh ghi Auto-Reload 16 bit. Mạch đếm khi đếm tới số này sẽ reset về 0. Giá trị từ 0 - 65535
- Internal Clock Division: Chia tần số cho 2 hoặc 4. Chọn No division.
- Auto-reload Preload: Sử dụng khi cần thay đổi giá trị thanh ghi Auto-Reload. Chọn Disable.

Để chọn giá trị cho Prescaler và Counter Period, ta chọn theo công thức sau:

$$
f=\frac{f_{OSC}}{\left(Prescaler+1\right)\left(Period+1\right)}
$$

Trong đó:

- $f$: tần số ngắt tràn số xảy ra. Ta muốn LED nháy đúng 1s nên ta chọn $f=1Hz$
- $f_{OSC}$: tần số cấp cho mạch đếm. Ở đây là 8MHz.
- $Prescaler$: giá trị của thanh ghi Prescaler.
- $Period$: Giá trị của thanh ghi Couter Period.

Ta chọn Prescaler = 3999 thì Period = 1999. Nhập các giá trị này vào bảng thông số.

![alt text](<images/Screenshot 2024-09-23 at 16.33.31.png>)

Trong tab *NVIC Settings*, tích vào dòng TIM2 global interrupt để bật ngắt của TIM2.

![alt text](<images/Screenshot 2024-09-23 at 16.35.06.png>)

Cuối cùng, cấu hình chân PC13 thành ngõ ra. Lưu lại và phần mềm tự động tạo code.

### Các hàm sử dụng

Để sử dụng TIM2 ở chế độ ngắt, thư viện HAL cung cấp một số hàm:

```c++
/*
* Bắt đầu Timer ở chế độ ngắt. 
* htim: con trỏ tới cấu trúc điều khiển Timer
*/
HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim);

/*
* Hàm callback. Được gọi mỗi khi timer bị tràn, hoặc update khi đếm tới giá trị Auto-Reload.
*/
HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
```

## Code
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
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  /* Gọi hàm này để bắt đầu TIM2 */
  HAL_TIM_Base_Start_IT(&htim2);

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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
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
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 3999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* Định nghĩa lại hàm callback. Hàm này được gọi mỗi 1s/lần */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}

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

Trong hàm `main()`, gọi hàm để bắt đầu đếm:
```c++
...
/* USER CODE BEGIN 2 */
/* Gọi hàm này để bắt đầu TIM2 */
HAL_TIM_Base_Start_IT(&htim2);

/* USER CODE END 2 */
...
```

Định nghĩa lại hàm callback:
```c++
...
/* USER CODE BEGIN 4 */
/* Định nghĩa lại hàm callback. Hàm này được gọi mỗi 1s/lần */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}

/* USER CODE END 4 */
...
```

## Câu hỏi và bài tập

### Câu hỏi

1. Timer đơn giản nhất là một mạch gì?
2. Vì sao Timer có thể dùng để thực hiện một công việc theo chu kỳ? (dùng cơ chế nào?)
3. Thanh ghi Auto-Reload để làm gì?
4. Khi nào thì hàm `HAL_TIM_PeriodElapsedCallback()` được gọi?
5. Chọn tần số ngắt xảy ra là 5Hz. Tính giá trị của Prescaler và Period, biết tần số cấp cho TIM2 là 8MHz.

### Bài tập

1. Làm một đồng hồ bấm giờ đơn giản bằng Timer. Mỗi 1s gửi số giây lên máy tính qua UART.

## Tài liệu tham khảo

[1] STMicroelectronics, "Medium-density performance line Arm®-based 32-bit MCU with 64 or 128 KB Flash, USB, CAN, 7 timers, 2 ADCs, 9 com. interfaces", STM32F103Cx8/B Datasheet, Sep. 2023.

[2] STMicroelectronics, "STM32F101xx, STM32F102xxm STM32F103xx, STM32F105xx and STM32F107xx advanced Arm®-based 32-bit MCUs", RM0008 Reference Manual, Feb. 2021.

