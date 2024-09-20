# Bài 1: Giới thiệu về giao thức truyền thông UART

**UART** (Universal Asynchronous Receiver/Transmitter) là giao thức truyền thông nối tiếp không đồng bộ, được sử dụng để truyền và nhận dữ liệu giữa các thiết bị.

Bài ngày hôm nay sẽ tìm hiểu sâu về giao thức UART, cách sử dụng UART trên vi điều khiển STM32F103.

## Mục lục

- [Bài 1: Giới thiệu về giao thức truyền thông UART](#bài-1-giới-thiệu-về-giao-thức-truyền-thông-uart)
	- [Mục lục](#mục-lục)
	- [UART - Universal Asynchronous Receiver/Transmitter](#uart---universal-asynchronous-receivertransmitter)
	- [Khung truyền dữ liệu của UART](#khung-truyền-dữ-liệu-của-uart)
	- [USART](#usart)
	- [USART trên vi điều khiển STM32F103](#usart-trên-vi-điều-khiển-stm32f103)
	- [Cấu hình USART trên STM32 với STM32CubeMX](#cấu-hình-usart-trên-stm32-với-stm32cubemx)
	- [Sử dụng thư viện HAL\_UART](#sử-dụng-thư-viện-hal_uart)

## UART - Universal Asynchronous Receiver/Transmitter

**UART (Universal Asynchronous Receiver/Transmitter)** là giao thức truyền thông nối tiếp không đồng bộ, được sử dụng để truyền và nhận dữ liệu giữa các thiết bị. Không giống như giao thức đồng bộ như SPI hoặc I2C, UART không yêu cầu một tín hiệu xung nhịp chung giữa các thiết bị. UART thường được sử dụng trong các ứng dụng như giao tiếp giữa vi điều khiển và máy tính hoặc các module giao tiếp nối tiếp như module Bluetooth, GPS, v.v.

Các đặc điểm của UART:
- **Truyền dữ liệu nối tiếp**: Dữ liệu được truyền từng bit qua một dây.
- **Không đồng bộ**: Không có xung nhịp (clock) chung giữa máy gửi và máy nhận. Giữa máy gửi và máy nhận phải cùng đồng ý cùng sử dụng một tốc độ, gọi là **tốc độ baud**
- **Hỗ trợ chế độ song công**: UART sử dụng hai dây dẫn: TX và RX. TX dùng để truyền dữ liệu còn RX dùng để nhận dữ liệu. Do đó UART có thể truyền và nhận dữ liệu cùng lúc.

## Khung truyền dữ liệu của UART

Dữ liệu được truyền đi bằng giao thức UART theo quy tắc đặc biệt, gọi là khung truyền:

![alt text](images/1-2.png)

Một khung truyền của UART gồm:
- St: Start bit - bit bắt đầu, luôn ở mức thấp. 
- (n): Databit - từng bit của dữ liệu. Bit trọng số thấp nhất (LSB) truyền đi trước. Số lượng databit có thể thay đổi (từ 5-8)
- P: Parity - bit dùng để kiểm tra lỗi của frame. Có thể chẵn, lẻ, hoặc không có
- Sp: Stop bit - bit dừng, luôn ở mức cao. Dài từ 1 - 2 bit.
- IDLE: Trạng thái nghỉ - Khi UART không truyền dữ liệu, trạng thái các chân TX, RX luôn ở mức cao.

Để hai thiết bị dùng UART có thể giao tiếp với nhau, các thông số sau phải sử dụng chung:

- Mức điện áp
- Tốc độ baud (baudrate)
- Số lượng databit
- Parity
- Số lượng Stop bit

## USART

USART (Universal ***Synchronous*** Asynchronous Receiver/Transmitter) là giao thức mở rộng của UART, có thêm tín năng truyền dữ liệu đồng bộ (có thêm một chân clock). Giao thức này tương thích với UART, ngoài ra còn hỗ trợ một số giao tiếp đồng bộ khác.

## USART trên vi điều khiển STM32F103

Trên vi điều khiển STM32F103 có ba ngoại vi USART hoạt động độc lập nhau

Mỗi ngoại vi có thể được lập trình để giao tiếp ở các chế độ:

- Asynchronous: chế độ không đồng bộ
- Synchronous: chế độ đồng bộ
- Single Wire: chế độ nhận và truyền trên cùng một dây dẫn (bán song công)
- Multiprocessor communication: chế độ giao tiếp đa thiết bị
- IrDA: chế độ giao tiếp qua hồng ngoại
- SmartCard: chế độ giao tiếp với smartcard (trong thẻ ATM, thẻ SIM,...)

## Cấu hình USART trên STM32 với STM32CubeMX

Sau khi tạo project mới trong STM32CubeIDE, mở tập tin .ioc sẽ tự động chuyển sang giao diện Device Configuration (STM32CubeMX).

Trong giao diện này, tìm tới [Pinout & Configuration] > [Connectivity] > [USART1]. Chọn Mode là *Asynchronous*. 

![alt text](<images/Screenshot 2024-09-20 at 16.14.41.png>)

Bảng *Configuration* hiện ra. Ở đây có thể tìm thấy các thông số của UART:

- Basic Parameters:
  - Baudrate: tốc độ baud
  - Word Length: số lượng databit + số lượng parity bit
  - Parity: chẵn (Even), lẻ (Odd) hoặc không có (None)
  - Stop bits: số lượng stop bit
- Advanced Parameters:
  - Data direction: chiều đi của dữ liệu. Chỉ truyền, chỉ nhận hoặc cả hai.
  - Oversampling: lấy mẫu quá mức

> Lưu ý: Nên điều chỉnh các thông số trong Basic Paremeters cho phù hợp. Thông số trong Advanced Parameters có thể để mặc định.

Ngoài ra các chân truyền UART được sử dụng là `PA9` (TX) và `PA10` (RX). Có thể nhấn Ctrl + Click + giữ chuột vào chân `PA9` để xem các chân thay thế (chân `PB6`). Tương tự có thể đổi chân RX sang `PB7`.

![alt text](<images/Screenshot 2024-09-20 at 16.24.37.png>)

> Lưu ý: Để đổi chân TX sang PB6, click chuột vào chân PB6 rồi chọn USART1_TX

## Sử dụng thư viện HAL_UART

