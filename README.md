## Health Monitoring System with Ward Management

The system measures the temperature, blood oxygen and pulse. This is combined with a ward management system with the use of RFID system, where the doctor can scan the RFID card before and while leaving the room in order to log the visiting time.

NodeMCU ESP8266 is used as the microcontroller for both systems.

### Sensors

1.  [MAX30100](https://robu.in/product/max30100-pulse-oximeter-heart-rate-sensor-module/) - Pulse oximeter 
2.  [DS18B20](https://www.electronicscomp.com/ds18b20-water-proof-temperature-sensor-probe-india) - Temperature sensor
3.  [RC522](https://www.electronicscomp.com/rc522-rfid-13.56mhz-reader-writer-module-india) - RFID sensor

### Libraries Used

#### For Pulse Oximetere and Temperature Sensor

1.  ESP8266WiFi
2.  BlynkSimpleEsp8266
3.  OneWire
4.  DallasTemperature
5.  Wire
6.  MAX30100\_PulseOximeter
7.  SimpleTimer

#### For RFID 

1.  NTPClient
2.  SPI
3.  MFRC522
4.  WiFiUdp
5.  Firebase\_ESP\_Client
6.  TokenHelper
7.  RTDBHelper


The health monitoring and ward management systems are independent of each other, and have separate connections.

Blynk is used as the front end for activating and monitoring the output from the sensors.

![Blynk Console](https://github.com/Uttkarsh09/IoT-Health-Monitoring-System-with-Ward-Management/blob/master/Photos/Blynk%20Console.png)


## Setting up

You can either use Arduino IDE or VSCode with the Arduino Extension.

1.  [Arduino Setup](https://www.instructables.com/How-to-Program-NodeMCU-on-Arduino-IDE/)
2.  Vscode 
    1.  Install the Arduino extension in VSCode.
    2.  Press ctrl+shift+p and type - “arduino initialize”, press enter and choose the following:
        1.  Enter the file name for your .ino file.
        2.  Board - NodeMCU 1.0 (ESP-12E Module).
        3.  Port - Depends on your machine.
        4.  Baud Rate - 115200
    3.  This will create an arduino.json file in .vscode folder.
    4.  Press ctrl+shift+p and type - “arduino library manager” and install all the above mentioned libraries.
