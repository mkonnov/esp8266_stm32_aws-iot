###### NOTE: all instructions are given in assumption that Linux is used as a host operating system

Building the firmwares
===

Clone this repository
---


```bash
git clone https://github.com/mkonnov/esp8266_stm32_aws-iot && cd esp8266_stm32_aws-iot 
git submodule update --init --recursive
```
Build the ESP8266 firmware
---
```bash
make -C esp-firmware
```

Build the STM32 firmware
---
```bash
make -C stm32-firmware/f302-temporary-project
```

Flashing the boards
===

Flashing the ESP8266 board
---
##### Certificates flashing
At first, we need to flash the certificates needed for AWS IoT connection.
Implying that we have created an appropriate 'thing' in AWS IoT and having its certificates downloaded, the following script is needed to flash them onto ESP device
```bash
cd esp-firmware/scripts
./flash_certs.py --partitions ../esp-firmware/partitions.csv --port /dev/ttyUSB0 --root-ca <path-to-rootCA> --cert <path-to-cert.pem.crt> --key <path-to-private.pem.key>
```
##### Firmware flashing
###### Since UART0 is used both for flashing and communication with NUCLEO board, the NUCLEO Reset (black) button should be pressed all the time while ESP firmware is flashing. Obviously, when another hardware will be used, STM32 nRST pin level should be kept low while flashing.
```bash
make -C esp-firmware flash
```

STM32 board flashing
---

For this I use `st-util`.  Any tool that you're using for loading the binary file can be used.
```bash
cd stm32-firmware/f32-temporary-project
st-flash write build/f302-temporary-project.bin 0x08000000
````


Connecting the boards
===

At the moment, for the prototyping the [ESP8266_DevKitc_V1](https://www.espressif.com/sites/default/files/documentation/ESP8266-DevKitC_getting_started_guide__EN.pdf) and [NUCLEO-F302R8](https://www.st.com/en/evaluation-tools/nucleo-f302r8.html)
 boards are used.

UART connection
---

ESP Pin | NUCLEO Pin | USB-UART converter
------- | ---------- | ------------------
RX | PC10 |
TX | PC11 |
2  |      | Rx

![Alt text](.img/IMG_20200528_205825.jpg?raw=true "Boards connection")

Connection to the host machine
---

###### As long as ESP serial port is left in its default value in menuconfig (/dev/ttyUSB0), then ESP board should be connected to the host first, in order to be detected as ttyUSB0. Other boards connection order is not important. Later instructions are given in assumption that USB-UART is detected as /dev/ttyUSB1.

The debug log from both boards can be seen using any serial communication util. Refer to the table below for port names and baud rates.

Debug purpose | Port | Baud rate
------------- | ---- | ---------
ESP logs      | /dev/ttyUSB1 | 115200
STM32 logs    | /dev/ttyACM0 | 38400


Running the firmwares
===

After both boards are wired and connected to the host, the blue NUCLEO button should be pressed to trigger the WPS session. ESP logs should report the successful connection.

The following should be seen at ESP logs:

![Alt text](.img/Screenshot_2020-05-29_18-44-57.png?raw=true "ESP logs")

Further, device connects to AWS and subscribes to appropriate topics. Logs as below.

![Alt text](.img/Screenshot_2020-05-29_18-52-21.png?raw=true "ESP logs")

### Troubleshooting

##### AWS SSL certificates are not flashed

If you see the logs as below or similar, then most probably the certificates are not flashed.

![Alt text](.img/Screenshot_2020-05-29_19-27-55.png?raw=true "ESP logs")

##### AWS SSL certificates flashed are not related to the endpoint or revoked

Logs below or similar indicates that certs are flashed and correctly parsed, but AWS declines the connection

![Alt text](.img/Screenshot_2020-05-29_19-35-55.png?raw=true "ESP logs")
=======
Running the unit tests
===
```bash
make -C tests
```
The similar output should be seen on successful unit tests pass. This outputs will evolve with a project, but each test group should return status 'OK' and 0 failures.

![Alt text](.img/Screenshot_2020-06-02_21-25-20.png?raw=true "ESP logs")

ESP8266 WiFi provisioning
===
Can be done in a 2 different ways:
- Direct creds sending
- Via WPS configuration

These options are mutually exclusive and the appropriate one should be chosen in [common_defs.h](common/common_defs.h)
WiFi credentials should be also set there.


Testing the system with AWS
===
In order to simulate the valves opening/closing, you should publish the appropriate message to an AWS topic related to ESP8266 MAC address. These can be seen in the ESP8266 debug logs. In my case they was `2EF43291670/commands/open` and `2EF43291670/commands/close`respectively.
The payload like shown below should be sent to control the valves:
```
{
  "data": [0,1,1,0,1,1,1,0]
}
```
Each element index represents the index of controlling valve (starting from 0). In case above, they was 1,2,4,5, and 6.

