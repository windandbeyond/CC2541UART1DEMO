## TI CC2541 BLE UART1 on HM-10 BLE module DEMO
This is wrote for understanding the UART registor's on TI CC254x BLE chip. Compile wit IAR studio.
### Hardware needed
* HM-10 BLE module
* USB serial adapter, FT232RL etc. 
* Firmware flashing tool
  * CC debuger
  * or CCloader with adruino https://github.com/RedBearLab/CCLoader
### UART setting
8N1, 115200
### UART command
type "1#", "2#" to turn LED on/off, type "3#" to display hello message. 
