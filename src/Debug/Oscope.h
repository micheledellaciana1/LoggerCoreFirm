#ifndef OSCOPE_H
#define OSCOPE_H

#include <Arduino.h>
#include <functional>

class Oscope
{

private:
    uint16_t *_array = nullptr;
    int16_t _dim;

    long _period; //nano

    std::function<uint16_t(int16_t)> _analogRead;

public:
    Oscope(std::function<uint16_t(int16_t)> analogReadF, int16_t Dimension)
    {
        _dim = Dimension;
        _analogRead = analogReadF;
        _array = new uint16_t[Dimension];
    }

    void setOScope(std::function<uint16_t(int16_t)> analogReadF, int16_t Dimension)
    {
        delete[] _array;
        _dim = Dimension;
        _analogRead = analogReadF;
        _array = new uint16_t[Dimension];
    }

    ~Oscope()
    {
        delete[] _array;
    }

    uint16_t *acquireSingleChannel(uint16_t channel)
    {
        uint64_t start = nanos();

        for (int i = 0; i < _dim; i++)
            _array[i] = _analogRead(channel);

        _period = nanos() - start;

        return _array;
    }

    uint16_t *acquireMultiChannel(uint16_t *channels, uint16_t NChannel)
    {
        uint64_t start = nanos();

        for (int i = 0; i < _dim; i++)
            _array[i] = _analogRead(channels[i % NChannel]);

        _period = nanos() - start;

        return _array;
    }

    float getPeriod() // micros
    {
        return _period * 0.001;
    }

    int16_t getArraySize()
    {
        return _dim;
    }

    uint64_t nanos()
    {
        uint32_t count, current, istatus;

        __disable_irq();
        current = SYST_CVR;
        count = systick_millis_count;
        istatus = SCB_ICSR; // bit 26 indicates if systick exception pending
        __enable_irq();
        if ((istatus & SCB_ICSR_PENDSTSET) && current > 50)
            count++;
        current = ((F_CPU / 1000) - 1) - current;

        return (count * 1000000L) + (current * 1000000000L) / F_CPU;
    }
};

#endif // ! OSCOPE_H