#include "WaterControll.h"
WaterControll sens;

void setup() {
  Serial.begin(9600);
  sens.Add(sens.PH, "pH", 33);
  sens.Add(sens.OPR, "OPR");

  sens.set_finishHandler(finish);
}

void finish(float* in){
  Serial.println("Data from sensors recieved!");
  for (int i = 0; i < sens.sensors_count; i++){
    Serial.println(sens.sensors_name[i] + ": " + String(in[i]));
  }
}

void loop() {
  sens.readAll_nw();
}

