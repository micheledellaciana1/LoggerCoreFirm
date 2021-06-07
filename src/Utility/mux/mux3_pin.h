
#ifndef MUX3_PIN
#define MUX3_PIN

#include <Arduino.h>
#include "Vmux.h"

/**
 * @brief mux3_pin is a virtual class, son of Vmux, that specify a device with 3 pin to control the logic, plus one pin to enable or disable the mux.
 */

class mux3_pin : public Vmux
{
protected:
    int16_t _S0; ///< pin that drives S0
    int16_t _S1; ///< pin that drives S1
    int16_t _S2; ///< pin that drives S2

    int16_t _enablePIN; ///< pin that drives enable,

    /**
     * @brief This function prepare the pins to select a line (set pins to low impedence);
     */
    void _prepareSelectLine()
    {
        pinMode(_S2, OUTPUT);
        pinMode(_S1, OUTPUT);
        pinMode(_S0, OUTPUT);
    }

public:
    /**
     * @brief Constructor of mux3_pin.
     * @param S0 pin that drives S0.
     * @param S1 pin that drives S1.
     * @param S2 pin that drives S2.
     * @param enablePIN pin that drives enable, -1 if is enabled by hardware link.
     */
    mux3_pin(int16_t S0, int16_t S1, int16_t S2, int16_t enablePIN) : Vmux(8)
    {
        _enablePIN = enablePIN;
        _S0 = S0;
        _S1 = S1;
        _S2 = S2;
    }
};

#endif