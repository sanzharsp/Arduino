#include <ACS712.h>
char watt[5];
ACS712 sensor(ACS712_30A, A0);
void setup() {
  sensor.calibrate();
  // put your setup code here, to run once:

}

void loop() {


  float V= 230;
  float I = sensor.getCurrentAC();
  float P = V * I;
  
  last_time = current_time;
  current_time = millis();    
  Wh = Wh+  P *(( current_time -last_time) /3600000.0) ;
  dtostrf(Wh, 4, 2, watt);
  
Serial.write(watt);
  delay(10000);

}
