#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "MI8Lite"; // Enter your WiFi name
const char *password = "walangadi27";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "esp32/gooner";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

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
  digitalWrite(pinRedLed, HIGH);
  digitalWrite(pinYellowLed, LOW);
  digitalWrite(pinGreenLed, LOW);

  // Set software serial baud to 115200;
  Serial.begin(115200);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to " + String(( char *) ssid) + "...");
  }
  Serial.println("Connected to the WiFi network");
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

  //  Serial.println("Incomming message: " + message);

  // expect message
  // r=1;y=1;g=1 or r=0;y=1;g=1
  int r = getValue(message, ';', 0).substring(2).toInt();
  int y = getValue(message, ';', 1).substring(2).toInt();
  int g = getValue(message, ';', 2).substring(2).toInt();

  digitalWrite(pinRedLed, r);
  digitalWrite(pinYellowLed, y);
  digitalWrite(pinGreenLed, g);

  Serial.println();
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

    String redLedCurrent = String(digitalRead(pinRedLed));
    String yellowLedCurrent = String(digitalRead(pinYellowLed));
    String greenLedCurrent = String(digitalRead(pinGreenLed));

    //    Serial.println("=================");
    //    Serial.println(redLedCurrent);
    //    Serial.println(yellowLedCurrent);
    //    Serial.println(greenLedCurrent);
    //    Serial.println("=================");

    String statusLed = String("r=" + redLedCurrent + ";y=" + yellowLedCurrent + ";g=" + greenLedCurrent);
    Serial.println(statusLed);

    client.publish(topic, statusLed.c_str());
  }

  client.loop();
}
