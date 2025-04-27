# Giao tiếp USB HID Keyboard
## Giới thiệu
HID là viết tắt của Human Interface Device là một phân lớp trong tiêu chuẩn USB cho các thiết bị máy tính được vận hành bởi con người. Nói theo cách chi tiết hơn, HID là một tiêu chuẩn được tạo ra để đơn giản hóa quá trình cài đặt thiết bị đầu vào.

HID chủ yếu được sử dụng cho các thiết bị cho phép con người điều khiển máy tính. Khi sử dụng các thiết bị này, máy tính có thể phản ứng với các input của con người (ví dụ: chuyển động của chuột hoặc nhấn phím). Quá trình này phải diễn ra nhanh để người dùng máy tính không thấy độ trễ quá lâu giữa hành động của mình cho đến khi máy tính phản hồi. Các ví dụ điển hình cho các thiết bị lớp HID là:

- Keyboards và pointing devices (mouse, joysticks và trackballs).
- Front-panel controls (buttons, knobs, sliders, and switches).
- Các thiết bị mô phỏng (vô lăng, bàn đạp, các thiết bị đầu vào VR).
- Remote controls và telephone keypads.
- Các thiết bị tốc độ dữ liệu thấp khác cung cấp dữ liệu môi trường (nhiệt kế, đồng hồ đo năng lượng hoặc đầu đọc mã vạch).
## Mục lục

