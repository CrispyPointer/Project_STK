/*********
  Modified from the examples of the Arduino LoRa library
  More resources: https://randomnerdtutorials.com
*********/

#include <SPI.h>
#include <LoRa.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SH1106 display(21, 22);
//define the pins used by the transceiver module
// SCK: GPIO 18
// MOSI: GPIO 23
// MISO: GPIO 19
#define nss 5
#define rst 14
#define dio0 2

//define pins use for UART
#define RXD2 16
#define TXD2 17
#define GPSBaud 9600

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXD2, TXD2);

unsigned long previousMillis = 0;
const long period = 5000;
int counter = 0;

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
  oled_setup();
}

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
    counter++;
    Serial.println(F("----------------------Sending Package---------------------"));
    loraTransmit();
    Serial.println(F("----------------------Ending Package----------------------"));
    previousMillis = currentMillis;
  }

  oled_display();
}

void transmitInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

void loraTransmit()
{
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(gps.location.lat(), 6);
  LoRa.print(F("-"));
  LoRa.print(gps.location.lng(), 6);
  LoRa.print(F("-"));
  LoRa.print(counter);
  LoRa.endPacket();
}

void oled_setup()
{
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display(); 
}

void oled_display()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("GPS - Reveiving: ");
  display.println(" ");
  
  display.print(gps.location.lat(), 6);
  display.print("-");
  display.println(gps.location.lng(), 6);

  display.println();
  display.println("Package transmit: ");
  display.print("#");
  display.println(counter);

  display.display();
}
