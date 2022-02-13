#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "Rumah Scarlett"; // Enter your WiFi name
const char *password = "Scarlett27";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "esp32/gooner";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

const int pinLed = 27;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(pinLed, OUTPUT);
  digitalWrite(pinLed, LOW);

  // Set software serial baud to 115200;
  Serial.begin(115200);
  // connecting to a WiFi network
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

  //  Serial.println();
  Serial.println();
  payload[length] = 0;
  String message = String(( char *) payload);

  //  Serial.println("Incomming message: " + message);

  if (message == "ON") {
    digitalWrite(pinLed, HIGH);
  }

  if (message == "OFF") {
    digitalWrite(pinLed, LOW);
  }

  if (message == "status") {
    Serial.println("publish message");
    int mode = digitalRead(pinLed);

    if (mode == 1) {
      client.publish(topic, "ON");
    } else {
      client.publish(topic, "OFF");
    }
  }

  Serial.println();
  Serial.println("-----------------------");
}

void loop() {
  client.loop();
}
