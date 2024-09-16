# Bài 2: Giao tiếp với IC 74HC595 dùng SPI

**74HC595** là IC thanh ghi dịch 8 bit, ngõ vào nối tiếp, ngõ ra song song. Cấu tạo và nguyên lý hoạt động của 74HC595 tương thích với giao thức SPI. Bài viết này sẽ hướng dẫn sử dụng ngoại vi SPI của STM32F103 để giao tiếp với IC 74HC595.

## Mục lục

- [Bài 2: Giao tiếp với IC 74HC595 dùng SPI](#bài-2-giao-tiếp-với-ic-74hc595-dùng-spi)
	- [Mục lục](#mục-lục)
	- [Kiến thức cần có](#kiến-thức-cần-có)
	- [Mục tiêu bài học](#mục-tiêu-bài-học)
	- [74HC595](#74hc595)
	- [Sơ đồ mạch điện](#sơ-đồ-mạch-điện)
	- [Project: Điều khiển 8 LED bằng thanh ghi dịch](#project-điều-khiển-8-led-bằng-thanh-ghi-dịch)
		- [Tạo project mới, cấu hình SPI và GPIO](#tạo-project-mới-cấu-hình-spi-và-gpio)
		- [Các hàm sử dụng](#các-hàm-sử-dụng)
		- [Code](#code)
	- [Câu hỏi và bài tập](#câu-hỏi-và-bài-tập)
		- [Câu hỏi](#câu-hỏi)
		- [Bài tập](#bài-tập)
	- [Tài liệu tham khảo](#tài-liệu-tham-khảo)

## Kiến thức cần có

- Series cơ bản
- Bài 1: Giới thiệu về giao thức SPI.

## Mục tiêu bài học

- Hiểu được cách giao tiếp, điều khiển 74HC595 qua SPI.

## 74HC595

**74HC595** là IC thanh ghi dịch 8 bit, ngõ vào nối tiếp, ngõ ra song song.

![alt text](<images/Screenshot 2024-09-16 at 13.46.25.png>)

**D Flip-flop** (viết tắt - DFF) là một mạch số, là một đơn vị lưu trữ dữ liệu nhỏ nhất. Nó có thể lưu được hai giá trị 0 hoặc 1. DFF gồm các chân D, Q, Q# (Q đảo) và CLK (chân có hình tam giác). Ngõ ra Q (và Q#) thể hiện giá trị đang lưu trong DFF. Ngõ vào D để cập nhật giá trị trong DFF. 

![alt text](<images/Screenshot 2024-09-16 at 14.09.35.png>)

Khi DFF hoạt động bình thường, giá trị trong DFF được lưu trữ và không thay đổi, không phụ thuộc vào tín hiệu D. **Tại thời điểm** khi tín hiệu CLK thay đổi từ 0 sang 1 (cạnh lên) thì giá trị của DFF được gán thành D.

![alt text](<images/Screenshot 2024-09-16 at 14.10.18.png>)

Tham khảo: [The D Flip-Flop (Quickstart Tutorial)](https://www.build-electronic-circuits.com/d-flip-flop/)

Mắc nối tiếp các DFF với nhau (ngõ ra của DFF này nối với ngõ vào của DFF kia), ta được **thanh ghi dịch** (shift register).

![alt text](<images/Screenshot 2024-09-16 at 14.12.25.png>)

Nguyên lý hoạt động của thanh ghi dịch: Khi tín hiệu CK cạnh lên, DFF sẽ lưu và gán giá trị nằm trong DFF ngay trước nó. Cụ thể: DFF thứ 3 sẽ lưu giá trị của DFF thứ 2, DFF thứ 2 sẽ lưu giá trị của DFF thứ 1, DFF thứ 1 sẽ lưu giá trị của ngõ vào SI. Kết quả là, tất cả các giá trị lưu trong các DFF sẽ dịch về sau 1 đơn vị.

![alt text](<images/Screenshot 2024-09-16 at 14.17.33.png>)

Tham khảo: [Sequential Logic: Shift Registers](https://toshiba.semicon-storage.com/ap-en/semiconductor/knowledge/e-learning/cmos-logic-basics/chap3/chap3-3-4.html)

Đó cũng chính là nguyên lý hoạt động cơ bản của 74HC595.

Bên trong 74HC595 có 8 DFF mắc nối tiếp nhau thành thanh ghi dịch 8 bits. Ngõ ra của mỗi DFF nối với một D-latch rồi nối tới cổng đệm ngõ ra. D-latch khá giống với DFF, chỉ khác là khi CLK = 1, D-latch sẽ ***luôn luôn*** cập nhật giá trị Q = D. D-latch được dùng như một bộ nhớ đệm, sau khi dịch các bit vào thanh ghi dịch, ta sẽ kích hoạt bộ đệm này để lưu dữ liệu.

> DFF chỉ cập nhật Q = D tại đúng thời điểm cạnh lên của CLK. D-latch cập nhật trong suốt khoảng thời gian CLK = 1.

![alt text](<images/Screenshot 2024-09-16 at 13.55.19.png>)

Mô tả các chân của 74HC595:


- VCC và GND: các chân cấp nguồn. Cấp nguồn từ 2V - 6V.
- Q0..7 (hoặc QA..H): các chân ngõ ra song song của thanh ghi dịch.
- DS: ngõ vào nối tiếp của thanh ghi dịch
- Q7': ngõ ra nối tiếp của thanh ghi dịch
- OE#: chân điều khiển ngõ ra song song. Khi OE# = 0 thì ngõ ra được kích hoạt. OE# = 1 thì ngõ ra trở kháng cao
- MR#: chân reset mạch. MR# = 0: reset thanh ghi dịch về 0
- SH_CP (hoặc SCLK, SCK): Chân clock của thanh ghi dịch. Khi chân này cạnh lên, thanh ghi dịch một đơn vị.
- ST_CP (hoặc LATCH): Chân latch. Khi ST_CP = 1, giá trị từ thanh ghi dịch lưu vào bộ đệm. ST_CP = 0, bộ đệm không thay đổi.

![alt text](<images/Screenshot 2024-09-16 at 13.49.49.png>)

Tham khảo [74HC595 datasheet](74HC595.PDF).

## Sơ đồ mạch điện

- PB15(MOSI) <---> DS.
- PB13(SCK) <---> SH_CP.
- PB10(CS#) <---> ST_CP.
- OE# <---> GND
- MR# <---> VCC
- Q0..Q7: nối LED nối tiếp điện trở.

![alt text](<images/Screenshot 2024-09-16 at 14.43.42.png>)

## Project: Điều khiển 8 LED bằng thanh ghi dịch

Project này sử dụng 74HC595 để điều khiển 8 LED, chỉ dùng 3 (hoặc 2) chân của STM32F103C8T6.

### Tạo project mới, cấu hình SPI và GPIO

Tạo project mới trên STM32CubeIDE. Trong giao diện của STM32CubeMX, bật SPI2 bằng cách chọn Mode là Half-Duplex Master. Các thông số còn lại để mặc định:

![alt text](<images/Screenshot 2024-09-16 at 14.55.21.png>)

Chọn chân PB10, chọn GPIO_Output. Trong bảng cấu hình của GPIO (System Core > GPIO), chọn chân PB10, sau đó chọn GPIO output level thành HIGH. Mục đích để khi vừa khởi tạo GPIO, ngõ ra của chân PB10 luôn ở mức 1:

![alt text](<images/Screenshot 2024-09-16 at 15.00.02.png>)

Lưu lại và tạo code.

### Các hàm sử dụng

```c++
/*
* Gửi dữ liệu qua SPI chế độ blocking
* hspi: con trỏ tới cấu trúc điều khiển spi
* pData: con trỏ tới dữ liệu muốn gửi đi
* Size: kích thước dữ liệu (số lượng byte)
* Timeout: thời gian chờ
* Giá trị trả về: trả về HAL_OK nếu gửi xong, trả về HAL_BUSY nếu SPI đang bận, HAL_ERROR nếu gặp lỗi, HAL_TIMEOUT nếu hết thời gian chờ mà chưa truyền xong.
*/
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size, uint32_t Timeout);
```

### Code

Khối `while()`:

```c++
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		/* Tạo biến a. Từ khóa static làm cho biến a lưu lại khi kết thúc vòng while */
		static uint8_t a = 0;

		/* Kéo chân LATCH xuống để bắt đầu truyền */
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);

		/* Dùng SPI gửi dữ liệu tới thanh ghi dịch. Gửi 1 byte (số a) qua spi2 */
		HAL_SPI_Transmit(&hspi2, &a, 1, 100);

		/* Đặt chân LATCH về như cũ */
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);

		a++;
		HAL_Delay(500);
	}
	/* USER CODE END 3 */
```

## Câu hỏi và bài tập

### Câu hỏi

1. Nguyên lý hoạt động của DFF và thanh ghi dịch?
2. Sự khác nhau của DFF và D-Latch
3. Nguyên lý hoạt động của 74HC595?

### Bài tập

1. Điều khiển 8 LED thành mạch đếm xuống.
2. Điều khiển 8 LED sáng dần tắt dần từ trái sang phải, rồi lặp lại.
3. Điều khiển 8 LED, 1 led sáng chạy từ phải sang trái
4. (Khó) Thiết kế mạch thanh ghi dịch 16 bit, dùng 2 IC 74HC595. Gợi ý: Q7' nối với DS. Các chân còn lại nối chung.

## Tài liệu tham khảo

[1] STMicroelectronics, "Medium-density performance line Arm®-based 32-bit MCU with 64 or 128 KB Flash, USB, CAN, 7 timers, 2 ADCs, 9 com. interfaces", STM32F103Cx8/B Datasheet, Sep. 2023.

[2] Philips Semiconductors, "8-bit serial-in, serial or parallel-out shift register with output latches; 3-state", Jun. 2003.