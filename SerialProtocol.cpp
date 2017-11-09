#include "SerialProtocol.h"
#include "Arduino.h"

/*
    Msg format:
    (CMD:PL1;PL2;...PLn)
*/

SerialProtocol::SerialProtocol(FuncHandler fh){
    fhandler = fh;
}

void SerialProtocol::sendData(String a, String b){
    Serial.println();
}

void SerialProtocol::sendData(String a, int b){
}

void SerialProtocol::sendData(String a, float b){
}

void SerialProtocol::sendData(SensData sd){
}

void SerialProtocol::run(){

}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length()-1;

    for(int i=0; i<=maxIndex && found<=index; i++){
        if(data.charAt(i)==separator || i==maxIndex){
            found++;
            strIndex[0] = strIndex[1]+1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void SerialProtocol::parse(String a){
    String topic;
    String res[10];
    if(a.startsWith("(") && a.endsWith(")")){
        topic = a.substring(1, a.find(":"));
        a = a.substring(a.find(":")+1);
        int i = 0;
        do {
            String f = getValue(a, ';', i++);
            if(f != "")
                res[i] = f;
        } while(f != "");
    }
    this->fhandler(topic, res);
}