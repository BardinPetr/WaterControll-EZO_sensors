#ifndef _SERIALPROTOCOL_H_
#define _SERIALPROTOCOL_H_

struct SensData{
    double pH, DO, OPR, COND, TDS, SAL, SG, T, FLOW;
};

class SerialProtocol{
public:
    typedef void (*FuncHandler)(String, String*);
    SerialProtocol(FuncHandler);

    void sendData(String, String);
    void sendData(String, int);
    void sendData(String, float);
    void sendData(SensData);

    void run();

    void parse(String);
    FuncHandler fhandler;
};

#endif
