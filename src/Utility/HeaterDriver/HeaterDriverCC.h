#ifndef HeaterDriverCC_H
#define HeaterDriverCC_H

#include <Arduino.h>
#include <Utility/EasyADC.h>
#include <Utility/EasyDAC.h>

class HeaterDriverCC
{

private:
    int16_t _pinDACCurrent;
    EasyDAC *_DACCurrent;
    int16_t _pinADCVoltageFall;
    EasyADC *_adcVoltageFall;
    int16_t _pinADCCurrent;
    EasyADC *_adcCurrent;

    float _refResistor;

public:
    HeaterDriverCC(int16_t pinDACCurrent, EasyDAC *DACCurrent, int16_t pinADCVoltageFall, EasyADC *adcVoltageFall, int16_t pinADCCurrent, EasyADC *adcCurrent, float refResistor)
    {
        _pinDACCurrent = pinDACCurrent;
        _DACCurrent = DACCurrent;
        _pinADCVoltageFall = pinADCVoltageFall;
        _adcVoltageFall = adcVoltageFall;
        _pinADCCurrent = pinADCCurrent;
        _adcCurrent = adcCurrent;

        _refResistor = refResistor;
    }

    ~HeaterDriverCC() {}

    void setCurrentValue(float milliAmps)
    {
        analogWrite(_pinDACCurrent, _DACCurrent->getVoltageI(milliAmps * 1e-3 * _refResistor));
    }

    float getCurrentValue()
    {
        return _adcCurrent->getVoltagef(analogRead(_pinADCCurrent)) / _refResistor;
    }

    float getVoltageFall()
    {
        return _adcVoltageFall->getVoltagef(analogRead(_pinADCVoltageFall));
    }

    float getHeaterResistanceValue_averanging(int times)
    {
        long VoltageCurrent = 0;
        for (size_t i = 0; i < times; i++)
            VoltageCurrent += analogRead(_pinADCCurrent);
        VoltageCurrent /= times;

        _adcCurrent->getVoltagef(VoltageCurrent / _refResistor);

        long Voltage = 0;
        for (size_t i = 0; i < times; i++)
            Voltage += analogRead(_pinADCCurrent);
        Voltage /= times;

        return _adcVoltageFall->getVoltagef(Voltage) / _adcCurrent->getVoltagef(VoltageCurrent / _refResistor);
    }
};

#endif // HeaterDriverCC_H