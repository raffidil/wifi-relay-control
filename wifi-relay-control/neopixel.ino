
void fadeLoop() {
  double i=0;
    while(i<1){
    setFractionStripColor(0,11,Rfade,Gfade,Bfade,i);
    i=i+0.025;
  delay(fadewait);
  } 
  double j=1;
  while(j>=0){
    setFractionStripColor(0,11,Rfade,Gfade,Bfade,j);
    j=j-0.025;
  delay(fadewait);
  }
  setFractionStripColor(0,11,Rfade,Gfade,Bfade,0);
  fadeTask.delay(fadeDelay);
}



void colorWipeLoop() {
  if (colorWipeTask.isFirstIteration()) {
    Serial.println("colorWipeLoop started");
    j = 0;
  }

  strip.setPixelColor(j, colorWipeColor);
  strip.show();
  j++;

  if (j >= strip.numPixels()) {
    colorWipeTask.disable();
  }
}
