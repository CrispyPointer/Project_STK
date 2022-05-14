# Project_STK - ESP32 - GPS - LORA
* A project collecting GPS signal and transmit information between two ESP32 through Lora

* ## Wiring Lora-ESP-GPS:
  ![Wiring diagram](https://github.com/phuocly2304/Project_STK/blob/main/figure/Sketch_1.png)

   ![Wiring diagram](https://github.com/phuocly2304/Project_STK/blob/main/figure/Lora_ESP.jpg)

* Note: GPS RX TX pins are connected to ESP32 TXD2 and RXD2 (pin 16 17)

###### The full arduino code are included inside :open_file_folder: [final_test_talker](https://github.com/phuocly2304/Project_STK/blob/main/Final_test_talker/Final_test_talker.ino) and :open_file_folder: [final_test_listener](https://github.com/phuocly2304/Project_STK/tree/main/Final_test_listener/Final_test_listener.ino)

### Snipped code for *Talker* explain:
> **Prerequisite library**
```C++
#include <SPI.h>
#include <LoRa.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
```

> Set up for both **Lora** module and **GPS**
> ###### Note: Put GPS module outside for proper signal receiving 
```C++
void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  ss.begin(GPSBaud);
  
  while (!Serial);
  Serial.println("LoRa Sender");
 
  //setup LoRa transceiver module
  LoRa.setPins(nss, rst, dio0);
  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(866E6)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}
```

> **Start transceiving signal**
> * GPS signal will be constantly receive through UART
> * Every 5 seconds, ESP will transmit the location to the other through **Lora**.

```C++
void loop() {
  unsigned long currentMillis = millis(); // store the current time
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      transmitInfo();

  //Wait after first 5 second for GPS connection
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }

  //Sending package every 5 second
  if(currentMillis - previousMillis >= period){
    Serial.println(F("----------------------Sending Package---------------------"));
    loraTransmit();
    Serial.println(F("----------------------Ending Package----------------------"));
    previousMillis = currentMillis;
  }
}
```
### Snipped code for *Listener* explain:

```C++
void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print(F("Received: "));
    // read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.println(LoRaData);
    }
  }
}
```

### First result
![First result](https://github.com/phuocly2304/Project_STK/blob/main/figure/lora_gps_result.jpg)

### Remaining tasks:
* Display data on TFT screen
* Test for connecting distance (A little tricky!!) 

### Second result:
###### The code has been updated so that it could be connected to the TFT and OLED display

 ***Prerequisite library***
> *For OLED display:*
```C++
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
```

> For TFT Display:
```C++
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
```
* Note: Screen driver and pins must be set up in User_Setup.h
```C++
// Find these setups inside the .h file and uncomment them
#define ST7735_DRIVER

#define TFT_WIDTH  128
#define TFT_HEIGHT 160

#define ST7735_REDTAB

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS    5  // Chip select control pin
#define TFT_DC    2  // Data Command control pin
#define TFT_RST   4  // Reset pin (could connect to RST pin)
```
#### Distance and display testing
###### This is the test to find out how far can the lora modules communicate with each other. The module is believed to reliably transmit and receive data more than 200 meters with basic setting. The test was placed at a park with trees and bushes as obstacles between two devices.

* First result has shown that it could communicate stably at approximately 250 meters.

![Transmit device](https://github.com/phuocly2304/Project_STK/blob/main/figure/transmit_lora.jpg)

![Receive device](https://github.com/phuocly2304/Project_STK/blob/main/figure/received_lora.jpeg)

* After 250 meter, signal got interference and requires a few reset to received data. *(Probably because of the trees and bushes between)*
  
![Max distance where signal was stable](https://github.com/phuocly2304/Project_STK/blob/main/figure/Test_distance_01.jpg)

* After 300 meter, the signal got lost completely

