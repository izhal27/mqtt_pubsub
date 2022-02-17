#include <ArduinoJson.h>

#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "your-wife-name"; // Enter your WiFi name
const char *password = "your-wife-password";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "host";
const char *topic = "your-topic";
const char *mqtt_username = "username";
const char *mqtt_password = "password";
const int mqtt_port = "port";

const int pinRedLed = 27;
const int pinYellowLed = 26;
const int pinGreenLed = 25;
unsigned long previousMillis = 0;
unsigned long interval = 10000; // interval time

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(pinRedLed, OUTPUT);
  pinMode(pinYellowLed, OUTPUT);
  pinMode(pinGreenLed, OUTPUT);
  digitalWrite(pinRedLed, LOW);
  digitalWrite(pinYellowLed, LOW);
  digitalWrite(pinGreenLed, LOW);

  // Set software serial baud to 115200;
  Serial.begin(115200);

  setupWiFi();
  setupPubSub();
}

void setupWiFi() {
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to " + String(( char *) ssid) + "...");
  }
  Serial.println("Connected to the WiFi network");
}

void setupPubSub() {
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  // publish and subscribe
  client.publish(topic, "Hi EMQ X I'm ESP32 ^^");
  client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");

  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }

  Serial.println();
  payload[length] = 0;
  String message = String(( char *) payload);

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  int red = doc["red"];
  int yellow = doc["yellow"];
  int green = doc["green"];

  //  Serial.println(red);
  //  Serial.println(yellow);
  //  Serial.println(green);

  digitalWrite(pinRedLed, red);
  digitalWrite(pinYellowLed, yellow);
  digitalWrite(pinGreenLed, green);

  Serial.println("-----------------------");
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    Serial.println("publish message");

    int redLedCurrent = digitalRead(pinRedLed);
    int yellowLedCurrent = digitalRead(pinYellowLed);
    int greenLedCurrent = digitalRead(pinGreenLed);

    const int capacity = JSON_OBJECT_SIZE(3);
    StaticJsonDocument<capacity> doc;

    doc["red"] = redLedCurrent;
    doc["yellow"] = yellowLedCurrent;
    doc["green"] = greenLedCurrent;

    //    JsonObject obj = doc.to<JsonObject>();
    char output[128];
    serializeJson(doc, output);
    Serial.println(output);

    Serial.println();
    client.publish(topic, output);
    Serial.println("========================");
  }

  client.loop();
}
