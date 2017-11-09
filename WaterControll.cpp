#include "WaterControll.h"
#include "Arduino.h"
#include <Wire.h>
#include <inttypes.h>
#include <avr/io.h>
#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
 
static HardwareSerial * hser = NULL;
static SoftwareSerial * sser = NULL;
int p1, p2, p3;
int runId =0;
bool type = false;

void WaterControll::ch(int id = -1){
  if (id == -1){
    runId ++;
    if(runId > WaterControll::sensors_count)
      runId = 0;
  }
  else{
    runId = id+1;
  }

  digitalWrite(p1, bitRead(runId, 0));
  digitalWrite(p2, bitRead(runId, 1));
  digitalWrite(p3, bitRead(runId, 2));
}

int parse_code(String* in){
  return atoi(in[0].c_str());
}
int parse_value(String* in){
  return atof(in[1].c_str());
}

WaterControll::WaterControll(){
    Wire.begin();
}

WaterControll::WaterControll(SoftwareSerial &theSerial, int _p1 = -1, int _p2 = -1, int _p3 = -1, bool i2c = false){
    type = false;

    sser = &theSerial;
    sser->begin(9600);
    if(i2c){
      Wire.begin();
    }

    if (_p1 != -1 && _p2 != -1 && _p3 != -1){
      p1 = _p1;
      p2 = _p2;
      p3 = _p3;
    }
}

WaterControll::WaterControll(HardwareSerial &theSerial, int _p1 = -1, int _p2 = -1, int _p3 = -1, bool i2c = false){
    type = true;

    hser = &theSerial;
    hser->begin(9600);
    if(i2c){
      Wire.begin();
    }

    if (_p1 != -1 && _p2 != -1 && _p3 != -1){
      p1 = _p1;
      p2 = _p2;
      p3 = _p3;
    }

    pinMode(p1, OUTPUT);
    pinMode(p2, OUTPUT);
    pinMode(p3, OUTPUT);
}

/*
 * Add sensor methods
*/
void WaterControll::Add(int type, String name, int addr){
    WaterControll::sensors_type[WaterControll::sensors_count] = type;
    WaterControll::sensors_addr[WaterControll::sensors_count] = addr;
    WaterControll::sensors_iface[WaterControll::sensors_count] = false;
    WaterControll::sensors_name[WaterControll::sensors_count++] = name;
    
    if(WaterControll::debug){
      Serial.println("[SENSORS] New sensor: " + name + "; type: " + WaterControll::types[type] + "; ID: " + String(WaterControll::sensors_count-1));
    }
}
void WaterControll::Add(int type, String name, bool iface){
    WaterControll::sensors_type[WaterControll::sensors_count] = type;
    WaterControll::sensors_addr[WaterControll::sensors_count] = WaterControll::std_addr[type];
    WaterControll::sensors_name[WaterControll::sensors_count] = name;
    WaterControll::sensors_iface[WaterControll::sensors_count++] = iface;
    
    if(WaterControll::debug){
      Serial.println("[SENSORS] New sensor: " + name + "; type: " + WaterControll::types[type] + "; ID: " + String(WaterControll::sensors_count-1));
    }
}

/*
 *
*/
void WaterControll::set_addr(int id, int addr){
    WaterControll::sendCommand(id, String("I2c," + String(addr)));
    Serial.println("[SENSOR] " + WaterControll::sensors_name[id] + ": I2C address changed! Sensor disconnected!");
}

void WaterControll::set_finishHandler(FuncHandler f){
    WaterControll::finishHandler = f;
}

void WaterControll::readAll_w(){
    Serial.println("[WARNING] This operation take many time! From 1 second to 8");
    for(int i = 0; i < WaterControll::sensors_count; i++){
        WaterControll::read_data_w(i);
    }
}

void WaterControll::readAll_nw(){
    if(WaterControll::finished){
      WaterControll::finishHandler(WaterControll::sensors_data);
      WaterControll::finished = false;
      WaterControll::now_sens = 0;
    }

    if(WaterControll::now_sens == WaterControll::sensors_count){
      WaterControll::finished = true;
    }
    
    else{
      float res = WaterControll::read_data_nw(WaterControll::now_sens);
      if(res != -255){
          //Serial.print("res: " + String(res));
          WaterControll::sensors_data[now_sens] = res;
          WaterControll::now_sens++;
      }
    }
}

void WaterControll::sendCommand(int id, String str){
    if(WaterControll::sensors_iface[id] == false){
      Wire.beginTransmission(WaterControll::sensors_addr[id]); 
      Wire.write(str.c_str());
      Wire.endTransmission();
    }
    else{
      ch(id);
      if(type == true){ //hser
        hser->print(str);
        hser->print("\r");
      }
      else{ //sser
        sser->print(str);
        sser->print("\r");
      }
    }
}

void WaterControll::calibrate(int id, String type, float val, bool wait = false){
    WaterControll::sendCommand(id, String("Cal," + type + "," + String(val)));
        
    if(WaterControll::debug){
      Serial.println("[SENSORS] Calibration request sended for sensor: " + String(WaterControll::sensors_name[WaterControll::sensors_count-1]));
    }

    if(wait){
      Serial.println("[SENSORS] Please wait for calibration respone for sensor: " + String(WaterControll::sensors_name[WaterControll::sensors_count-1]));
      delay(1600);

      String code = WaterControll::requestFrom(id);
      
      if(code == "1" || code == "*OK")
        Serial.println("[SENSOR] Calibration sucssfull");
    }
}

void WaterControll::set_debug(bool state){
    WaterControll::debug = state;
}

void WaterControll::request_data(int id){ 
  WaterControll::sendCommand(id, "r");
}

String WaterControll::requestFrom(int id){
  if(WaterControll::sensors_iface[id] == false){
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
    return String(sd);
  }
  else{
    ch(id);
    if(type){
      char sd[30];            
      byte in_char = 0;      
      int sensor_bytes_received = 0;

      memset(sd, 0, sizeof(sd));

      while(!hser->available());

      if (hser->available()) { 
          hser->readBytesUntil(13, sd, 30);
      }
      return String(sd);
    }
    else{
      char sd[30];            
      byte in_char = 0;      
      int sensor_bytes_received = 0;

      memset(sd, 0, sizeof(sd));

      while(!sser->available());

      if (sser->available()) { 
          sser->readBytesUntil(13, sd, 30);
      }
      return String(sd);
    }
  }
}

float WaterControll::requestFrom_f(int id){
  return WaterControll::requestFrom(id).toFloat();
}

float WaterControll::read_data_w(int id){
    char sensordata[30];            
    byte in_char = 0;      
    int sensor_bytes_received = 0;

    WaterControll::request_data(id);
    delay(1000);

    float rec_data = WaterControll::requestFrom_f(id);

    return rec_data;
}

float WaterControll::read_data_nw(int id){
  char sensordata[30];            
  byte in_char = 0;      
  int sensor_bytes_received = 0;

  if(WaterControll::reading){
    if((millis() - WaterControll::read_start) > 1500){
      float rec_data = WaterControll::requestFrom_f(id);

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