int button = D3;
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin
long buttonTime = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers



void buttonLoop() {
  reading = digitalRead(button);

  if (reading == HIGH && previous == LOW && millis() - buttonTime > debounce) {
    Serial.println("BUTTON PRESSED");
    
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

  previous = reading;


}

bool buttonSetup(){
  pinMode(button, INPUT);
  delay(100);
  return true;
}

