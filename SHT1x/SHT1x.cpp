/**
 * SHT1x Library
 *
 * Written by Lucienne Swart
 * This is an adaptation from the Beegee-Tokyo / SHT1x-ESP library.
 *
 * Based on the work by:
 *    Bernd Giesecke <beegee@giesecke.tk>
 *    Vincent Pang <wingshun.pang@gmail.com>
 *    Jonathan Oxer <jon@oxer.com.au> / <www.practicalarduino.com>
 *    Maurice Ribble: <www.glacialwanderer.com/hobbyrobotics/?p=5>
 *    Wayne ?: <ragingreality.blogspot.com/2008/01/ardunio-and-sht15.html>
 *
 * Manages communication with SHT1x series (SHT10, SHT11, SHT15)
 * temperature / humidity sensors from Sensirion (www.sensirion.com).
 */
#include "SHT1x.h"
#include <math.h>

SHT1x::SHT1x(uint8_t dataPin, uint8_t clockPin, Voltage voltage)
    : _dataPin(dataPin), _clockPin(clockPin), _voltage(voltage),
      _tempResolution(TemperatureMeasurementResolution::Temperature_14bit),
      _humidityResolution(HumidityMeasurementResolution::Humidity_12bit) {}

/* ================  Public methods ================ */

float SHT1x::readTemperatureC() const {
    const double D1 = getD1ForC(_voltage);
    const double D2 = getD2ForC(_tempResolution);
    uint16_t rawData = readRawData(ShtCommand::MeasureTemperature, _dataPin, _clockPin);
    if (rawData == kRawDataError) return NAN;
    return D1 + D2 * rawData;
}

float SHT1x::readTemperatureF() const {
    const double D1 = getD1ForF(_voltage);
    const double D2 = getD2ForF(_tempResolution);
    uint16_t rawData = readRawData(ShtCommand::MeasureTemperature, _dataPin, _clockPin);
    if (rawData == kRawDataError) return NAN;
    return D1 + D2 * rawData;
}

float SHT1x::readHumidity() const {
    const double C1 = getC1(_humidityResolution);
    const double C2 = getC2(_humidityResolution);
    const double C3 = getC3(_humidityResolution);
    const double T1 = getT1(_humidityResolution);
    const double T2 = getT2(_humidityResolution);
    uint16_t rawData = readRawData(ShtCommand::MeasureRelativeHumidity, _dataPin, _clockPin);
    if (rawData == kRawDataError) return NAN;
    double linearHumidity = C1 + C2 * rawData + C3 * rawData * rawData;
    float temperature = readTemperatureC();
    if (isnan(temperature)) return NAN;
    return (temperature - 25.0) * (T1 + T2 * rawData) + linearHumidity;
}

/* ================  Private methods ================ */

uint16_t SHT1x::readRawData(ShtCommand command, uint8_t dataPin, uint8_t clockPin) const {
    if (!sendCommandSHT(command, dataPin, clockPin)) {
        return kRawDataError;
    }
    if (!waitForResultSHT(dataPin)) {
        return kRawDataError;
    }
    uint16_t result = getData16SHT(dataPin, clockPin);
    skipCrcSHT(dataPin, clockPin);
    return result;
}

bool SHT1x::sendCommandSHT(ShtCommand command, uint8_t dataPin, uint8_t clockPin) const {
    pinMode(clockPin, OUTPUT);
    controlDataPin(dataPin, HIGH);
    delayMicroseconds(1);
    digitalWrite(clockPin, HIGH);
    controlDataPin(dataPin, LOW);
    delayMicroseconds(1);
    digitalWrite(clockPin, LOW);
    delayMicroseconds(1);
    digitalWrite(clockPin, HIGH);
    controlDataPin(dataPin, HIGH);
    delayMicroseconds(1);
    digitalWrite(clockPin, LOW);
    delayMicroseconds(1);
    
    for (uint8_t i = 0; i < 8; i++) {
        controlDataPin(dataPin, !!(static_cast<uint8_t>(command) & (1 << (7 - i))));
        digitalWrite(clockPin, HIGH);
        delayMicroseconds(1);
        digitalWrite(clockPin, LOW);
        delayMicroseconds(1);
    }
    
    pinMode(dataPin, INPUT);
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(1);
    int ack = digitalRead(dataPin);
    if (ack != LOW) return false;
    digitalWrite(clockPin, LOW);
    delayMicroseconds(10);
    ack = digitalRead(dataPin);
    if (ack != HIGH) return false;
    
    return true;
}

