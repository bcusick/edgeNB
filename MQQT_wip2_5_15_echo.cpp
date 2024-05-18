#include <ArduinoMqttClient.h>
#include <MKRNB.h>
#include "shared.hpp"

// Define your network credentials **put in secret.h
const char PINNUMBER[]     = ""; 
const char GPRS_APN[]      = "hologram";

// Initialize the NB client and MqttClient
GPRS gprs;
NB nbAccess;
NBClient client;
MqttClient mqttClient(client);


// Define your MQTT 
const char broker[] = "test.mosquitto.org";
int port = 1883;

const char topic[]  = "BCUSICK/temperature";

const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;

int currentTemp = 25;
int initConnect();

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  delay(1500);

  pinMode(LED_BUILTIN, OUTPUT);

  // Start INITIAL NB AND MQTT connection
  while(!initConnect());

  // subscribe to a topic
  mqttClient.subscribe(topic);
}
 
void loop() {

  

  // Ensure the NB connection is still active
  if (!nbAccess.isAccessAlive()) {
    while (!initConnect());
  }
  else {
    mqttClient.poll(); //MQTT keep alive
    int messageSize = mqttClient.parseMessage();
    if (messageSize) {
      // we received a message, print out the topic and contents
      Serial.print("Received a message with topic '");
      Serial.print(mqttClient.messageTopic());
      Serial.print("', length ");
      Serial.print(messageSize);
      Serial.println(" bytes:");

      // use the Stream interface to print the contents
      while (mqttClient.available()) {
        Serial.print((char)mqttClient.read());
      }
      Serial.println();

      Serial.println();
    }

    // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
    // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
      // save the last time a message was sent
      previousMillis = currentMillis;

      Serial.print("Sending message to topic: ");
      Serial.println(topic);
      Serial.print("echo ");
      Serial.println(count);

      // send message, the Print interface can be used to set the message contents
      mqttClient.beginMessage(topic);
      mqttClient.print("echo ");
      mqttClient.print(count);
      mqttClient.endMessage();

      Serial.println();

      count++;
    }
  }
}
    


int initConnect() {
  Serial.println("Starting LTE connection...");

  NB_NetworkStatus_t status = nbAccess.begin(PINNUMBER, GPRS_APN);
  if (status == NB_READY) {
    Serial.println("LTE connected!");
    digitalWrite(LED_BUILTIN, HIGH);
    mqttClient.connect(broker, port);
    Serial.println("MQTT connected!");
    return 1;
  }
  else {
    Serial.println("LTE retry!");
    digitalWrite(LED_BUILTIN, LOW);
    return 0;
  }
}




