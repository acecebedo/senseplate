//include libraries
#include <Arduino.h>
#include "HX711.h"
#include "soc/rtc.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Pushbutton.h>
#include "BluetoothSerial.h"

// HX711 amplifier circuit pins
const int loadcell_data_pin = 16;
const int loadcell_sck_pin = 4;

HX711 scale;
int value;
int lastValue;

// calibration factor determined by raw reading / known weight of object
#define CALIBRATION_FACTOR -424.466

//OLED Display variables and display initialization
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//initialise BT
BluetoothSerial ESP_BT;

//power LED pin
const int powerLED = 32;

//tactile buttons
#define buttn1 33
#define buttn2 34
Pushbutton button1 (buttn1);
Pushbutton button2 (buttn2);


void setup() {
  Serial.begin(115200);
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(2000);      //delay for stability
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  Serial.println("Initializing the scale");
  scale.begin(loadcell_data_pin, loadcell_sck_pin);
  scale.set_scale(CALIBRATION_FACTOR);   // this value is obtained by calibrating the scale with known weights
  scale.tare();               // reset the scale to 0

  //bluetooth setup
  ESP_BT.begin("Senseplate");

  //pin setup
  pinMode (powerLED, OUTPUT);
}

void loop() {
  //powerLED to stay high as long as there is power
    digitalWrite(powerLED, HIGH);
    
  //checks if button is pressed then calls tare method of HX711 instance 'scale'
  if (button1.getSingleDebouncedPress()){
    scale.tare();
  }
  
  //loop to use weight value in oled and in bluetooth signal
    value = round(scale.get_units());
    Serial.println(value);
    ESP_BT.println(value);
    if (value != lastValue){
      oledDisplay(value); 
    }
    lastValue = value;
}

void oledDisplay(int weight){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);         //reposition cursor (where text will be)
  display.setCursor(0, 10);
  display.setTextSize(2);
  display.print(weight);
  display.print(" ");
  display.print("g");
  display.display();  
}
