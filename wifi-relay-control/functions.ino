int firstWifiloop = 0;
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {

  //convert topic to string to make it easier to work with
  String topicStr = p_topic;
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }

  //Print out some debugging info
  Serial.println("Callback update.");
  Serial.print("Topic: ");
  Serial.println(topicStr);

  if (topicStr == "/home/room/light/set/") {

    if (payload.equals(String("ON"))) {
      digitalWrite(switchPin1, LOW);
      lightState = LOW;
      client.publish("/home/room/light/state/", "ON");
    }

    else if (payload.equals(String("OFF"))) {
      digitalWrite(switchPin1, HIGH);
      lightState = HIGH;
      client.publish("/home/room/light/state/", "OFF");
    }

  }

}

int mqttTry = 0;
void reconnect() {

  //attempt to connect to the wifi if connection is lost
  if (WiFi.status() != WL_CONNECTED) {
    //debug printing
    Serial.print("Connecting to ");
    Serial.println(ssid);

    while (WiFi.status() != WL_CONNECTED && firstWifiloop < 20 ) {
      delay(500);
      Serial.print(".");
      firstWifiloop++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      //print out some more debug once connected
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.print("* IP address:\t");
      Serial.println(WiFi.localIP());
      setStripColor(3, 4, 0, 92, 3);
      delay(500);
      setStripColor(3, 4, 0, 0, 0);
      state = LOW;
    }
  }

  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if (WiFi.status() == WL_CONNECTED) {
    // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");

      String clientName;
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);

      if (client.connect((char*) clientName.c_str())) {
        Serial.print("\nMQTT Connected to:\t");
        Serial.println(MQTT_SERVER);
        client.subscribe(room_light);

      }

      else {
        Serial.println("\tFailed.");
        mqttTry++;
        break;
      }
    }
  }

}

//generate unique name from MAC addr
String macToStr(const uint8_t* mac) {

  String result;

  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);

    if (i < 5) {
      result += ':';
    }
  }

  return result;
}

void setStripColor(int from, int to, uint8_t r, uint8_t g, uint8_t b) {
  for (int i = from; i <= to; i++ ) {
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
}

void setFractionStripColor(int from, int to, uint8_t r, uint8_t g, uint8_t b, double fraction) {
  r = ((r - 0) * fraction);
  g = ((g - 0) * fraction);
  b = ((b - 0) * fraction);
  for (int i = from; i <= to; i++ ) {
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
}

