
#ifndef SHT31_ARP_B2_5kS_H
#define SHT31_ARP_B2_5kS_H

#include <Arduino.h>
#include "EasyADC.h"

class SHT31_ARP_B2_5kS
{
private:
    EasyADC *_TA;
    int16_t _PTA;
    EasyADC *_RHA;
    int16_t _PRHA;

    float _Vcc;

public:
    SHT31_ARP_B2_5kS(int pinTemp, int pinRH, float Vcc) : _Vcc(Vcc)
    {
        _TA = new EasyADC(0, Vcc);
        _PTA = pinTemp;
        _RHA = new EasyADC(0, Vcc);
        _PRHA = pinRH;
    }

    float readRH()
    {
        float RH = -12.5 + 125 * _RHA->getVoltagef(analogRead(_PRHA)) / _Vcc;
        if (RH < 0)
            return 0;
        else
            return RH;
    }

    float readTemp()
    {
        return -66.875 + 218.75 * _TA->getVoltagef(analogRead(_PTA)) / _Vcc;
    }

    ~SHT31_ARP_B2_5kS()
    {
        delete _RHA;
        delete _TA;
    }
};
#endif // !SHT31_ARP_B2_5kS_H