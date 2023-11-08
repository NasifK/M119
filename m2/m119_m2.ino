#include <ArduinoBLE.h>
#include <Arduino_LSM6DS3.h>
#include <WiFi101.h>
#include <SPI.h>

long previousMillis = 0;
float ax, ay, az;
float gx, gy, gz;
byte mac[6];

BLEService IMUService("180A"); // BLE IMU Service

// BLE Data Characteristic - custom 128-bit UUID, read and writable by central
BLEFloatCharacteristic streamCharacteristic_ax("2A57", BLERead | BLEWrite);
BLEFloatCharacteristic streamCharacteristic_ay("2A58", BLERead | BLEWrite);
BLEFloatCharacteristic streamCharacteristic_az("2A59", BLERead | BLEWrite);
BLEFloatCharacteristic streamCharacteristic_gx("2A60", BLERead | BLEWrite);
BLEFloatCharacteristic streamCharacteristic_gy("2A61", BLERead | BLEWrite);
BLEFloatCharacteristic streamCharacteristic_gz("2A62", BLERead | BLEWrite);


void floatToBytes(float num, unsigned char* bytes) {
    memcpy(bytes, &num, sizeof(float));
}

void grab_imu_data() {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(ax, ay, az);
  }

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(gx, gy, gz);
  }

}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // IMU Setup
  if (!IMU.begin()) {
    Serial.println("IMU Initialization Failed.");
    while (1);
  }

  // set built in LED pin to output mode
  pinMode(LED_BUILTIN, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Nano 33 IoT");
  BLE.setAdvertisedService(IMUService);

  // add the characteristic to the service
  IMUService.addCharacteristic(streamCharacteristic_ax);

  // add service
  BLE.addService(IMUService);

  // set the initial value for the characteristic:
  streamCharacteristic_ax.writeValue(0);
  streamCharacteristic_ay.writeValue(0);
  streamCharacteristic_az.writeValue(0);
  streamCharacteristic_gx.writeValue(0);
  streamCharacteristic_gy.writeValue(0);
  streamCharacteristic_gz.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
  WiFi.macAddress(mac);
  for (int i = 0 ; i < 6; ++i) {
    Serial.print(mac[i], HEX);
    Serial.print(':');
  }
  Serial.print('\n');
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    delay(10000);
    int count = 0;
    // while the central is still connected to peripheral:
    while (central.connected()) {
      // grab IMU data
      grab_imu_data();
      delay(50);

      streamCharacteristic_ax.setValue(ax);
      streamCharacteristic_ay.setValue(ay);
      streamCharacteristic_az.setValue(az);
      streamCharacteristic_gx.setValue(gx);
      streamCharacteristic_gy.setValue(gy);
      streamCharacteristic_gz.setValue(gz);
    }
  }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, LOW);         // will turn the LED off
}