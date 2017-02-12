#ifndef _WATERCONTROLL_H_
#define _WATERCONTROLL_H_

#include <inttypes.h>
#include "Arduino.h"
#include "Arduino.h"
#include <Wire.h>

class WaterControll{
public:
    typedef void (*FuncHandler)(float*);

    WaterControll();

    void Add(int, String, int);
    void Add(int, String);

    void request_data(int);
    float read_data_w(int);
    float read_data_nw(int);

    void sendCommand(int, String);
    String* requestFrom(int id);
    void calibrate(int, int, float, bool);

    void readAll_w();
    void readAll_nw();

    void set_debug(bool);
    void set_finishHandler(FuncHandler);
    void set_addr(int, int);

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

    int std_addr[6] = {63, 63, 63, 63, 63, 63};

    String types[6] = {"PH", "OPR", "Dissloved Oxygen", "Conductivity", "Temperature", "Flow"};
    bool debug = true;

    FuncHandler finishHandler;

    enum {
    PH,    // 0 -pH Circuit
    OPR,   // 1 -OPR Circuit
    DO,    // 2 -Dissolved Oxygen Circuit
    COND,  // 3 -Conductivity Circuit
    RTD,   // 4 -RTD Temperature Circuit
    FLOW   // 5 -Flow sensor
    };
};
 
#endif