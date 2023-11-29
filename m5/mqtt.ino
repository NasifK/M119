#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino_LSM6DS3.h>
// MQTT code inspired by Warren's MQTT code he presented in discussion. Thanks Warren!
// https://github.com/warrenwallis/ECEM119/tree/main/M2

// WiFi
const char *ssid = "KatN"; // Enter your WiFi name
const char *password = "1234abcd";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "mqtt.eclipseprojects.io";
const char *topic = "ECEM119";
// const char *mqtt_username = "emqx";
// const char *mqtt_password = "public";
const int mqtt_port = 1883;

int counter = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
 // Set software serial baud to 115200;
	Serial.begin(115200);
	while (!Serial) delay(10); // delay until serial is connected
	// connecting to a WiFi network

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }
  Serial.println("IMU Initialized");
	
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
			delay(500);
			Serial.println("Connecting to WiFi..");
	}
	Serial.println("Connected to the WiFi network");
	//connecting to a mqtt broker
	client.setServer(mqtt_broker, mqtt_port);
	client.setCallback(callback);
	while (!client.connected()) {
			String client_id = "esp32-client-";
			if (client.connect(client_id.c_str())) { //, mqtt_username, mqtt_password)) {
					Serial.println("mqtt broker connected");
			} else {
					Serial.print("failed with state ");
					Serial.print(client.state());
					delay(2000);
			}
	}
	// publish and subscribe
	client.publish(topic, "Hi I'm ESP32");
	//client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
	Serial.print("Message arrived in topic: ");
	Serial.println(topic);
	Serial.print("Message:");
	for (int i = 0; i < length; i++) {
		Serial.print((char) payload[i]);
	}
	Serial.println();
	Serial.println("-----------------------");
}

void loop()
{
  displayIMU();
}

void displayIMU() {
  float x, y, z;

  String output = "";
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);

    output += "Acceleration in g's";
    output += " ax = ";
    output += x;
    output += " ay = ";
    output += y;
    output += " az = ";
    output += z;

    client.publish(topic, output.c_str());
  }

  output = "";
  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(x, y, z);

    output += "Gyroscope in degrees/second";
    output += " gx = ";
    output += x;
    output += " gy = ";
    output += y;
    output += " gz = ";
    output += z;

    client.publish(topic, output.c_str());
  }

  delay(250);
}