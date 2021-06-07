#ifndef LedDriver_H
#define LedDriver_H

#include <Arduino.h>
#include <Utility/EasyADC.h>
#include <Utility/EasyDAC.h>

class LedDriver
{

private:
    int16_t _pinDACCurrent;
    EasyDAC *_DACCurrent;

    float _refResistor;

public:
    LedDriver(int16_t pinDACCurrent, EasyDAC *DACCurrent, float refResistor, float frequency = 117187.5)
    {
        _pinDACCurrent = pinDACCurrent;
        _DACCurrent = DACCurrent;

        _refResistor = refResistor;
        analogWriteFrequency(_pinDACCurrent, frequency);
    }

    ~LedDriver() {}

    void setCurrentValue(float milliAmps)
    {
        analogWrite(_pinDACCurrent, _DACCurrent->getVoltageI(milliAmps * 1e-3 * _refResistor));
    }
};

#endif // HeaterDriverCC_H