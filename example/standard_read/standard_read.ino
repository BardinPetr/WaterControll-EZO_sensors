#include "WaterControll.h"
WaterControll sens(Serial, 2, 3, 4, false);

void setup() {
  Serial.begin(9600);
  sens.Add(sens.PH, "pH", true);             //Add the pH sensor on Serial
  sens.Add(sens.OPR, "OPR", true);           //Add the OPR sensor on Serial
  sens.Add(sens.DO, "Oxygen", true);         //Add the Oxygen sensor on Serial
  sens.Add(sens.COND, "Conductivity", false);//Add the Conductivity sensor on I2C
  sens.Add(sens.RTD, "Temperature", false);  //Add the Temperature sensor on I2C
  sens.Add(sens.FLOW, "Flow", true);         //Add the Flow sensor on Serial
  
  sens.set_finishHandler(finish);
}

void finish(float* in){
  Serial.println("\n\n***************\nData from sensors recieved!");
  for (int i = 0; i < sens.sensors_count; i++){
    Serial.println(sens.sensors_name[i] + ":\t\t\t\t" + String(in[i]));
  }
  Serial.println("***************\n");
}

void loop() {
  sens.readAll_nw();
}

