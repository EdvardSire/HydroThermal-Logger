// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <EEPROM.h>

#define DHTPIN 2
#define DHTTYPE DHT11 // or DHT22
#define EEPROM_START_ADDRESS 6
uint32_t delayMS;

DHT_Unified dht(DHTPIN, DHTTYPE);
void clean_EEPROM(void);
void fill_EEPROM(void);
void show_EEPROM(void);
void set_index(int value);
int get_index();
void setup();
void loop();
void print_debug_info(sensor_t sensor);

void clean_EEPROM(void) {
  for (int i = 0; i < EEPROM.length(); i++)
    EEPROM.write(i, 0);
}
void fill_EEPROM(void) {
  for (int i = 0; i < EEPROM.length(); i++)
    EEPROM.write(i, i);
}

void show_EEPROM(void) {
  for (int i = 0; i < EEPROM.length(); i++)
    Serial.println(EEPROM.read(i));
}

// Function to write a 32-bit integer to EEPROM at a specified address
void set_index(int value) {
  // Write the 32-bit integer byte by byte to EEPROM addresses 0 to 3
  for (int i = 0; i < 4; i++) {
    EEPROM.write(i, (value >> (8 * i)) & 0xFF);
  }
  Serial.print(F("Setting index: "));
  Serial.println(value);
  Serial.println();
}

// Function to read a 32-bit integer from EEPROM at a specified address
int get_index() {
  char data[4];
  int32_t result = 0;

  // Read the 32-bit integer byte by byte from the first 4 bytes
  for (int i = 0; i < 4; i++)
    data[i] = EEPROM.read(i);

  // Interpret the char array as a 32-bit integer
  for (int i = 0; i < 4; i++)
    result |= (data[i] & 0xFF) << (8 * i);

  return result;
}

void setup() {
  Serial.begin(9600);

  // DO NOT TOUCH
  // set_index(EEPROM_START_ADDRESS);

  show_EEPROM();

  dht.begin();
  sensor_t sensor;

  delayMS = 450000; // 7.5 min
  // delayMS = 10000;
}

void loop() {
  delay(delayMS);

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  } else {
    Serial.print(F("Temperature: "));
    // Serial.print(event.temperature);
    // Serial.println(F("°C"));

    int ADDR = get_index();
    if (ADDR <= EEPROM.length()) {
      EEPROM.write(ADDR, (uint8_t)event.temperature);
      set_index(ADDR + 1);
    }
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  } else {
    Serial.print(F("Humidity: "));
    // Serial.print(event.relative_humidity);
    // Serial.println(F("%"));

    int ADDR = get_index();
    if (ADDR <= EEPROM.length()) {
      EEPROM.write(ADDR, (uint8_t)event.relative_humidity);
      set_index(ADDR + 1);
    }
  }
}

void print_debug_info(sensor_t sensor) {
  dht.temperature().getSensor(&sensor);
  Serial.println(F("DHTxx Unified Sensor Example"));
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("°C"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("°C"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("%"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("%"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
}
