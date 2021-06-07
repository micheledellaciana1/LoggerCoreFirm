#ifndef AMPMETER_H
#define AMPMETER_H

#include "EasyADC.h"
#include "mux\Vmux.h"

class AmpMeter
{
private:
    EasyADC *_CR;
    int16_t _PCR;
    Vmux *_mux;

    float *_RefResistors;

    float _MaxVoltageFall;

    float _lowTrigger(float CurrentBestEstimation)
    {
        if (_mux->PossibleToselectSuccLine())
            return _MaxVoltageFall * _RefResistors[_mux->getSelectedLine()] / _RefResistors[_mux->getSelectedLine() + 1];

        return 0;
    }

public:
    bool Autorange = true;
    bool BlinkOnAutorange = true;
    float ReactivityAutorange = 0.4;

    int16_t maxIndexUsableFromAutorange;
    int16_t minIndexUsableFromAutorange;

    long microsecondsDelay = 50000L; ///< Delay in microseconds after a change in reference resistor.
    int16_t millisAverange = 0;

    AmpMeter(int16_t PinCReader, EasyADC *CReader, Vmux *mux, const float *RefResistors, float MaxVoltageFall)
    {
        _CR = CReader;
        _PCR = PinCReader;

        _mux = mux;
        setMaxVoltageFall(MaxVoltageFall);

        _RefResistors = new float[mux->getNLine()];
        for (int16_t i = 0; i < mux->getNLine(); i++)
            _RefResistors[i] = RefResistors[i];

        maxIndexUsableFromAutorange = _mux->getNLine();
        minIndexUsableFromAutorange = 0;
    }

    ~AmpMeter()
    {
        delete[] _RefResistors;
    }

    float MeasureCurrent()
    {
        float readVoltage;
        float CurrentBestEstimation;

        readVoltage = _CR->getVoltagef(analogRead(_PCR));
        CurrentBestEstimation = readVoltage / _RefResistors[_mux->getSelectedLine()];

        if (Autorange)
        {
            for (int i = minIndexUsableFromAutorange; i < maxIndexUsableFromAutorange; i++)
            {
                float LT = _lowTrigger(CurrentBestEstimation) * ReactivityAutorange;

                if (fabs(readVoltage) < _MaxVoltageFall && fabs(readVoltage) > LT)
                    break;

                if (fabs(readVoltage) < LT)
                {
                    if (_mux->getSelectedLine() < maxIndexUsableFromAutorange)
                    {
                        _mux->selectSuccLine();

                        if (BlinkOnAutorange)
                            blink();
                        else
                            delayMicroseconds(microsecondsDelay);

                        readVoltage = _CR->getVoltagef(analogRead(_PCR));
                        CurrentBestEstimation = readVoltage / _RefResistors[_mux->getSelectedLine()];
                    }
                }

                if (fabs(readVoltage) > _MaxVoltageFall)
                {
                    if (_mux->getSelectedLine() > minIndexUsableFromAutorange)
                    {
                        _mux->selectPrecLine();

                        if (BlinkOnAutorange)
                            blink();
                        else
                            delayMicroseconds(microsecondsDelay);

                        readVoltage = _CR->getVoltagef(analogRead(_PCR));
                        CurrentBestEstimation = readVoltage / _RefResistors[_mux->getSelectedLine()];
                    }
                }
            }
        }

        return CurrentBestEstimation;
    }

    inline void setMaxVoltageFall(float NewMaxVoltageFall) { _MaxVoltageFall = fabs(NewMaxVoltageFall); }
    inline const float &getMaxVoltageFall() const { return _MaxVoltageFall; }
    inline Vmux *getMux() { return _mux; }
    inline float *getRefResistors() { return _RefResistors; }

private:
    void blink()
    {
        pinMode(13, OUTPUT);
        digitalWrite(13, HIGH);
        delayMicroseconds(microsecondsDelay);
        digitalWrite(13, LOW);
        pinMode(13, INPUT);
        delayMicroseconds(microsecondsDelay);
    }
};

#endif
