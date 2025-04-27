# Bài 1.1: Các thao tác cơ bản trên phầm mềm STM32CubeIDE

Bài viết này hướng dẫn các thao tác cơ bản trên phần mềm STM32CubeIDE.

## Mục lục

- [Bài 1.1: Các thao tác cơ bản trên phầm mềm STM32CubeIDE](#bài-11-các-thao-tác-cơ-bản-trên-phầm-mềm-stm32cubeide)
  - [Mục lục](#mục-lục)
  - [Mục tiêu bài học](#mục-tiêu-bài-học)
  - [Viết code vào file main.c sao cho đúng ?](#viết-code-vào-file-mainc-sao-cho-đúng-)
  - [File stm32f1xx\_it.c](#file-stm32f1xx_itc)
  - [Thêm file mới](#thêm-file-mới)
  - [Thêm file có sẵn](#thêm-file-có-sẵn)
  - [Thêm thư viện ngoài](#thêm-thư-viện-ngoài)
  - [Phím tắt khi viết code](#phím-tắt-khi-viết-code)

## Mục tiêu bài học

- Viết code vào file main.c
- Sử dụng một số phím tắt thường dùng

## Viết code vào file main.c sao cho đúng ?

Phần mềm STM32CubeIDE tích hợp **STM32CubeMX**. **STM32CubeMX** là một phần mềm cấu hình và cài đặt thông số cho vi điều khiển STM32 bằng giao diện đồ hoạ dễ sử dụng.

![alt text](<images/Screenshot 2024-09-11 at 21.08.59.png>)

Sau khi cấu hình xong các chân GPIO, các ngoại vi,... phần mềm sẽ **tự động tạo code và lưu vào file `main.c`**. Để tránh ghi đè lên code của người dùng, file `main.c` được chia làm nhiều vùng. Người dùng chỉ được phép viết code vào các vùng được cho phép. Những vùng cho phép được bắt đầu bằng dòng comment `USER CODE BEGIN` và kết thúc bằng `USER CODE END`. 

Ví dụ về một file `main.c`:


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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
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
  /* USER CODE BEGIN 2 */

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

Các biến và hàm viết nằm ngoài phần `USER CODE` được tự động tạo. Trong đó các hàm có tên `MX_` ở đầu là do STM32CubeMX tự động tạo. Các hàm này thường là các hàm khởi tạo cho các ngoại vi I2C, SPI, GPIO,...

## File stm32f1xx_it.c

File stm32f1xx_it.c là nơi định nghĩa các hàm phục vụ ngắt của vi điều khiển STM32. Khi ta cấu hình một ngắt nào đó, STM32CubeMX sẽ tự động tạo hàm phục vụ ngắt tương ứng trong file này. Mỗi hàm phục vụ ngắt có tên thêm chữ `Handler` ở đuôi.

Có thể viết code vào các khối `USER CODE` để viết các hàm phục vụ ngắt này. Tuy nhiên, thư viện HAL đã xử lý sẵn các ngắt và cung cấp các hàm Callback.

Ta có thể định nghĩa lại các hàm Callback này trong file `main.c` để xử lý khi ngắt xảy ra, thay vì viết hàm xử lý vào file `stm32f1xx_it.c`.

Ví dụ: hàm `HAL_GPIO_EXTI_Callback()` được gọi khi ngắt trên GPIO xảy ra. Hàm này được người dùng định nghĩa lại trong file `main.c`.

```c++
/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	/* Kiểm tra nếu GPIO gây ra ngắt là chân số 5 */
	if(GPIO_Pin == GPIO_PIN_5){
		/* Nháy đèn LED ở chân PC13 */
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	}
}
/* USER CODE END 4 */
```

## Thêm file mới

Chuột phải vào thư mục muốn thêm, chọn [New] > [Header File] nếu muốn thêm file .h hoặc chọn [New] > [Source File] nếu muốn thêm file.c

## Thêm file có sẵn

Cách 1: Kéo thả file từ cửa sổ thư mục của Windows vào trong thư mục muốn thêm trong phần mềm STM32CubeIDE. Phần mềm sẽ hỏi bạn muốn copy file hay link tới file đó. Chọn copy.
Cách 2: Chuột phải vào thư mục muốn thêm file, chọn [Import]. Cửa sổ mới hiện ra, bung thư mục *General* ra, rồi chọn *File System*. Nhấn **Next**. Bên cạnh dòng *From Directory*, chọn **Browse...**. Cửa sổ chọn thư mục của Windows sẽ hiện ra. Tìm tới thư mục chứa file muốn thêm vào, chọn thư mục rồi nhấn **Open**. Các file trong thư mục sẽ được liệt kê trong bảng bên phải. Tích chọn file muốn thêm, rồi nhấn **Finish**.

## Thêm thư viện ngoài

Thêm thư viện ngoài cũng như thêm file có sẵn. Đối với những thư viện đơn giản (chỉ gồm 1 hoặc 2 cặp file .c và .h), thêm các file .h vào thư mục **Core/Inc**, các file .c vào thư mục **Core/Src**.

## Phím tắt khi viết code

Các phím tắt hay sử dụng:
- Alt + mũi tên lên/xuống: Di chuyển dòng hiện tại lên trên/xuống
- Shift + mũi tên/xuống: Bôi đen nhiều dòng
- Giữ Alt + Click chuột: Tạo thêm một con trỏ văn bản tại vị trí click chuột.
- Ctrl + Alt + mũi tên xuống: Copy dòng hiện tại xuống dưới.
- Ctrl + B: Build
- Ctrl + Space: Hiện lên gợi ý về tên hàm, tên biến; tự động hoàn thành code.