/*
  =======================================
  WIRING INSTRUCTIONS FOR SHT1x SENSOR
  =======================================
  
  Connect the SHT1x sensor to the Arduino Uno as follows:

  - **Black – GND**: Connect the GND pin of the sensor to the GND pin of the Arduino.
  - **Red – 5V**: Connect the VCC pin of the sensor to the 5V pin of the Arduino.
  - **Yellow – Clock**: Connect the Clock pin of the sensor to Arduino pin 11 (clockPin in the sketch).
  - **Green – Data**: Connect the Data pin of the sensor to Arduino pin 10 (dataPin in the sketch).
  
  =======================================
  
  SHT1x Sensor Pinout:

  1. GND  (Black)   -> Ground
  2. VCC  (Red)     -> 5V Power Supply
  3. SCK  (Yellow)  -> Clock (connect to Arduino pin 11)
  4. DATA (Green)   -> Data (connect to Arduino pin 10)

  =======================================

  Steps to Run:

  1. Connect the SHT1x sensor according to the wiring instructions above.
  2. Upload this sketch to your Arduino Uno.
  3. Open the Arduino Serial Monitor (set baud rate to 9600).
  4. Watch the temperature (in Celsius and Fahrenheit) and humidity values print out every second.

  =======================================
  
  Written by Lucienne Swart
  This is an adaptation from the Beegee-Tokyo / SHT1x-ESP library.
*/

#include <SHT1x.h>

// Pin configuration for the SHT1x sensor
const uint8_t dataPin = 10;   // Data pin for SHT1x (Green wire connected here)
const uint8_t clockPin = 11;  // Clock pin for SHT1x (Yellow wire connected here)

// Create an instance of the SHT1x class
SHT1x sht1x(dataPin, clockPin, SHT1x::Voltage::DC_5_0v);

void setup() {
  // Initialize serial communication for debugging (set baud rate to 9600)
  Serial.begin(9600);

  // Wait for the serial monitor to connect (necessary on some boards)
  while (!Serial) {
    ; // Wait for the serial connection to be established
  }

  // Print a starting message to the serial monitor
  Serial.println("SHT1x sensor test started!");
}

void loop() {
  // Read the temperature in Celsius
  float temperatureC = sht1x.readTemperatureC();
  // Read the temperature in Fahrenheit
  float temperatureF = sht1x.readTemperatureF();
  // Read the humidity
  float humidity = sht1x.readHumidity();

  // Print the temperature and humidity to the serial monitor
  Serial.print("Temperature (C): ");
  Serial.print(temperatureC);
  Serial.print(" °C, ");

  Serial.print("Temperature (F): ");
  Serial.print(temperatureF);
  Serial.print(" °F, ");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Wait for 1 second before the next reading
  delay(1000);
}


