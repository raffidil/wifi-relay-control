

void mqttLoop() {
  //reconnect if connection is lost
  if (WiFi.status() != WL_CONNECTED || !client.connected() && mqttTry < 3) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi is not connected");
    }
    Serial.println("reconnect...");
    reconnect();
    if (mqttTry == 3) {
      Serial.println("NeoPixel OFF");

    }
    mqttTask.delay(2000);
  }

  //maintain MQTT connection
  client.loop();

  //MUST delay to allow ESP8266 WIFI functions to run
  delay(10);


}

bool httpSetup() {
  Serial.println("Server started");
  server.begin();
  return true;
}

void httpLoop() {
  WiFiClient httpclient = server.available();
  if (!httpclient) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("main new client");

  // Read the first line of the request
  String request = httpclient.readStringUntil('\r');
  Serial.println(request);
  httpclient.flush();

  // Return the response
  httpclient.println("HTTP/1.1 200 OK");
  httpclient.println("Content-Type: text/html");
  httpclient.println(""); //  do not forget this one

  if (request.indexOf("/light") != -1) {
    Serial.println("http Light GET");

    if (lightState == HIGH) {
      digitalWrite(switchPin1, LOW);
      lightState = LOW;
      client.publish("/home/room/light/state/", "ON");
    }
    else if (lightState == LOW) {
      digitalWrite(switchPin1, HIGH);
      lightState = HIGH;
      client.publish("/home/room/light/state/", "OFF");
    }

  }

  if (request.indexOf("/on") != -1) {
    Serial.println("http ON GET");

    digitalWrite(switchPin1, LOW);
    lightState = LOW;
    client.publish("/home/room/light/state/", "ON");

  }

  if (request.indexOf("/off") != -1) {
    Serial.println("http OFF GET");

    digitalWrite(switchPin1, HIGH);
    lightState = HIGH;
    client.publish("/home/room/light/state/", "OFF");

  }

  if (request.indexOf("/fade=") != -1) {
    Serial.print("fade ");
    httpclient.print("fade ");
    char hex[7];
    memcpy( hex, &request[10], 6 );
    hex[7] = '\0';
    Serial.println(hex);
    httpclient.println(hex);

    long long number = strtol( hex, NULL, 16);
    Rfade = number >> 16;
    Gfade = number >> 8 & 0xFF;
    Bfade = number & 0xFF;

    char sec1[5];
    memcpy( sec1, &request[16], 4 );
    sec1[5] = '\0';
    Serial.println(sec1);
    httpclient.println(sec1);
    long fadewaitsec = strtol( sec1, NULL, 10);
    fadewait = fadewaitsec;


    char sec2[5];
    memcpy( sec2, &request[20], 4 );
    sec2[5] = '\0';
    Serial.println(sec2);
    httpclient.println(sec2);
    long fadedelaysec = strtol( sec2, NULL, 10);
    fadeDelay = fadedelaysec;
    disableAll();
    fadeTask.enableIfNot();
    state = HIGH;
  }

  if (request.indexOf("/hex=") != -1) {
    Serial.print("hex ");
    httpclient.print("hex ");
    char hex[7];
    memcpy( hex, &request[9], 6 );
    hex[7] = '\0';
    Serial.println(hex);
    httpclient.println(hex);

    long long number = strtol( hex, NULL, 16);
    long long r = number >> 16;
    long long g = number >> 8 & 0xFF;
    long long b = number & 0xFF;

    colorWipeColor = strip.Color(r, g, b);
    disableAll();
    colorWipeTask.enableIfNot();
    state = HIGH;
  }


  if (request.indexOf("/stop") != -1) {
    Serial.println("disable all");
    httpclient.println("disable all");
    colorWipeColor = strip.Color(0, 0, 0);
    disableAll();
    colorWipeTask.enableIfNot();
    state = LOW;
  }
}

