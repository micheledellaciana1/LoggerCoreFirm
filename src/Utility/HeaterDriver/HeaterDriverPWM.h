#ifndef HEATERDRIVERPWM_H
#define HEATERDRIVERPWM_H

#include <Utility/EasyADC.h>
#include <Utility/EasyDAC.h>

class HeaterDriverPWM
{
private:
    int16_t _pinPWM;
    EasyDAC *_PWMDAC;
    int16_t _PWMvalue;

    int16_t _pinADCVoltageFall;
    EasyADC *_adcVoltageFall;
    int16_t _pinADCCurrent;
    EasyADC *_adcCurrent;

    float _refResistor;

    int16_t _PINgate;

public:
    long microsDelayMeasurementMode = 50;

    HeaterDriverPWM(int16_t PINgate, int16_t pinPWM, EasyDAC *PWMDAC, int16_t pinADCVoltageFall, EasyADC *adcVoltageFall, int16_t pinADCCurrent, EasyADC *adcCurrent, float refResistor)
    {
        _PINgate = PINgate;
        pinMode(PINgate, OUTPUT);
        digitalWrite(PINgate, HIGH);

        _pinPWM = pinPWM;
        _PWMDAC = PWMDAC;
        pinMode(pinPWM, OUTPUT);
        analogWrite(pinPWM, 0);

        _pinADCVoltageFall = pinADCVoltageFall;
        _adcVoltageFall = adcVoltageFall;
        _pinADCCurrent = pinADCCurrent;
        _adcCurrent = adcCurrent;

        _refResistor = refResistor;
    }

    ~HeaterDriverPWM() {}

    void setCurrentValue(float value)
    {
        int16_t oldValue = _PWMvalue;
        _PWMvalue = _PWMDAC->getVoltageI(value);
        if (oldValue != _PWMvalue)
            analogWrite(_pinPWM, _PWMvalue);
    }

    float getCurrentValue()
    {
        return _adcCurrent->getVoltagef(analogRead(_pinADCCurrent)) / _refResistor;
    }

    float getVoltageFall()
    {
        return _adcVoltageFall->getVoltagef(analogRead(_pinADCVoltageFall));
    }

    float getHeaterResistanceValue()
    {
        return getVoltageFall() / getCurrentValue();
    }

    void setInMeasurementMode()
    {
        digitalWriteFast(_PINgate, 1); //is inverted
        digitalWriteFast(_pinPWM, 1);
        delayMicroseconds(microsDelayMeasurementMode);
    }

    void setInSupplyMode()
    {
        digitalWriteFast(_PINgate, 0);
        analogWrite(_pinPWM, _PWMvalue);
    }

    int getPWMPin()
    {
        return _pinPWM;
    }
};
#endif // ! HEATERDRIVERPWM_H