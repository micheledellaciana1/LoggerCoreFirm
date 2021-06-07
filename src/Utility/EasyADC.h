#ifndef EasyADC_H
#define EasyADC_H

#include <Arduino.h>

class EasyADC
{
private:
    float _convertFactor;
    float _InverseConvertFactor;

    float _V_min;
    float _V_max;

    int16_t _usedBitResolution;

public:
    static EasyADC ADC_0_3_3;
    static int16_t bitResolution;

    EasyADC(float V_min, float V_max)
    {
        _V_min = V_min;
        _V_max = V_max;

        _convertFactor = (V_max - V_min) / ((1 << bitResolution) - 1);
        _InverseConvertFactor = 1. / _convertFactor;
        _usedBitResolution = bitResolution;
    }

    ~EasyADC() {}

    float getVoltagef(int32_t voltageI)
    {
        if (bitResolution != _usedBitResolution)
        {
            _convertFactor = (_V_max - _V_min) / ((1 << bitResolution) - 1);
            _InverseConvertFactor = 1. / _convertFactor;
            _usedBitResolution = bitResolution;
        }

        return voltageI * _convertFactor + _V_min;
    }

    int16_t getVoltageI(float voltagef)
    {
        if (bitResolution != _usedBitResolution)
        {
            _convertFactor = (_V_max - _V_min) / ((1 << bitResolution) - 1);
            _InverseConvertFactor = 1. / _convertFactor;
            _usedBitResolution = bitResolution;
        }

        return round((voltagef - _V_min) * _InverseConvertFactor);
    }

    const float &getV_max()
    {
        return _V_max;
    }

    const float &getV_min()
    {
        return _V_min;
    }
};

int16_t EasyADC::bitResolution = 10;
EasyADC EasyADC::ADC_0_3_3 = EasyADC(0, 3.3);

#endif // !EasyADC_H
