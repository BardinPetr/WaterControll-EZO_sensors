#include "WaterControll.h"
WaterControll sens;

void setup() {
  Serial.begin(9600);
  sens.Add(0, "pH", 3);
}

void loop() {
  
}

