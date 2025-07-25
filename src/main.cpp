#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define FREE_STREET_LED 32
#define SLOW_TRAFFIC_LED 33
#define TRAFFIC_JAM_LED 25
#define ROAD_CLOSED_LED 26

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* hostName = "ESP32-TJD-PrudenteDeMorais";

const char* mqttTopic1 = "traffic/raw/prudente_de_moraes";
//const char* mqttTopic2 = "traffic/status/PrudenteDeMorais";
//const char* mqttTopic3 = "traffic/status/PrudenteDeMorais";
//const char* mqttTopic4 = "traffic/status/PrudenteDeMorais";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void ConnectWiFi();
void ConnectMQTT();
void Callback(char* topic, uint8_t*, unsigned int);
void ResetDisplay();

String displayLine = "Prudente de Morais 0";

void setup() {
  Serial.begin(9600);

  pinMode(FREE_STREET_LED, OUTPUT);
  pinMode(SLOW_TRAFFIC_LED, OUTPUT);
  pinMode(TRAFFIC_JAM_LED, OUTPUT);
  pinMode(ROAD_CLOSED_LED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  ConnectWiFi();
  ConnectMQTT();
  mqttClient.subscribe(mqttTopic1, 0);
  mqttClient.setCallback(Callback);

  digitalWrite(FREE_STREET_LED, HIGH);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.setTextSize(1);
  display.setTextColor(WHITE);

  ResetDisplay();

  display.setCursor(0, 16);
  display.println("Prudente de Morais 1");
  display.display();

}

void loop() {

  if(WiFi.status() != WL_CONNECTED)
  {
    ConnectWiFi();
  }

  if(!mqttClient.connected())
  {
    ConnectMQTT();
  }

  mqttClient.loop();

  /*
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
  */
}


void Callback(char* topic, uint8_t* message, unsigned int messageLength)
{
  char* found = strstr(topic, mqttTopic1);
  if(!topic)
  {
    Serial.println("Topic did not match. Skipping...");
  }

  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.println(" - Message: ");
  String messageTemp;

  for (int i = 0; i < messageLength; i++) 
  {
    messageTemp += (char)message[i];
  }

  Serial.println(messageTemp);

  JsonDocument json;
  deserializeJson(json, messageTemp);

  const char* jsonProperty = "congestion_level";

  if(!json[jsonProperty].is<String>())
  {
    Serial.println("Message did not have a key named status. Aborting.");
    return;
  }
  else
  {
    Serial.print("Found a status! Status is:");
    Serial.println((const char*)json[jsonProperty]);
  }

  const char* status = json[jsonProperty];

  ResetDisplay();
  display.setCursor(0, 16);
  
  //display.println("Prudente de Morais 0");
  found = strstr(status, "medium");

  if(found)
  {
    digitalWrite(SLOW_TRAFFIC_LED, HIGH);
  
    display.println("Prudente de Morais 2");
  }
  else
    digitalWrite(SLOW_TRAFFIC_LED, LOW);
  

  found = strstr(status, "low");

  if(found)
  {
    digitalWrite(FREE_STREET_LED, HIGH);
  
    display.println("Prudente de Morais 1");
  }
  else
    digitalWrite(FREE_STREET_LED, LOW);
  

  found = strstr(status, "high");

  if(found)
  {
    digitalWrite(TRAFFIC_JAM_LED, HIGH);
    
    display.println("Prudente de Morais 3");
  }
  else
    digitalWrite(TRAFFIC_JAM_LED, LOW);
  
  
  found = strstr(status, "closed");

  if(found)
  {
    digitalWrite(ROAD_CLOSED_LED, HIGH);
  
    display.println("Prudente de Morais 4");
  }
  else
    digitalWrite(ROAD_CLOSED_LED, LOW);
  
  display.display();

}

void ResetDisplay()
{
  display.clearDisplay();
  display.display();

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("LENTO 5 SN");
  display.display();

  display.setTextSize(1); //Each character is 7 pixels tall
  
  display.setCursor(0, 16);
  display.println("                     ");

  display.setCursor(0, 24);
  display.println("Salgado Filho 0");

  display.setCursor(0, 32);
  display.println("Amintas Barros 0");

  display.setCursor(0, 40);
  display.println("Nasc. De Castro 0");
  display.display();
}


void ConnectWiFi()
{

  WiFi.begin(ssid, password);
  WiFi.setHostname(hostName);

  Serial.println("Connecting to wifi...");
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Error connecting to WiFi - Attempting again...");
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
  while(!mqttClient.connect(hostName))
  {
    Serial.println("Error connecting to MQTT - Attempting again...");
    delay(500);
  }

  Serial.println("MQTT Connected!");

}