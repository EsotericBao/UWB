#include <WiFi.h>
#include <PubSubClient.h>

// WiFi Credentials
const char *ssid = "legalslavery";            // Replace with your WiFi name
const char *password = "atleast8char";  // Replace with your WiFi password

// MQTT Broker Settings
const char *mqtt_broker = "broker.emqx.io";
const char *mqtt_topic = "emqx/esp32";
const char *mqtt_username = "desu";
const char *mqtt_password = "desu";
const int mqtt_port = 1883;

const int ledpin = 32;
const int interval = 1000;
const int commPin = 27;

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

// Function Declarations
void connectToWiFi();

void connectToMQTT();

 
void setup() {
    Serial.begin(115200);
    pinMode(ledpin, OUTPUT);
    pinMode(commPin, INPUT_PULLUP);

    connectToWiFi();
    mqtt_client.setServer(mqtt_broker, mqtt_port);
    mqtt_client.setKeepAlive(60);
    mqtt_client.setCallback(mqttCallback); // Corrected callback function name
    connectToMQTT();

}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
}

void connectToMQTT() {
    while (!mqtt_client.connected()) {
        String client_id = "esp32-client-" + String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
        if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT broker");
            mqtt_client.subscribe(mqtt_topic);
            mqtt_client.publish(mqtt_topic, "Hi EMQX I'm ESP32 ^^"); // Publish message upon successful connection
        } else {
            Serial.print("Failed, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void mqttCallback(char *mqtt_topic, byte *payload, unsigned int length) {
    Serial.print("Message received on mqtt_topic: ");
    Serial.println(mqtt_topic);
    Serial.print("Message: ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
        int x = payload[i];
    if (x == '2'){
      digitalWrite(ledpin, LOW);
      delay(100);
      digitalWrite(ledpin, HIGH);
      delay(interval);
      digitalWrite(ledpin, LOW);

    }
    Serial.println("\n-----------------------");
    }
}


void loop() {
    if (!mqtt_client.connected()) {
        connectToMQTT();
    }
     mqtt_client.loop();

     int state = digitalRead(commPin);
     if( state == LOW){
      mqtt_client.publish(mqtt_topic, "1");
      delay(500);
      state = HIGH;
     }
}
