#include "WaterControll.h"
#include "Arduino.h"
#include <Wire.h>
#include <inttypes.h>

int parse_code(String* in){
  return atoi(in[0].c_str());
}
int parse_value(String* in){
  return atof(in[1].c_str());
}

WaterControll::WaterControll(){
    Wire.begin();
}

void WaterControll::readAll_w(){
    Serial.println("[WARNING] This operation take many time! From 1 second to 8");
    for(int i = 0; i < WaterControll::sensors_count; i++){
        WaterControll::read_data_w(i);
    }
}
float* WaterControll::readAll_nw(){
    if(WaterControll::now_sens == WaterControll::sensors_count){
      WaterControll::finished = true;
      return WaterControll::sensors_data;
    }
    else{
      float res = WaterControll::read_data_nw(WaterControll::now_sens);
      if(res != -255)
          WaterControll::now_sens++;
      else
          WaterControll::sensors_data[now_sens] = res;
    }
}

void WaterControll::sendCommand(int id, String str){
    Wire.beginTransmission(WaterControll::sensors_addr[id]); 
    Wire.write(str.c_str());
    Wire.endTransmission();
}

void WaterControll::calibrate(int id, int type, float val, bool wait = false){
    String cal_type[3] = {"mid", "low", "high"};

    WaterControll::sendCommand(id, String("Cal," + cal_type[type] + "," + String(val)));
        
    if(WaterControll::debug){
      Serial.println("[SENSORS] Calibration request sended for sensor: " + String(WaterControll::sensors_name[WaterControll::sensors_count-1]));
    }

    if(wait){
      Serial.println("[SENSORS] Please wait for calibration respone for sensor: " + String(WaterControll::sensors_name[WaterControll::sensors_count-1]));
      delay(1600);

      String* a = WaterControll::requestFrom(id);
      int code = parse_code(a);
      //float rec_data = parse_value(a);
      if(code == 1)
        Serial.println("[SENSOR] Calibration sucssfull");
    }
}

void WaterControll::set_debug(bool state){
    WaterControll::debug = state;
}

void WaterControll::Add(int type, String name, int addr){
    WaterControll::sensors_type[WaterControll::sensors_count] = type;
    WaterControll::sensors_addr[WaterControll::sensors_count] = (addr == -1 ? WaterControll::std_addr[type] : addr);
    WaterControll::sensors_name[WaterControll::sensors_count++] = name;
    
    if(WaterControll::debug){
      Serial.println("[SENSORS] New sensor: " + name + "; type: " + WaterControll::types[type] + "; ID: " + String(WaterControll::sensors_count-1));
    }
}

void WaterControll::request_data(int id){ 
  WaterControll::sendCommand(id, "r");
}

String* WaterControll::requestFrom(int id){
    char sd[30];            
    byte in_char = 0;      
    int sensor_bytes_received = 0;

    memset(sd, 0, sizeof(sd));

    Wire.requestFrom(WaterControll::sensors_addr[id], 48, 1);
    byte code = Wire.read();

    while (Wire.available()) { 
      in_char = Wire.read();

      if (in_char == 0) {     
        Wire.endTransmission();    
        break;             
      }
      else {
        sd[sensor_bytes_received] = in_char;      
        sensor_bytes_received++;
      }
    }
    String out[2] = {String(code), String(sd)};
    return out;
}

float WaterControll::read_data_w(int id){
    char sensordata[30];            
    byte in_char = 0;      
    int sensor_bytes_received = 0;

    WaterControll::request_data(id);
    delay(1000);

    String* a = WaterControll::requestFrom(id);
    int code = parse_code(a);
    float rec_data = parse_value(a);

    switch (code) {           
      case 1:                   
        Serial.println(String("[SENSOR] " + WaterControll::sensors_name[id] + " - Data recieved: " + a[1]));  
        break;             

      case 2:                    
        Serial.println("[SENSOR] Command failed");
        break;                         	   
        
      case 254:                      
        Serial.println("[SENSOR] Circuit not ready"); 
        break;                         	 

      case 255: 
        Serial.println("[SENSOR] No data");         
        break;                         	   
    }
    return rec_data;
}

float WaterControll::read_data_nw(int id){
  char sensordata[30];            
  byte in_char = 0;      
  int sensor_bytes_received = 0;

  if(WaterControll::reading){
    if((millis() - WaterControll::read_start) > 1000){
      String* a = WaterControll::requestFrom(id);
      int code = parse_code(a);
      float rec_data = parse_value(a);

      switch (code) {           
        case 1:                   
          Serial.println(String("[SENSOR] " + WaterControll::sensors_name[id] + " - Data recieved: " + a[1]));  
          break;             

        case 2:                    
          Serial.println("[SENSOR] Command failed");
          break;                         	   
          
        case 254:                      
          Serial.println("[SENSOR] Circuit not ready"); 
          break;                         	 

        case 255: 
          Serial.println("[SENSOR] No data");         
          break;                         	   
      }
      WaterControll::read_start = 0;
      WaterControll::reading = false;
      return rec_data;
    }
  } else {
    WaterControll::request_data(id);
    WaterControll::read_start = millis();
    WaterControll::reading = true;
  }
  return -255;  
}