bool SHT1x::waitForResultSHT(uint8_t dataPin) const {
    pinMode(dataPin, INPUT);
    int ack = HIGH;
    unsigned long startWait = millis();
    while (ack == HIGH) {
        delay(10);
        ack = digitalRead(dataPin);
        if ((millis() - startWait) > 1000) break;
    }
    return ack != HIGH;
}

uint16_t SHT1x::getData16SHT(uint8_t dataPin, uint8_t clockPin) const {
    uint8_t rawData[2] = {0};
    for (uint8_t i = 0; i < 8; ++i) {
        digitalWrite(clockPin, HIGH);
        delayMicroseconds(1);
        rawData[0] |= digitalRead(dataPin) << (7 - i);
        digitalWrite(clockPin, LOW);
        delayMicroseconds(1);
    }
    controlDataPin(dataPin, LOW);
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(1);
    digitalWrite(clockPin, LOW);
    pinMode(dataPin, INPUT);
    for (uint8_t i = 0; i < 8; ++i) {
        digitalWrite(clockPin, HIGH);
        delayMicroseconds(1);
        rawData[1] |= digitalRead(dataPin) << (7 - i);
        digitalWrite(clockPin, LOW);
        delayMicroseconds(1);
    }
    return (rawData[0] << 8) | rawData[1];
}

void SHT1x::skipCrcSHT(uint8_t dataPin, uint8_t clockPin) const {
    controlDataPin(dataPin, HIGH);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
}

double SHT1x::getC1(HumidityMeasurementResolution resolution) const {
    const double VALUES[] = {-2.0468, -2.0468};
    return VALUES[static_cast<uint8_t>(resolution)];
}

double SHT1x::getC2(HumidityMeasurementResolution resolution) const {
    const double VALUES[] = {0.0367, 0.5872};
    return VALUES[static_cast<uint8_t>(resolution)];
}

double SHT1x::getC3(HumidityMeasurementResolution resolution) const {
    const double VALUES[] = {-1.5955e-6, -4.0845e-4};
    return VALUES[static_cast<uint8_t>(resolution)];
}

double SHT1x::getT1(HumidityMeasurementResolution resolution) const {
    const double VALUES[] = {0.01, 0.01};
    return VALUES[static_cast<uint8_t>(resolution)];
}

double SHT1x::getT2(HumidityMeasurementResolution resolution) const {
    const double VALUES[] = {0.00008, 0.00128};
    return VALUES[static_cast<uint8_t>(resolution)];
}

double SHT1x::getD1ForC(Voltage voltage) const {
    const double VALUES[] = {-40.1, -39.8, -39.7, -39.66, -39.6, -39.4};
    return VALUES[static_cast<uint8_t>(voltage)];
}

double SHT1x::getD1ForF(Voltage voltage) const {
    const double VALUES[] = {-40.2, -39.6, -39.5, -39.42, -39.3, -38.9};
    return VALUES[static_cast<uint8_t>(voltage)];
}

double SHT1x::getD2ForC(TemperatureMeasurementResolution resolution) const {
    const double VALUES[] = {0.01, 0.04};
    return VALUES[static_cast<uint8_t>(resolution)];
}

double SHT1x::getD2ForF(TemperatureMeasurementResolution resolution) const {
    const double VALUES[] = {0.018, 0.072};
    return VALUES[static_cast<uint8_t>(resolution)];
}

void SHT1x::controlDataPin(uint8_t dataPin, uint8_t val) const {
    if (val) {
        pinMode(dataPin, INPUT_PULLUP);
    } else {
        digitalWrite(dataPin, LOW);
        pinMode(dataPin, OUTPUT);
    }
}
