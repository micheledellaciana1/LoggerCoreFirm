
#ifndef VMUX_H
#define VMUX_H

#include <Arduino.h>

/**
 * @brief Vmux is a pure virtual class that resume the behavior of a generic mux.
 * @details A generic Analog Multiplex is a devise that  behave like a switch. It has a logic tha controls which line is activated and a signal that enable the usage.
 * So a generic mux class needs the implementation of selectLine(), that depends from the specific device and configuration, and two function that
 * enable or disable the multiplexer.
 */

class Vmux
{
protected:
    int16_t _selectedLine; ///< Selected line.
    int16_t _nLine;        ///< Number of lines.
    bool _enable;          ///< Enable variable.

public:
    /**
    * @brief Destructor, does nothing.
    */
    virtual ~Vmux() {}

    /**
     * @brief Contruct the object with the number of lines.
     */
    Vmux(int16_t nLine) : _nLine(nLine) {}

    /**
     * @brief Returns the selected line.
     * @return Returns the selected line.
     */
    inline const int16_t &getSelectedLine() const
    {
        return _selectedLine;
    }

    /**
     * @brief Returns the number of lines.
     * @return Returns the number of line.
     */
    inline const int16_t &getNLine() const
    {
        return _nLine;
    }

    /**
     * @brief Pure virtual function that enable the mux.
     */
    virtual bool enable() = 0;

    /**
     * @brief Pure virtual function that disable the mux.
     */
    virtual bool disable() = 0;

    /**
     * @brief Pure virtual function that select a specific line.
     * @param line is the line to be selected.
     */
    virtual void selectLine(int16_t line) = 0;

    /**
     * @brief Select the line _selectedLine-1.
     * @return Returns true if the operation is permitted.
     */
    bool selectPrecLine()
    {
        if (_selectedLine > 0)
        {
            selectLine(_selectedLine - 1);
            return true;
        }
        return false;
    }

    /**
     * @brief Select the line _selectedLine+1.
     * @return Returns true if the operation is permitted.
     */
    bool selectSuccLine()
    {
        if (_selectedLine < _nLine - 1)
        {
            selectLine(_selectedLine + 1);
            return true;
        }
        return false;
    }

    /**
     * @brief Check if it's possible to select previous line.
     * @return Returns true if the operation is permitted.
     */
    bool PossibleToselectPrecLine()
    {
        if (_selectedLine > 0)
            return true;

        return false;
    }

    /**
     * @brief Check if it's possible to select next line.
     * @return Returns true if the operation is permitted.
     */
    bool PossibleToselectSuccLine()
    {
        if (_selectedLine < _nLine - 1)
            return true;

        return false;
    }

    const bool &iSEnable() const { return _enable; }
};

#endif