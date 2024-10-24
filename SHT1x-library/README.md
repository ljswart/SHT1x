# SHT1x Arduino Library

## Overview
The SHT1x library allows you to interface with the Sensirion SHT1x series (SHT10, SHT11, SHT15) temperature and humidity sensors. This is an adaptation of the SHT1x-ESP library by Beegee-Tokyo, modified for use with Arduino by Lucienne Swart.

## Features
- Read temperature in Celsius and Fahrenheit
- Read humidity with temperature correction
- Works with SHT1x series sensors (SHT10, SHT11, SHT15)
  
## Installation

1. Download the ZIP file from [GitHub](https://github.com/ljswart/SHT1x-library).
2. Open the Arduino IDE.
3. Go to `Sketch` > `Include Library` > `Add .ZIP Library...`.
4. Select the downloaded ZIP file.
5. The library will now be available in the Arduino IDE.

## Pinout

| Sensor Pin | Arduino Pin |
|------------|-------------|
| GND (Black)| GND         |
| VCC (Red)  | 5V          |
| SCK (Yellow)| Pin 11      |
| DATA (Green)| Pin 10      |

## Example

Here is a simple example that reads temperature and humidity from the sensor:

```cpp
#include <SHT1x.h>

const uint8_t dataPin = 10;   // Data pin for SHT1x (Green wire)
const uint8_t clockPin = 11;  // Clock pin for SHT1x (Yellow wire)

SHT1x sht1x(dataPin, clockPin, SHT1x::Voltage::DC_5_0v);

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  Serial.println("SHT1x sensor test started!");
}

void loop() {
  float temperatureC = sht1x.readTemperatureC();
  float temperatureF = sht1x.readTemperatureF();
  float humidity = sht1x.readHumidity();

  Serial.print("Temperature (C): ");
  Serial.print(temperatureC);
  Serial.print(" °C, ");
  Serial.print("Temperature (F): ");
  Serial.print(temperatureF);
  Serial.print(" °F, ");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  
  delay(1000);
}