- [Giao tiếp USB HID Keyboard](#giao-tiếp-usb-hid-keyboard)
  - [Giới thiệu](#giới-thiệu)
  - [Mục lục](#mục-lục)
  - [Quá trình giao tiếp USB HID với máy tính](#quá-trình-giao-tiếp-usb-hid-với-máy-tính)
    - [HID Report Descriptor](#hid-report-descriptor)
    - [HID Report](#hid-report)
      - [Modifier](#modifier)
      - [Reserved](#reserved)
      - [Keycode](#keycode)
  - [Project: Nhấn button để gửi phím tới máy tính.](#project-nhấn-button-để-gửi-phím-tới-máy-tính)
    - [Cấu hình USB HID](#cấu-hình-usb-hid)
    - [Một số hàm và cấu trúc sử dụng](#một-số-hàm-và-cấu-trúc-sử-dụng)
    - [Code](#code)
      - [Chỉnh sửa Mouse thành Keyboard](#chỉnh-sửa-mouse-thành-keyboard)
      - [Hàm main](#hàm-main)
    - [Quá trình nạp code và kết nối USB](#quá-trình-nạp-code-và-kết-nối-usb)

## Quá trình giao tiếp USB HID với máy tính
Sau khi cắm dây USB kết nối STM32 với máy tính:
- Máy tính gửi yêu cầu cấu hình USB.
- STM32 trả lời lại bằng cách gửi các Descriptor (bao gồm HID Report Descriptor).
- Máy tính đọc HID Report Descriptor từ thiết bị để xác định cấu trúc và nội dung của các report.
- STM32 gửi HID Report tới máy tính khi có sự kiện liên quan đến mouse xảy ra, máy tính xử lý report rồi tiếp tục nhận report tiếp theo.

Vì thiết bị HID có thể gửi dữ liệu tại bất kỳ thời điểm nào, nên máy tính cần đảm bảo rằng dữ liệu được lấy mẫu định kỳ.

### HID Report Descriptor

HID Report Descriptor là một trong những Descriptor mà USB Host (máy tính) yêu cầu từ USB Device (STM32/Keyboard). Trước khi STM32 gửi dữ liệu tới máy tính, STM32 sẽ gửi HID Report Descriptor để mô tả cấu trúc và nội dung của các report.

Report Descriptor chỉ gửi lần đầu, máy tính lưu trữ thông tin từ HID Report Descriptor để sử dụng trong suốt thời gian thiết bị hoạt động. Report Descriptor của keyboard dễ dàng tìm được trên google:
```c++
char ReportDescriptor[63] = {
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x06, // USAGE (Keyboard)
    0xa1, 0x01, // COLLECTION (Application)
    0x05, 0x07, //   USAGE_PAGE (Keyboard)
    0x19, 0xe0, //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7, //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00, //   LOGICAL_MINIMUM (0)
    0x25, 0x01, //   LOGICAL_MAXIMUM (1)
    0x75, 0x01, //   REPORT_SIZE (1)
    0x95, 0x08, //   REPORT_COUNT (8)
    0x81, 0x02, //   INPUT (Data,Var,Abs)
    0x95, 0x01, //   REPORT_COUNT (1)
    0x75, 0x08, //   REPORT_SIZE (8)
    0x81, 0x03, //   INPUT (Cnst,Var,Abs)
    0x95, 0x05, //   REPORT_COUNT (5)
    0x75, 0x01, //   REPORT_SIZE (1)
    0x05, 0x08, //   USAGE_PAGE (LEDs)
    0x19, 0x01, //   USAGE_MINIMUM (Num Lock)
    0x29, 0x05, //   USAGE_MAXIMUM (Kana)
    0x91, 0x02, //   OUTPUT (Data,Var,Abs)
    0x95, 0x01, //   REPORT_COUNT (1)
    0x75, 0x03, //   REPORT_SIZE (3)
    0x91, 0x03, //   OUTPUT (Cnst,Var,Abs)
    0x95, 0x06, //   REPORT_COUNT (6)
    0x75, 0x08, //   REPORT_SIZE (8)
    0x15, 0x00, //   LOGICAL_MINIMUM (0)
    0x25, 0x65, //   LOGICAL_MAXIMUM (101)
    0x05, 0x07, //   USAGE_PAGE (Keyboard)
    0x19, 0x00, //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65, //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00, //   INPUT (Data,Ary,Abs)
    0xc0        // END_COLLECTION
};
```
### HID Report
Một HID Report của máy tính bao gồm 8 bytes:
- 1 byte Modifier
- 1 byte Reserved
- 6 byte Keycodes
```c++
typedef struct {
	uint8_t MODIFIER; // Phím điều khiển (Ctrl, Alt, Shift, v.v.)
	uint8_t RESERVED; // Byte dự phòng, không sử dụng, giá trị 0x00
	uint8_t KEYCODE1; // Keycode 1-6:Các phím được nhấn
	uint8_t KEYCODE2;
	uint8_t KEYCODE3;
	uint8_t KEYCODE4;
	uint8_t KEYCODE5;
	uint8_t KEYCODE6;
}keyboardReport;
```
#### Modifier
Mỗi bit trong byte Modifier đại diện cho một phím điều khiển:
|Bit|Phím|
|---|----|
|0|Left Control|
|1|Left Shift|
|2|Left Alt|
|3|Left GUI|
|4|Right Control|
|5|Right Shift|
|6|Right Alt|
|7|Right GUI|

Nếu không có phím nào được nhấn, MODIFIER = 0x00.

Ví dụ:
- Chỉ nhấn Left Shift: MODIFIER = 0b00000010 = 0x02.
- Nhấn Left Shift và Right Ctrl: MODIFIER = 0b00100010 = 0x22.
- Không nhấn: MODIFIER = 0x00.
#### Reserved
Byte dự phòng, luôn giữ giá trị 0x00 (không sử dụng).
#### Keycode
Gồm 6 bytes, mỗi byte chứa mã của một phím đang được nhấn, tối đa nhấn được 6 phím. Mã phím được xem ở HID Usage Table trong file Keyboard_Description_Helper.pdf. Nếu không có phím nào được nhấn, giá trị trong Keycode là 0x00.
Ví dụ:
- Nhấn phím a: KEYCODE1 = 0x04, các KEYCODE còn lại = 0x00.
- Nhấn phím a và b: KEYCODE1 = 0x04, KEYCODE2 = 0x05, các KEYCODE còn lại = 0x00.
- Nhấn phím 'A': MODIFIER = 0x02, KEYCODE1 = 0x04, các KEYCODE còn lại = 0x00.
## Project: Nhấn button để gửi phím tới máy tính.
### Cấu hình USB HID
Cấu hình STM32F103 như sau:

- Đặt `PB0` và `PA7` thành `GPIO_Input`.
- Bật Timer bất kỳ và tự viết hàm `delayUs()` sử dụng ngắt của Timer đó.
- Vào Connectivity/USB: check ô Device(FS), cấu hình để mặc định.
- Vào Middleware and Software Packs/USB_Device: Class For FS IP chọn Human Interface Device Class (HID). Trong phần Device Descriptor, đặt tên thiết bị hiện trên máy tính ở PRODUCT_STRING (Product Identifier).![alt text](image01.png)
- Optional: Sử dụng thạch anh ngoài. Ở project này thì vẫn có thể sử dụng HSI được.
- Vào Clock Configuration, để sử dụng USB thì chỉ có thể chọn PLLCLK là 48MHz hoặc 72MHz.![alt text](image02.png)
### Một số hàm và cấu trúc sử dụng
```c++
/* Cấu trúc HID Report của bàn phím */
typedef struct {
	uint8_t MODIFIER;
	uint8_t RESERVED;
	uint8_t KEYCODE1;
	uint8_t KEYCODE2;
	uint8_t KEYCODE3;
	uint8_t KEYCODE4;
	uint8_t KEYCODE5;
	uint8_t KEYCODE6;
}keyboardReport;
/* Hàm gửi report lên máy tính*/
uint8_t USBD_HID_SendReport(USBD_HandleTypeDef  *pdev, uint8_t *report, uint16_t len)
/*
* pdev: con trỏ đến cấu trúc dữ liệu đại diện cho thiết bị USB.
* report: con trỏ tới HID Report.
* len: độ dài report.
*/
```
### Code
#### Chỉnh sửa Mouse thành Keyboard
STM32CubeIDE mặc định chỉ tạo code cho Mouse, để sử dụng Keyboard, ta cần thay thế một số thứ như sau:
- Trong Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc vào file usbd_hid.h sửa HID_MOUSE_REPORT_DESC_SIZE 74U thành 63U:

```c++
#define HID_MOUSE_REPORT_DESC_SIZE    63U // Report Descriptor của keyboard 63 byte
```
- Trong Middlewares/ST/STM32_USB_Device_Library/Class/HID/Src vào file usbd_hid.c:

```c++
/* USB HID device FS Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_CfgFSDesc[USB_HID_CONFIG_DESC_SIZ]  __ALIGN_END =
{
  ...
  // Chỉ sửa lại dòng dưới thành 0x01:
  0x01,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  ...
};

// Thay thế Report Descriptor của Mouse thành của Keyboard.
__ALIGN_BEGIN static uint8_t HID_MOUSE_ReportDesc[HID_MOUSE_REPORT_DESC_SIZE]  __ALIGN_END =
{
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	0x09, 0x06, // USAGE (Keyboard)
	0xa1, 0x01, // COLLECTION (Application)
	0x05, 0x07, //   USAGE_PAGE (Keyboard)
	0x19, 0xe0, //   USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xe7, //   USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00, //   LOGICAL_MINIMUM (0)
	0x25, 0x01, //   LOGICAL_MAXIMUM (1)
	0x75, 0x01, //   REPORT_SIZE (1)
	0x95, 0x08, //   REPORT_COUNT (8)
	0x81, 0x02, //   INPUT (Data,Var,Abs)
	0x95, 0x01, //   REPORT_COUNT (1)
	0x75, 0x08, //   REPORT_SIZE (8)
	0x81, 0x03, //   INPUT (Cnst,Var,Abs)
	0x95, 0x05, //   REPORT_COUNT (5)
	0x75, 0x01, //   REPORT_SIZE (1)
	0x05, 0x08, //   USAGE_PAGE (LEDs)
	0x19, 0x01, //   USAGE_MINIMUM (Num Lock)
	0x29, 0x05, //   USAGE_MAXIMUM (Kana)
	0x91, 0x02, //   OUTPUT (Data,Var,Abs)
	0x95, 0x01, //   REPORT_COUNT (1)
	0x75, 0x03, //   REPORT_SIZE (3)
	0x91, 0x03, //   OUTPUT (Cnst,Var,Abs)
	0x95, 0x06, //   REPORT_COUNT (6)
	0x75, 0x08, //   REPORT_SIZE (8)
	0x15, 0x00, //   LOGICAL_MINIMUM (0)
	0x25, 0x65, //   LOGICAL_MAXIMUM (101)
	0x05, 0x07, //   USAGE_PAGE (Keyboard)
	0x19, 0x00, //   USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0x65, //   USAGE_MAXIMUM (Keyboard Application)
	0x81, 0x00, //   INPUT (Data,Ary,Abs)
	0xc0        // END_COLLECTION
};
```
#### Hàm main
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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_hid.h"
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
extern USBD_HandleTypeDef hUsbDeviceFS;

typedef struct {
	uint8_t MODIFIER; // Phím điều khiển (Ctrl, Alt, Shift, v.v.)
	uint8_t RESERVED; // Byte dự phòng, không sử dụng, giá trị 0x00.
	uint8_t KEYCODE1; // Keycode 1-6: Các phím được nhấn
	uint8_t KEYCODE2;
	uint8_t KEYCODE3;
	uint8_t KEYCODE4;
	uint8_t KEYCODE5;
	uint8_t KEYCODE6;
}keyboardReport;

keyboardReport HIDkeyBoard = {0, 0, 0, 0, 0, 0, 0, 0};
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
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t prevState1 = 0, prevState2 = 0;
  uint8_t state1, state2;
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  state1 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	  state2 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);
	  if(prevState1 == 1 && state1 == 0) {
		  HIDkeyBoard.KEYCODE1=0x00; // nhả phím
		  USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&HIDkeyBoard, sizeof(HIDkeyBoard)); // Gửi Report đến máy tính
	  }
	  if(prevState1 == 0 && state1) {
		  HIDkeyBoard.KEYCODE1=0x04; // nhấn phím 'a'
		  USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&HIDkeyBoard, sizeof(HIDkeyBoard)); // Gửi Report đến máy tính
	  }

	  if(prevState2 == 1 && state1 == 0) {
		  HIDkeyBoard.MODIFIER=0x00; // thả phím left shift
		  HIDkeyBoard.KEYCODE1=0x00; // thả phím 'b'
		  HIDkeyBoard.KEYCODE2=0x00; // thả phím 'c'
		  HIDkeyBoard.KEYCODE3=0x00; // thả phím 'd'
		  HIDkeyBoard.KEYCODE4=0x00; // thả phím 'e'
		  HIDkeyBoard.KEYCODE5=0x00; // thả phím 'f'
		  HIDkeyBoard.KEYCODE6=0x00; // thả phím 'g'
		  USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&HIDkeyBoard, sizeof(HIDkeyBoard)); // Gửi Report đến máy tính
	  }
	  if(prevState2 == 0 && state2) {
		  HIDkeyBoard.MODIFIER|= 1 << 1; // nhấn left shift
		  HIDkeyBoard.KEYCODE1=0x05; // nhấn phím 'b'
		  HIDkeyBoard.KEYCODE2=0x06; // nhấn phím 'c'
		  HIDkeyBoard.KEYCODE3=0x07; // nhấn phím 'd'
		  HIDkeyBoard.KEYCODE4=0x08; // nhấn phím 'e'
		  HIDkeyBoard.KEYCODE5=0x09; // nhấn phím 'f'
		  HIDkeyBoard.KEYCODE6=0x0A; // nhấn phím 'g'
		  USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&HIDkeyBoard, sizeof(HIDkeyBoard)); // Gửi Report đến máy tính
	  }
	  prevState1 = state1;
	  prevState2 = state2;
	  HAL_Delay(20);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
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
/* USER CODE BEGIN Includes */
#include "usbd_hid.h"
/* USER CODE END Includes */
```

Định nghĩa và khai báo:
```c++
typedef struct {
	uint8_t MODIFIER; // Phím điều khiển (Ctrl, Alt, Shift, v.v.)
	uint8_t RESERVED; // Byte dự phòng, không sử dụng, giá trị 0x00.
	uint8_t KEYCODE1; // Keycode 1-6: Các phím được nhấn
	uint8_t KEYCODE2;
	uint8_t KEYCODE3;
	uint8_t KEYCODE4;
	uint8_t KEYCODE5;
	uint8_t KEYCODE6;
}keyboardReport;

keyboardReport HIDkeyBoard = {0, 0, 0, 0, 0, 0, 0, 0};
extern USBD_HandleTypeDef hUsbDeviceFS;
```

Trong hàm main:
```c++
/* USER CODE BEGIN WHILE */
uint8_t prevState1 = 0, prevState2 = 0;
uint8_t state1, state2;
while (1)
{
/* USER CODE END WHILE */

/* USER CODE BEGIN 3 */
	state1 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	state2 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);
	if(prevState1 == 1 && state1 == 0) {
		HIDkeyBoard.KEYCODE1=0x00; // nhả phím
		USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&HIDkeyBoard, sizeof(HIDkeyBoard)); // Gửi Report đến máy tính
	}
	if(prevState1 == 0 && state1) {
		HIDkeyBoard.KEYCODE1=0x04; // nhấn phím 'a'
		USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&HIDkeyBoard, sizeof(HIDkeyBoard)); // Gửi Report đến máy tính
	}

	if(prevState2 == 1 && state1 == 0) {
		HIDkeyBoard.MODIFIER=0x00; // thả phím left shift
		HIDkeyBoard.KEYCODE1=0x00; // thả phím 'b'
		HIDkeyBoard.KEYCODE2=0x00; // thả phím 'c'
		HIDkeyBoard.KEYCODE3=0x00; // thả phím 'd'
		HIDkeyBoard.KEYCODE4=0x00; // thả phím 'e'
		HIDkeyBoard.KEYCODE5=0x00; // thả phím 'f'
		HIDkeyBoard.KEYCODE6=0x00; // thả phím 'g'
		USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&HIDkeyBoard, sizeof(HIDkeyBoard)); // Gửi Report đến máy tính
	}
	if(prevState2 == 0 && state2) {
		HIDkeyBoard.MODIFIER|= 1 << 1; // nhấn left shift
		HIDkeyBoard.KEYCODE1=0x05; // nhấn phím 'b'
		HIDkeyBoard.KEYCODE2=0x06; // nhấn phím 'c'
		HIDkeyBoard.KEYCODE3=0x07; // nhấn phím 'd'
		HIDkeyBoard.KEYCODE4=0x08; // nhấn phím 'e'
		HIDkeyBoard.KEYCODE5=0x09; // nhấn phím 'f'
		HIDkeyBoard.KEYCODE6=0x0A; // nhấn phím 'g'
		USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&HIDkeyBoard, sizeof(HIDkeyBoard)); // Gửi Report đến máy tính
	}
	prevState1 = state1;
	prevState2 = state2;
	HAL_Delay(20);
}
/* USER CODE END 3 */
```
### Quá trình nạp code và kết nối USB
- Nối dây USB-Type C vào cổng type C của CH340 để nạp code cho STM32 bằng STM32CubeProgrammer.
- Rút dây USB-Type C, kéo BOOT0 xuống 0.
- Nối dây USB-Type C vào cổng typeC của STM32.
- Vào "Bluetooth and other devices" xem nếu máy tính có nhận được keyboard chưa.![alt text](image03.png)