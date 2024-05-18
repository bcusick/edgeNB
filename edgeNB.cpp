#include <ArduinoMqttClient.h>
#include <MKRNB.h>
#include <Wire.h>
#include <RTCZero.h>
#include "shared.hpp"

// Define your network credentials **put in secret.h
const char PINNUMBER[]     = ""; 
const char GPRS_APN[]      = "hologram";

#define SELF_I2C_ADDR 0x05

SensorValues_t sensorData; //stucture to hold All data passed between Edge and NB

// Water tank constants
#define PI 3.1415926535897932384626433832795
#define radius 0.28  // in meters

// I2C communication flow control variables
int ctrlRec = 0;
int ctrlReq = 0;
int valid = 0;

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

RTCZero rtc;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  delay(1500);

  pinMode(LED_BUILTIN, OUTPUT);

  Wire.begin(SELF_I2C_ADDR);                // Start I2C communication 
  Wire.onRequest(requestEvent); // Register event

  rtc.begin();
  Serial.println(rtc.getEpoch());
  // Start INITIAL NB AND MQTT connection
  while(!initConnect());

  // subscribe to a topic
  //mqttClient.subscribe(topic);
}
 
void loop() {

  
  Serial.println(rtc.getEpoch());
  // Ensure the NB connection is still active
  if (!nbAccess.isAccessAlive()) {
    while (!initConnect());
  }
  else {
    //mqttClient.poll(); //MQTT keep alive
    //data.modemTime = nbAccess.getTime();
  }
    
}
    
void setTime() {
  

  rtc.setEpoch(nbAccess.getTime());
}

int initConnect() {
  Serial.println("Starting LTE connection...");

  NB_NetworkStatus_t status = nbAccess.begin(PINNUMBER, GPRS_APN);
  if (status == NB_READY) {
    Serial.println("LTE connected!");
    digitalWrite(LED_BUILTIN, HIGH);
    setTime(); //update data stuture to share with Edge controller
    //mqttClient.connect(broker, port);
    //Serial.println("MQTT connected!");
    return 1;
  }
  else {
    Serial.println("LTE retry!");
    digitalWrite(LED_BUILTIN, LOW);
    return 0;
  }
}





/*
  Function that handles when the Edge Control sends data to the MKR.
  @param bytes The I2C communicated sensors raw values
*/
/*
void receiveEvent(int bytes) {
  uint8_t buf[200];
  uint8_t *ptr = &buf[0];

  SensorValues_t *vals;

  Serial.println("Receive event");
  ctrlRec = 1;
  while (Wire.available() > 0) {
    *ptr = Wire.read();
    ptr++;
  }

  vals = (SensorValues_t *)buf;

  if (ctrlRec - ctrlReq) {
    uploadValues(vals);
  }

  ctrlRec = 0;
  ctrlReq = 0;
}
*/


// Callback function for request event
void requestEvent() {
  //SensorValues_t sensorData;
  sensorData.epoch = rtc.getEpoch();
  // Send sensor data to master
  Wire.write((byte *)&sensorData, sizeof(sensorData));
}