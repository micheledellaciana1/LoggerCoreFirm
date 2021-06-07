
#ifndef MUXMAX338_8_1_H
#define MUXMAX338_8_1_H

#include "mux3_pin.h"
#include <Arduino.h>

/**
 * @brief muxMAX338_8_1 is an implementation of mux3_pin to be used with the device MAX338.
 * @details MAX338 is an Analog Multiplexer that switches 8 lines in one line.
 */

class muxMAX338_8_1 : public mux3_pin
{

public:
    /**
     * @brief Constructor of muxMAX338_8_1.
     * @details By default is selected line 0 and the mux is disabled.
     * @param S0 pin that drive A0.
     * @param S1 pin that drive A1.
     * @param S2 pin that drive A2.
     * @param _enablePIN pin that drive enable, -1 if is enable by hardware link.
     */
    muxMAX338_8_1(int16_t S0, int16_t S1, int16_t S2, int16_t enablePin) : mux3_pin(S0, S1, S2, enablePin)
    {
        disable();
        selectLine(0);
    }

    /**
    *@brief this function switch COMMON to a specific line.
    *@param line is the line to be selected;
    */
    void selectLine(int16_t line)
    {
        _selectedLine = line;
        _prepareSelectLine();
        switch (line)
        {
        case 0:
            digitalWriteFast(_S2, LOW);
            digitalWriteFast(_S1, LOW);
            digitalWriteFast(_S0, LOW);
            break;
        case 1:
            digitalWriteFast(_S2, LOW);
            digitalWriteFast(_S1, LOW);
            digitalWriteFast(_S0, HIGH);
            break;
        case 2:
            digitalWriteFast(_S2, LOW);
            digitalWriteFast(_S1, HIGH);
            digitalWriteFast(_S0, LOW);
            break;
        case 3:
            digitalWriteFast(_S2, LOW);
            digitalWriteFast(_S1, HIGH);
            digitalWriteFast(_S0, HIGH);
            break;
        case 4:
            digitalWriteFast(_S2, HIGH);
            digitalWriteFast(_S1, LOW);
            digitalWriteFast(_S0, LOW);
            break;
        case 5:
            digitalWriteFast(_S2, HIGH);
            digitalWriteFast(_S1, LOW);
            digitalWriteFast(_S0, HIGH);
            break;
        case 6:
            digitalWriteFast(_S2, HIGH);
            digitalWriteFast(_S1, HIGH);
            digitalWriteFast(_S0, LOW);
            break;
        case 7:
            digitalWriteFast(_S2, HIGH);
            digitalWriteFast(_S1, HIGH);
            digitalWriteFast(_S0, HIGH);
            break;
        default:
            break;
        }
    }

    /**
     * @brief Enable the mux.
     * @return Returns true if possible;
     */
    bool enable()
    {
        if (_enablePIN < 0)
            return false;

        _enable = true;
        pinMode(_enablePIN, OUTPUT);
        digitalWriteFast(_enablePIN, HIGH);

        return true;
    }

    /**
     * @brief Disable the mux.
     * @return Returns true if possible;
     */
    bool disable()
    {
        _enable = false;

        if (_enablePIN < 0)
            return false;

        pinMode(_enablePIN, OUTPUT);
        digitalWriteFast(_enablePIN, LOW);

        return true;
    }
};

#endif