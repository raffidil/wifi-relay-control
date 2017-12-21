#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>
#include <TaskScheduler.h>
#include <Adafruit_NeoPixel.h>
#include <stdlib.h>

void callback(char* topic, byte* payload, unsigned int length);

#define MQTT_SERVER "192.168.1.12"  //you MQTT IP Address
#define PIN D1
const char* ssid = "ssid";
const char* password = "password";


int state = HIGH;      // the current state of the output pin
int lightState = HIGH;
int stateDown = HIGH;
const int switchPin1 = D6;

uint8_t colorWipeSpeed = 50;
uint8_t fadeSpeed = 0;
uint32_t colorWipeColor;
uint16_t j;

//fade
long Rfade = 0;
long Gfade = 0;
long Bfade = 0;
long fadewait = 60;
long fadeDelay = 3000;


char const* room_light = "/home/room/light/set/";

Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, PIN, NEO_GRB + NEO_KHZ800);
WiFiServer server(80);
WiFiManager wifiManager;


Scheduler runner;
WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);

void mqttLoop();
void buttonLoop();
void httpLoop();
void colorWipeLoop();
void FadeLoop();


bool buttonSetup();
bool httpSetup();

Task buttonTask(50, TASK_FOREVER, &buttonLoop, NULL, false, &buttonSetup);
Task httpTask(10, TASK_FOREVER, &httpLoop, NULL, false, &httpSetup);
Task mqttTask(100, TASK_FOREVER, &mqttLoop, NULL, false);
Task fadeTask(fadeSpeed, TASK_FOREVER, &fadeLoop, NULL, false);
Task colorWipeTask(colorWipeSpeed, TASK_FOREVER, &colorWipeLoop);

void disableAll() {
  colorWipeTask.disable();
  fadeTask.disable();
}

void setup() {
  Serial.begin(115200);
  delay(200);


  //initialize the switch as an output and set to LOW (off)
  pinMode(switchPin1, OUTPUT); // Relay Switch 1
  digitalWrite(switchPin1, LOW);



  strip.begin();
  strip.setBrightness(255);
  strip.show();

  // Inint task schedueler
  runner.init();
  runner.addTask(colorWipeTask);
  runner.addTask(fadeTask);
  runner.addTask(buttonTask);
  runner.addTask(mqttTask);
  runner.addTask(httpTask);
  delay(200);

  //start the serial line for debugging

  //NeoPixels off
  Serial.println("NeoPixel OFF");
  setStripColor(0, 11, 0, 0, 0);
  state = LOW;

  //start wifi subsystem
  WiFi.disconnect();
  WiFi.softAPdisconnect();
  WiFi.mode(WIFI_OFF);
  delay(500);
  WiFi.mode(WIFI_STA);
  WiFi.config(IPAddress(192, 168, 1, 16), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0), IPAddress(192, 168, 1, 1));
  WiFi.begin(ssid, password);

  //attempt to connect to the WIFI network and then connect to the MQTT server
  reconnect();
  Serial.println("setup reconnect done");


  buttonTask.enable();
  mqttTask.enable();
  httpTask.enable();
}


void loop() {
  runner.execute();
}

























