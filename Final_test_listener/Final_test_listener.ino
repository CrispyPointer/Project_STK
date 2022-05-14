/*********
  Modified from the examples of the Arduino LoRa library
  More resources: https://randomnerdtutorials.com
*********/

#include <SPI.h>
#include <LoRa.h>
#include <TinyGPSPlus.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip

//define the pins used by the lora transceiver module
// SCK: GPIO 18
// MOSI: GPIO 23
// MISO: GPIO 19

#define nss 15    //slave selected
#define rst 14
#define dio0 12

// The TinyGPSPlus object
TinyGPSPlus gps;

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
// Cs:  5
// RST: 4
// DC:  2
// LED: 3.3V

char const *gpsStream;
String LoRaData;
int counter = 0;

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  setup_lora();
  setup_tft();
}

void loop() {
  receive_lora();
  tft_display();
}

void setup_lora()
{
  while (!Serial);
  Serial.println("LoRa Receiver");

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

void setup_tft()
{
  tft.init();
  tft.setRotation(3); //landscape orientation [1 or 3]
}

void receive_lora()
{
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
//    Serial.print(F("Received: "));
    // read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      counter++;
//      Serial.print(F("package #"));
//      Serial.print(counter++);
//      Serial.print(F(" "));
//      Serial.println(LoRaData);
    }
  }
}

void tft_display()
{
  //Fill screen with black
  tft.fillScreen(TFT_BLACK);

  //Set address window
  tft.setAddrWindow(0, 0, 160, 128);
  
  // Set "cursor" at top left corner of display (0,0) and select font 2
  // (cursor will move to next line automatically during printing with 'tft.println'
  //  or stay on the line is there is room for the text with tft.print)
  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_RED,TFT_BLACK); tft.setTextFont(2);  tft.setTextSize(2);
  tft.println("  P-STK  ");
  
  tft.setTextColor(TFT_YELLOW,TFT_BLACK);  tft.setTextSize(1);
  tft.println("Receiving location: ");
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);  tft.setTextSize(1);
  tft.println(LoRaData);

  tft.setTextColor(TFT_WHITE,TFT_BLACK);   tft.setTextSize(1);
  tft.print("Packages received #");
  tft.print(counter);
  tft.println(" ");
  yield();
}
