#ifndef _WATERCONTROLL_H_
#define _WATERCONTROLL_H_
 
#include <inttypes.h>
#include <avr/io.h>
//#include "arduino.h"
#include <SoftwareSerial.h>
#include <Wire.h>
#include <AltSoftSerial.h>
#include "SerialProtocol.h"

class WaterControll{
public:
    typedef void (*FuncHandler)(float*);

    WaterControll(); //Begin i2c
    WaterControll(SoftwareSerial &, int, int, int, bool); //Begin with software serial
    WaterControll(HardwareSerial &, int, int, int, bool); //Begin with hardware serial

    void Add(int, String, int);
    void Add(int, String, bool);

    void request_data(int);
    float read_data_w(int);
    float read_data_nw(int);

    void sendCommand(int, String);
    String requestFrom(int id);
    float requestFrom_f(int id);
    void calibrate(int, String, float, bool);

    void readAll_w();
    void readAll_nw();

    void set_debug(bool);
    void set_finishHandler(FuncHandler);
    void set_addr(int, int);

    float rf(int);

    bool reading = false;
    long int read_start = 0;
    int now_sens = 0;
    bool finished = false;

    int max_count = 8;
    int sensors_count = 0;

    int sensors_type[8];
    int sensors_addr[8];
    String sensors_name[6];
    float sensors_data[8];
    bool sensors_iface[8];

    int std_addr[6] = {99, 98, 97, 100, 102, 0, 103};

    String types[6] = {"PH", "OPR", "Dissloved Oxygen", "Conductivity", "Temperature", "Flow", "Pump"};
    bool debug = true;
 
    FuncHandler finishHandler;

    String C_MID = "mid";
    String C_LOW = "low";
    String C_HIGH = "high";
    String C_TEMP = "t";
    
    enum {
    PH,    // 0 -pH Circuit
    OPR,   // 1 -OPR Circuit
    DO,    // 2 -Dissolved Oxygen Circuit
    COND,  // 3 -Conductivity Circuit
    RTD,   // 4 -RTD Temperature Circuit
    FLOW,  // 5 -Flow sensor
    PUMP   // 6 -Pump
    };

private:
    void ch(int);

    int p1, p2, p3;
    int runId =0;
    bool type = false;
};
 
#endif
