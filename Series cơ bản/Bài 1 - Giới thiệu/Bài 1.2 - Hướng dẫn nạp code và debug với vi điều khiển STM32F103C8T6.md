# Bài 1.2: Hướng dẫn nạp code và debug với vi điều khiển STM32F103C8T6

## Mục lục

- [Bài 1.2: Hướng dẫn nạp code và debug với vi điều khiển STM32F103C8T6](#bài-12-hướng-dẫn-nạp-code-và-debug-với-vi-điều-khiển-stm32f103c8t6)
	- [Mục lục](#mục-lục)
	- [Nạp code và debug cho STM32F103C8T6](#nạp-code-và-debug-cho-stm32f103c8t6)
	- [Nạp code qua STM32CubeProgrammer](#nạp-code-qua-stm32cubeprogrammer)
	- [Nạp code và debug với STM32CubeIDE](#nạp-code-và-debug-với-stm32cubeide)

## Nạp code và debug cho STM32F103C8T6

STM32F103C8T6 hỗ trợ một vài cách nạp code và debug:

1. Dùng ngoại vi USART1 ở chân PA9 (TX) và PA10 (RX), kết hợp với IC chuyển USB - UART (CH340). Sử dụng phần mềm STM32CubeProgrammer. Cách này chỉ có thể nạp code, không thể debug.
2. Dùng STLink kết nối với cổng SWD của STM32F103C8T6. Sơ đồ kết nối:

	| STLink | STM32F103C8T6 |
	| ---	| ---|
	| SWDIO | SWDIO|
	| SWCLK | SWCLK |
	| GND | GND |
	| +3.3V | +3.3V |

	> Lưu ý: tại một thời điểm chỉ được cấp một nguồn. Ví dụ nếu đã cắm dây USB từ máy tính thì đừng nối dây +3.3V từ STLink nữa.

	Dùng STLink vừa có thể nạp code (qua STM32CubeProgrammer, STM32CubeIDE) vừa có thể debug (STM32CubeIDE).

## Nạp code qua STM32CubeProgrammer

**STM32CubeProgrammer** là một công cụ giúp người dùng nạp code qua UART, USB DFU, STlink, JTAG

Tải STM32CubeProgrammer qua [link](https://www.st.com/en/development-tools/stm32cubeprog.html).

![alt text](<images/Screenshot 2024-11-22 at 19.07.57.png>)

Bên cạnh dòng Serial number, chọn nút refresh để tìm STLink. Nếu STLink kết nối thành công thì mã serial của STLink sẽ hiện ở ô bên cạnh:

![alt text](<images/Screenshot 2024-11-22 at 19.12.53.png>)

Nếu máy tính không nhận STLink, kiểm tra *Device Manager* và cài [STLink Server](https://www.st.com/en/development-tools/st-link-server.html).

Trên STM32, đặt Boot0 = 1, Boot1 = 0, rồi nhấn reset để đưa về chế độ bootloader (nạp code). Trên STM32CubeProgrammer, bấm **Connect**. Nếu thành công, bộ nhớ của STM32 sẽ được đọc và hiển thị trên cửa sổ *Device memory*.

Sau khi đã kết nối xong, ở cột bên trái, chọn ô thứ hai. Đây là cửa sổ nạp code. Chọn **Browse**, tìm tới file đuôi .elf hoặc đuôi .hex cần nạp (thường nằm trong thư mục Debug):

![alt text](<images/Screenshot 2024-11-22 at 19.20.01.png>)

Cuối cùng, chọn **Start Programming**, chờ thông báo thành công. Bấm **Disconnect**, chuyển Boot0 = 0, rồi nhấn nút reset để chạy code vừa nạp.

Nếu sử dụng giao tiếp UART để nạp code, thao tác tương tự với khi nạp bằng STLink. Bên cạnh nút **Connect**, bấm cửa sổ kéo xuống, chọn **UART**:

![alt text](<images/Screenshot 2024-11-22 at 19.24.45.png>)

Đưa mạch về chế độ bootloader, rồi thao tác tương tự.

## Nạp code và debug với STM32CubeIDE

STM32CubeIDE tích hợp nạp code và debug với vi điều khiển STM32. Phần mềm yêu cầu STLink được kết nối với máy tính.

Tham khảo [video](https://www.youtube.com/watch?v=jbVgIK9Jlgk).