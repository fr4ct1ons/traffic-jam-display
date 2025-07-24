#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define FREE_STREET_LED 32
#define SLOW_TRAFFIC_LED 33
#define TRAFFIC_JAM_LED 25
#define ROAD_CLOSED_LED 26

const char* ssid = "";
const char* password = "";
const char* hostName = "ESP32-TJD-PrudenteDeMorais";

const char* mqttServer = "homeassistant.local";
const char* mqttUser = "user";
const char* mqttPassword = "password";

const char* mqttTopic1 = "traffic/status/PrudenteDeMorais";
//const char* mqttTopic2 = "traffic/status/PrudenteDeMorais";
//const char* mqttTopic3 = "traffic/status/PrudenteDeMorais";
//const char* mqttTopic4 = "traffic/status/PrudenteDeMorais";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void ConnectWiFi();
void ConnectMQTT();
void Callback(char* topic, uint8_t*, unsigned int);

void setup() {
  Serial.begin(9600);

  pinMode(FREE_STREET_LED, OUTPUT);
  pinMode(SLOW_TRAFFIC_LED, OUTPUT);
  pinMode(TRAFFIC_JAM_LED, OUTPUT);
  pinMode(ROAD_CLOSED_LED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  ConnectWiFi();
  ConnectMQTT();
  mqttClient.subscribe(mqttTopic1);
  mqttClient.setCallback(Callback);

}

void loop() {

  if(!mqttClient.connected())
  {
    ConnectMQTT();
  }

  mqttClient.loop();

  mqttClient.subscribe(mqttTopic1);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);

}


void Callback(char* topic, uint8_t* message, unsigned int messageLength)
{
  char* found = strstr(topic, mqttTopic1);
  if(topic == mqttTopic1)

  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.println(". Message: ");
  String messageTemp;

  for (int i = 0; i < messageLength; i++) 
  {
    messageTemp += (char)message[i];
  }

  Serial.println(messageTemp);

  JsonDocument json;
  deserializeJson(json, messageTemp);

  const char* status = json["json"];
  found = strstr(status, "medium");

  if(found)
    digitalWrite(SLOW_TRAFFIC_LED, HIGH);
  else
    digitalWrite(SLOW_TRAFFIC_LED, LOW);
  

  found = strstr(status, "low");

  if(found)
    digitalWrite(FREE_STREET_LED, HIGH);
  else
    digitalWrite(FREE_STREET_LED, LOW);
  

  found = strstr(status, "high");

  if(found)
    digitalWrite(TRAFFIC_JAM_LED, HIGH);
  else
    digitalWrite(TRAFFIC_JAM_LED, LOW);
  



}


void ConnectWiFi()
{

  WiFi.begin(ssid, password);
  WiFi.setHostname(hostName);

  Serial.println("Connecting to wifi...");
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Error connecting - Attempting again...");
    delay(500);
  }

  Serial.print("WiFi connected. IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Signal strength: ");
  Serial.println(WiFi.RSSI());

}

void ConnectMQTT()
{

  Serial.println("Connecting to MQTT...");
  mqttClient.setServer(mqttServer, 1883);
  while(!mqttClient.connect(hostName, mqttUser, mqttPassword))
  {
    Serial.println("Error connecting - Attempting again...");
    delay(500);
  }

  Serial.println("MQTT Connected!");

}