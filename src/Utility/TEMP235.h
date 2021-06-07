#ifndef TEMP235_H
#define TEMP235_H

#include <Arduino.h>
#include "EasyADC.h"

class TEMP235
{
private:
    EasyADC *_ADC;
    int _pinTemp;

public:
    TEMP235(int pinTemp, EasyADC *ADC)
    {
        _pinTemp = pinTemp;
        _ADC = ADC;
    }

    float readTemp()
    {
        double Millivolt = _ADC->getVoltagef(analogRead(_pinTemp)) * 1000;

        if (Millivolt <= 1500)
            return (Millivolt - 500) / 10. + 0;
        if (Millivolt <= 1752.5 && Millivolt > 1500)
            return (Millivolt - 1500) / 10.1 + 100;
        if (Millivolt > 1752.5)
            return (Millivolt - 1752.5) / 10.6 + 125;
    }

    ~TEMP235() {}
};

#endif // !TEMP235.h