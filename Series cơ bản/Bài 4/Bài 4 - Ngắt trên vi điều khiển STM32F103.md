# Bài 4: Ngắt trên vi điều khiển STM32F103

**Ngắt** (interrupt) là một cơ chế điều khiển quan trọng trong vi điều khiển, cho phép CPU dừng lại để xử lí các sự kiện có độ ưu tiên cao.

Bài viết này sẽ hướng dẫn cấu hình và sử dụng ngắt trên vi điều khiển STM32F103C8

## Mục lục

- [Bài 4: Ngắt trên vi điều khiển STM32F103](#bài-4-ngắt-trên-vi-điều-khiển-stm32f103)
	- [Mục lục](#mục-lục)
	- [Kiến thức cần có](#kiến-thức-cần-có)
	- [Mục tiêu bài học](#mục-tiêu-bài-học)
	- [Ngắt](#ngắt)
	- [Ngắt trên vi điều khiển STM32](#ngắt-trên-vi-điều-khiển-stm32)
		- [Ngắt ngoài](#ngắt-ngoài)
		- [Ngắt trong](#ngắt-trong)
		- [Bảng ngắt](#bảng-ngắt)
	- [Phục vụ ngắt](#phục-vụ-ngắt)
	- [Project: Điều khiển đèn LED với nút nhấn, sử dụng ngắt](#project-điều-khiển-đèn-led-với-nút-nhấn-sử-dụng-ngắt)
		- [Sơ đồ mạch điện](#sơ-đồ-mạch-điện)
		- [Tạo project mới, cấu hình GPIO](#tạo-project-mới-cấu-hình-gpio)
		- [Code](#code)
	- [Project: Giao tiếp Serial trong khi đang nháy đèn](#project-giao-tiếp-serial-trong-khi-đang-nháy-đèn)
		- [Tạo project mới, cấu hình GPIO và USART](#tạo-project-mới-cấu-hình-gpio-và-usart)
		- [Các hàm sử dụng](#các-hàm-sử-dụng)
		- [Code](#code-1)
	- [Câu hỏi và bài tập](#câu-hỏi-và-bài-tập)
		- [Câu hỏi](#câu-hỏi)
		- [Bài tập](#bài-tập)
	- [Tài liệu tham khảo](#tài-liệu-tham-khảo)

## Kiến thức cần có

- [Bài 1 - Giới thiệu vi điều khiển STM32F103](<../Bài 1/Bài 1 - Giới thiệu vi điều khiển STM32, sử dụng STM32CubeIDE và thư viện HAL.md>)
- [Bài 2 - Lập trình GPIO cho vi điều khiển STM32F103C8T6](<../Bài 2/Bài 2 - Lập trình GPIO cho vi điều khiển STM32F103C8T6.md>)

## Mục tiêu bài học

- Học cách cấu hình ngắt trong, ngắt ngoài
- Học cách sử dụng ngắt trong thư viện HAL

## Ngắt

**Ngắt** (interrupt) là một cơ chế quan trọng trong lập trình nhúng, cho phép vi điều khiển dừng thực hiện chương trình chính để xử lý các sự kiện quan trọng ngay khi chúng xảy ra. 

**Ngắt** giúp tiết kiệm tài nguyên và tăng hiệu suất làm việc của vi điều khiển.

Ví dụ: Khi đọc giá trị nút nhấn dùng hàm `HAL_GPIO_ReadPin()`, bạn viết nó vào vòng `while()`:

```c++
while(1){
	uint8_t pinValue = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	if(pinValue == 1)
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 1);
	else
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 0);
}
```

Đoạn code trên ***liên tục kiểm tra*** giá trị chân `PA0`, rồi điều khiển `PA1` tương ứng. Đây là phương pháp ***polling***. Nhược điểm của phương pháp này là CPU dùng toàn bộ ***100%*** thời gian chỉ dành cho việc đọc chân `PA0`. Như vậy sẽ lãng phí CPU, đặc biệt là khi có nhiều tác vụ cần phải xử lý.

Thay vì vậy, ta sử dụng **ngắt ngoài** của chân `PA0`. Khi ngắt xảy ra, CPU sẽ dừng chương trình hiện tại, nhảy tới **hàm phục vụ ngắt** (ISR), điều khiển chân `PA1` theo giá trị của `PA0`, rồi quay về hàm `main()`. Khi đó thời gian xử lý CPU dành cho hàm ngắt là rất ít mà vẫn đảm bảo chức năng.

Ta có thể ví ngắt như chuông báo thức vậy. Thay vì nhìn đồng hồ và căn đúng 2 giờ rồi đi nấu cơm, ta có thể đặt chuông hẹn lúc 2 giờ, rồi đi làm việc khác. Khi chuông reo thì ta sẽ bật bếp nấu cơm,...

## Ngắt trên vi điều khiển STM32

Trên vi điều khiển STM32 có 2 loại ngắt: **ngắt ngoài** (External Interrupt) và **ngắt trong** (Internal Interrupt).

### Ngắt ngoài

Ngắt ngoài là ngắt xảy ra khi tín hiệu ở chân GPIO thay đổi. Có 3 trường hợp:
- Tín hiệu cạnh lên (Rising Edge): Tín hiệu thay đổi từ mức thấp lên mức cao
- Tín hiệu cạnh xuống (Falling Edge): Tín hiệu thay đổi từ mức cao xuống mức thấp
- Cả hai: Khi tín hiệu cạnh lên hoặc cạnh xuống thì đều sinh ra ngắt.

Ngoại vi điều khiển ngắt ngoài của STM32F103 tên là External Interrupt/Event Controller (EXTI), điều khiển lên tới 19 ngắt, 16 ngắt trong số đó nối tới tất cả các chân GPIO của vi điều khiển. Các đường ngắt có tên **EXTIn**

Vì số lượng GPIO lớn hơn số đường ngắt, nên các chân GPIO được nối chung tới các đường ngắt như sau:

![alt text](<images/Screenshot 2024-09-22 at 17.25.37.png>)

Để tiết kiệm không gian thêm nữa, các đường ngắt được gộp với nhau:
- Từ EXTI0 đến EXTI4: mỗi đường ngắt là một ngắt riêng biệt
- Từ EXTI5 đến EXTI9: gộp với nhau thành một ngắt, gọi là EXTI9_5
- Từ EXTI10 đến EXTI15: gộp với nhau thành một ngắt, gọi là EXTI15_10

### Ngắt trong

Ngắt trong là các ngắt tới từ ngoại vi bên trong vi điều khiển như TIMER, UART, I2C, SPI, ADC,...

### Bảng ngắt

Bảng ngắt là một danh sách các ngắt được đặt ở phần đầu của bộ nhớ chương trình (địa chỉ 0x00000000). Tham khảo [RM0008](rm0008.pdf), bảng 63, trang 204.

Ngắt nằm ở địa chỉ càng thấp (địa chỉ càng nhỏ) thì càng được ưu tiên. Ngắt có độ ưu tiên cao nhất là RESET (khi nhấn nút reset, ngắt này xảy ra và reset vi điều khiển).

## Phục vụ ngắt

Khi ngắt xảy ra, CPU sẽ dừng chương trình hiện tại (nếu ngắt có độ ưu tiên cao hơn) và nhảy tới **hàm phục vụ ngắt** (ISR) của ngắt tương ứng. Sau khi thực hiện xong hàm ngắt, CPU lại quay trở về tiếp tục chương trình chính.

> Lưu ý: Hàm phục vụ ngắt phải ngắn gọn, đơn giản. Xử lí hàm ngắt quá lâu sẽ ảnh hưởng tới chương trình chính.

## Project: Điều khiển đèn LED với nút nhấn, sử dụng ngắt

### Sơ đồ mạch điện

![alt text](<../Bài 2/images/schematic2.png>)

### Tạo project mới, cấu hình GPIO

Sau khi tạo project, mở giao diện STM32CubeMX để cấu hình ngắt và GPIO. Chọn chân PC13 trên hình, rồi chọn GPIO_Output. Chọn chân PA5 trên hình, rồi chọn GPIO_EXTI5:

Trong [Pinout & Configuration] > [GPIO], trong bảng *Configuration*, chọn dòng `PA5`. Chọn *GPIO mode* là `External Interrupt Mode with Rising edge trigger detection` (ngắt cạnh lên).

![alt text](<images/Screenshot 2024-09-23 at 14.30.14.png>)

Chọn tab *NVIC* bên cạnh. Tích vào dòng `EXTI line [9:5] interrupts` để kích hoạt ngắt ngoài.

![alt text](<images/Screenshot 2024-09-23 at 14.34.01.png>)

Lưu lại và để phần mềm tạo code.

### Code

Sau khi tạo code, mở file `main.c`, tìm tới hàm khởi tạo GPIO, `static void MX_GPIO_Init(void)`:


```c++
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}
```
 
Hàm này cài đặt các thông số cho GPIO PA5 và PC13; trong đó hai dòng lệnh cuối để bật ngắt ngoài `EXTI9_5`.

Phần mềm còn tạo sẵn hàm phục vụ ngắt (ISR) vào cuối file `stm32f1xx_it.c`:

```c++
...
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */

  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
}
```

Hàm này gọi `HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5)`, xử lý ngắt, rồi gọi hàm callback `HAL_GPIO_EXTI_Callback(GPIO_Pin)`. Ta sẽ viết code xử lý ngắt vào hàm callback này.

Hàm callback `HAL_GPIO_EXTI_Callback(GPIO_Pin)` được khai báo và định nghĩa lại trong file `main.c`:

```c++
...
/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	/* Kiểm tra nếu GPIO gây ra ngắt là chân số 5 */
	if(GPIO_Pin == GPIO_PIN_5){
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	}
}
/* USER CODE END 4 */
...
```

File `main.c`:
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
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	/* Kiểm tra nếu GPIO gây ra ngắt là chân số 5 */
	if(GPIO_Pin == GPIO_PIN_5){
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	}
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

## Project: Giao tiếp Serial trong khi đang nháy đèn

Project này cho thấy sức mạnh của ngắt. Trong vòng while là đoạn code nháy đèn sử dụng hàm HAL_Delay(); trong khi đó vẫn giao tiếp với máy tính qua UART. Máy tính sẽ gửi một ký tự (một byte) bất kỳ, khi vi điều khiển nhận được sẽ ngay lập tức gửi trả lại ký tự đó về máy tính.

### Tạo project mới, cấu hình GPIO và USART

Tạo project mới. Cấu hình cho chân PC13 ngõ ra (GPIO_Output). Bật USART1 ở chế độ không đồng bộ, các thông số còn lại giữ nguyên. Trong bảng *Configuration*, chọn tab *NVIC Settings*. Tích vào dòng `USART1 global interrupt` để bật ngắt cho USART1.

![alt text](<images/Screenshot 2024-09-23 at 15.46.27.png>)

Lưu lại và để phần mềm tạo code.

### Các hàm sử dụng

Để kết hợp truyền nhận qua UART và ngắt, ta sử dụng các hàm đặc biệt:

```c++
/*
* Truyền dữ liệu qua UART ở chế độ ngắt. Hàm trả về ngay lập tức, CPU không cần phải đợi UART truyền xong. Sau khi truyền xong, tự động gọi hàm HAL_UART_TxCpltCallback()
* huart: con trỏ tới cấu trúc điều khiển của UART
* pData: con trỏ tới dữ liệu cần gửi đi
* Size: số lượng byte gửi đi
*/
HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);

/*
* Nhận dữ liệu từ UART ở chế độ ngắt. Hàm trả về ngay lập tức, CPU không cần phải đợi UART nhận xong. Sau khi nhận đủ, tự động gọi hàm HAL_UART_TxCpltCallback()
* huart: con trỏ tới cấu trúc điều khiển của UART
* pData: con trỏ tới nơi nhận dữ liệu
* Size: số byte nhận về.
*/
HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

/* Hàm callback. Được gọi sau khi gửi xong dữ liệu ở UART chế độ ngắt */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);

/* Hàm callback. Được gọi sau khi nhận xong dữ liệu ở UART chế độ ngắt */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
```

### Code

Trong file `stm32f1xx_it.c`, hàm phục vụ ngắt được tự động tạo ở cuối file:

```c++
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}
```

Hàm `HAL_UART_IRQHandler(&huart1)` là hàm phục vụ ngắt được tạo sẵn bởi thư viện HAL. Khác với hàm phục vụ ngắt của GPIO, hàm này rất phức tạp. Ta chỉ sử dụng các hàm callback do hàm này gọi, bao gồm: `HAL_UART_TxCpltCallback()` và `HAL_UART_RxCpltCallback()`

File `main.c`:

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
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Tạo một biến lưu trữ dữ liệu nhận về */
uint8_t receivedData;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  /* Nhận 1 byte từ máy tính qua UART, lưu vào biến receivedData, ở chế độ ngắt */
  HAL_UART_Receive_IT(&huart1, &receivedData, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	/* Nháy đèn */
	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	  HAL_Delay(500);
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
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

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
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
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
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

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
/* Định nghĩa lại hàm callback HAL_UART_RxCpltCallback() */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	/* Gửi trả lại dữ liệu đã nhận */
	HAL_UART_Transmit_IT(&huart1, &receivedData, 1);

	/* Tiếp tục nhận dữ liệu tiếp theo */
	HAL_UART_Receive_IT(&huart1, &receivedData, 1);
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

Tạo biến `receivedData` để lưu ký tự nhận được:
```c++
...
/* USER CODE BEGIN PV */
/* Tạo một biến lưu trữ dữ liệu nhận về */
uint8_t receivedData;

/* USER CODE END PV */
...
```

Trong hàm `main()`, gọi hàm `HAL_UART_Receive_IT()` để bắt đầu nhận dữ liệu từ máy tính:
```c++
...
/* USER CODE BEGIN 2 */
/* Nhận 1 byte từ máy tính qua UART, lưu vào biến receivedData, ở chế độ ngắt */
HAL_UART_Receive_IT(&huart1, &receivedData, 1);

/* USER CODE END 2 */
...
```

Khi nhận đủ 1 byte, hàm callback `HAL_UART_RxCpltCallback()` sẽ được gọi. Ta định nghĩa lại hàm này trong file `main.c`:
```c++
...
/* USER CODE BEGIN 4 */
/* Định nghĩa lại hàm callback HAL_UART_RxCpltCallback() */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	/* Gửi trả lại ký tự đã nhận */
	HAL_UART_Transmit_IT(&huart1, &receivedData, 1);

	/* Tiếp tục nhận ký tự tiếp theo */
	HAL_UART_Receive_IT(&huart1, &receivedData, 1);
}
...
```

Cuối cùng, thêm code nháy LED vào vòng `while()`:
```c++
...
/* Infinite loop */
/* USER CODE BEGIN WHILE */
while (1)
{
	/* USER CODE END WHILE */

	/* USER CODE BEGIN 3 */
	/* Nháy đèn */
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	HAL_Delay(500);
}
/* USER CODE END 3 */
...
```

## Câu hỏi và bài tập

### Câu hỏi
1. Có mấy loại ngắt? Nêu sự khác nhau giữa chúng?
2. Ngắt có độ ưu tiên cao nhất của STM32F103C8T6?
3. Nếu các đường ngắt được nối chung với nhau, làm sao để biết ngắt tới từ đâu?

### Bài tập

1. Nhấn nút để gửi chữ "Hello" lên Serial. Dùng ngắt ngoài.
2. Nhận đúng 5 ký tự từ UART gửi từ Serial trên máy tính. Khi nhận đủ 5 kí tự, gửi trả lại Serial; thực hiện trong khi đang nháy đèn LED.

## Tài liệu tham khảo

[1] STMicroelectronics, "Medium-density performance line Arm®-based 32-bit MCU with 64 or 128 KB Flash, USB, CAN, 7 timers, 2 ADCs, 9 com. interfaces", STM32F103Cx8/B Datasheet, Sep. 2023.

[2] STMicroelectronics, "STM32F101xx, STM32F102xxm STM32F103xx, STM32F105xx and STM32F107xx advanced Arm®-based 32-bit MCUs", RM0008 Reference Manual, Feb. 2021.