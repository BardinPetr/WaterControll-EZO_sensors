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
}

void loop() {
  //To perform calibration type in serial monitor: <sensor id>:c.<caltype>=<cal value>
  if(Serial.available()){
    char res[10];
    Serial.readBytesUntil(13, res, 10);

    int ch = String(strtok(res, ":")).toInt();
    String cmd = String(strtok(NULL, ":"));
    
    if(cmd.startsWith("c")){
      strtok(cmd.c_str(), ".");
      String cr = String(strtok(NULL, "."));
      String type = String(strtok(cr.c_str(), "="));
      float val = String(strtok(NULL, "=")).toFloat();

      sens.calibrate(ch, type, val, true);
    }
  }
}

