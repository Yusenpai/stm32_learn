# Bài 6: Cấu hình cây clock của STM32F103C8

Bài viết này hướng dẫn cấu hình cây clock của vi điều khiển STM32F103C8 trên phần mềm STM32CubeMX.

## Mục lục

- [Bài 6: Cấu hình cây clock của STM32F103C8](#bài-6-cấu-hình-cây-clock-của-stm32f103c8)
	- [Mục lục](#mục-lục)
	- [Kiến thức cần có](#kiến-thức-cần-có)
	- [Mục tiêu bài học](#mục-tiêu-bài-học)
	- [Cây clock](#cây-clock)
		- [Nguồn của xung clock](#nguồn-của-xung-clock)
		- [Xung clock trong hệ thống: SYSCLK, HCLK, PCLK1 và PCLK2](#xung-clock-trong-hệ-thống-sysclk-hclk-pclk1-và-pclk2)
		- [Miền AHP và APB](#miền-ahp-và-apb)
	- [Cấu hình cây clock trên giao diện STM32CubeMX](#cấu-hình-cây-clock-trên-giao-diện-stm32cubemx)
	- [Câu hỏi và bài tập](#câu-hỏi-và-bài-tập)
		- [Câu hỏi](#câu-hỏi)
		- [Bài tập](#bài-tập)
	- [Tài liệu tham khảo](#tài-liệu-tham-khảo)

## Kiến thức cần có

- [Bài 1 - Giới thiệu vi điều khiển STM32, sử dụng STM32CubeIDE và thư viện HAL](<../Bài 1/Bài 1 - Giới thiệu vi điều khiển STM32, sử dụng STM32CubeIDE và thư viện HAL.md>)

## Mục tiêu bài học

- Học cách cấu hình xung clock trên vi điều khiển STM32F103C8

## Cây clock

**Cây clock** (clock tree) là một biểu đồ thể hiện đường đi của xung clock, nguồn cấp xung clock, nơi nhận xung clock, tần số của xung clock,... mà ta có thể điều chỉnh để phù hợp với ứng dụng của mình.

![alt text](<images/Screenshot 2024-09-24 at 16.33.11.png>)

### Nguồn của xung clock

Trên vi điều khiển STM32F103C8 có nhiều nguồn xung clock, mỗi nguồn đều được đặt tên:

- **HSE**: (High Speed External) Dao động thạch anh ngoài, 4 - 16MHz
- **HSI**: (High Speed Internal) Dao động RC nội 8MHz
- **LSE**: (Low Speed External) Dạo động thạch anh ngoài 32.768kHz
- **LSI**: (Low Speed Internal) Dao động RC nội 40kHz


LSI và LSE được dùng cho mạch thời gian thực bên trong vi điều khiển, hoặc làm nguồn clock cho WDT.

HSI và HSE được dùng làm nguồn xung cấp cho cả hệ thống. Cả hai nguồn có thể đi qua khối PLL (Phase Lock Loop) để nhân tần số lên gấp nhiều lần. Ngõ ra của xung PLL gọi là **PLLCLK**.

### Xung clock trong hệ thống: SYSCLK, HCLK, PCLK1 và PCLK2

**SYSCLK** chính là xung cấp cho CPU hoạt động. Xung này được lấy từ 1 trong 3 nguồn: HSE, HSI hoặc PLLCLK. Xung SYSCLK giới hạn ở 72MHz.



### Miền AHP và APB

Trên vi điều khiển STM32F103C8 chia thành 2 miền lớn: **miền AHB và miền APB**. 

**Miền AHB** (Advanced High-performance Bus) là một Bus hoạt động ở tần số cao. Bus này kết nối các khối:
- CPU
- SRAM
- FLASH
- DMA

Miền AHB được cấp bởi xung **HCLK**.

**Miền ABP** (Advanced Peripheral Bus) là một Bus đơn giản hơn AHB, hoạt động ở tần số thấp hơn hoặc bằng AHB. Bus chia làm hai bus nhỏ: **APB1** và **APB2**, được cấp xung tương ứng là **PCLK1** và **PCLK2**

APB1 kết nối các khối:
- RTC
- TIM2,3,4
- USART2,3
- SPI2
- I2C1,2
- CAN
- USB
- WWDG

APB2 kết nối các khối
- GPIO
- TIM1
- SPI1
- USART1
- ADC

Giao tiếp giữa hai miền AHB và APB sử dụng cầu AHB-APB.

Các xung HCLK và PCLK1, PCLK2 được chia từ xung SYSCLK. HCLK và PCLK2 có tần số tối đa là 72MHz, trong khi đó PCLK1 là 36MHz. Ngoại lệ là các Timer, tần số cấp cho các Timer tối đa luôn là 72MHz dù Timer đó kết nối tới PCLK1.

## Cấu hình cây clock trên giao diện STM32CubeMX

Cấu hình cây clock ở tab *Clock Configuration*. Mặc định tần số của các xung clock:
- HSI: 8MHz
- HSE: không sử dụng
- LSI: 40KHz
- LSE: không sử dụng
- PLLCLK: không sử dụng
- SYSCLK: lấy từ HSI, tần số 8MHz.
- HCLK = PCLK1 = PCLK2 = SYSCLK = 8MHz.

![alt text](<images/Screenshot 2024-09-24 at 16.46.07.png>)

HSE và LSE là hai nguồn xung tới từ thạch anh ngoài, nếu muốn sử dụng cần phải cấu hình khối RCC của vi điều khiển. Trong [Pinout & Configuration] > [System Core] > [RCC], ở dòng *High Speed Clock* và *Low Speed Clock*, chọn *Crystal/Ceramic Resonator*.

![alt text](<images/Screenshot 2024-09-24 at 16.49.28.png>)

Quay lại tab *Clock Configuration*. Quá trình cấu hình xung clock theo các bước:

1. Chọn nguồn: HSE hay HSI?
2. Chọn xung SYSCLK: tần số bao nhiêu? Chọn HSE hoặc HSI nếu trùng tần số. Nếu yêu cầu tần số cao hơn thì chọn PLLCLK
3. Chọn hệ số nhân của PLLCLK phù hợp để đạt được tần số SYSCLK mong muốn. Nếu muốn sử dụng USB thì chỉ có thể chọn PLLCLK là 48MHz hoặc 72MHz.
4. Chọn tần số HCLK và PCLK1/2

Ví dụ 1:
- Sử dụng thạch anh ngoài HSE 16MHz
- SYSCLOCK = 72MHz
- HCLK = PCLK2 = 72MHz
- PCLK1 = 36MHz
- Sử dụng USB
  
![alt text](<images/Screenshot 2024-09-24 at 17.01.57.png>)

Ví dụ 2:
- Sử dụng thạch anh ngoài HSE 8MHz
- SYSCLOCK = 16MHz
- HCLK = PCLK2 = 8MHz
- PCLK1 = 8MHz
- Không sử dụng USB

![alt text](<images/Screenshot 2024-09-24 at 17.05.28.png>)

Trong khi cấu hình cây clock, có thể gặp lỗi:
- Xung clock vượt quá giới hạn đối với SYSCLK, HCLK, PCLK1/2
- Xung clock không chính xác đối với USB. USB chỉ có thể cấp xung clock là 48MHz và sử dụng thạch anh ngoài.

Phần mềm STM32CubeMX có thể tự tính toán và đưa ra thông số phù hợp nếu gặp phải lỗi. Ở góc trên màn hình, chọn `Resolve Clock Issues`.

![alt text](<images/Screenshot 2024-09-24 at 17.10.34.png>)

## Câu hỏi và bài tập

### Câu hỏi

1. Kể tên các nguồn xung clock của STM32F103C8
2. Kể tên các miền bus. Các miền bus kết nối các khối nào với nhau ?
3. Tần số tối đa của PCLK1 và PCLK2 là bao nhiêu?

### Bài tập

1. Cấu hình xung clock sau: sử dụng HSE 16MHz, sử dụng USB, HCLK = 48MHz, PCLK1 = 24MHz, PCLK2 = 48MHz.
2. Cấu hình xung clock sau: sử dụng HSI 8MHz, không sử dụng USB, tần số hoạt động của CPU là 125kHz. Các xung khác tuỳ ý.

## Tài liệu tham khảo

[1] STMicroelectronics, "Medium-density performance line Arm®-based 32-bit MCU with 64 or 128 KB Flash, USB, CAN, 7 timers, 2 ADCs, 9 com. interfaces", STM32F103Cx8/B Datasheet, Sep. 2023.