/**
 * @file LoopManager.h
 * @author Michele Della Ciana
 * @date 21 Jan 2019
*/

#ifndef LOOPMANAGER_H
#define LOOPMANAGER_H

#include "VMode.h"
#include <Arduino.h>

using namespace std;

/**
 * @brief This class is the kernel of every program.
 * @details The class LoopManager is a Singleton class accessible via getInstance().
 *  It stores VMode ptr and recursively call run() fuction of every stored VMode.
 *  Many times you have a priority in calling different modes, so there is also a scheduler that guarantees
 * that every mode as at least a delay of \par PeriodMode.
 * Run() function has to be called inside loop() function to recursively execute run() funtion of every mode loaded.
*/

class LoopManager
{

private:
    VMode **_modes;                ///< Ptr of loaded modes
    int64_t *_PeriodMode;          ///< Ptr of minimum time distance beetwen execution of corrispondig mode
    int64_t *_timeLastCallMode;    ///< Stores the absolute time of the last execution of corrispondig mode.
    static LoopManager *_instance; ///< Istance of the singleton class

    int16_t _NloadedModes; ///< Number of loaded Modes
    int16_t _NModeMax;     ///< Indicate the memory held to load modes.

private:
    /**
* @brief Private Constructor of the class
* @details It hel the memory needed to allocate \par NModeMax modes.
*/

    LoopManager(int16_t NModeMax)
    {
        _modes = new VMode *[NModeMax];
        _timeLastCallMode = new int64_t[NModeMax];
        _PeriodMode = new int64_t[NModeMax];
        _NloadedModes = 0;
        _NModeMax = NModeMax;
    }

public:
    /**
     *  @brief Destructor of the class.
     * @details Because is a singleton class in normal condiction is called one time at the end of the program so 
     * you don't care. It deallocated every modes, calling their destructors ad initialize every pointer to \par nullptr.
     * At the end it deallocates the pointer to the modes, reinitializing it to nullptr. 
     */
    virtual ~LoopManager()
    {
        for (int16_t i = 0; i < _NloadedModes; i++)
        {
            delete _modes[i];
            _modes[i] = nullptr;
        }

        delete[] _modes;
        _modes = nullptr;
    }

    /**
     * @brief Funtion to access the singleton instance of the class
     * @return Returns the instance.
     */

    static LoopManager *getInstance()
    {
        return _instance;
    }

    /**
     * @brief Funtion to load a new mode. It's usually called during the setup step.
     * @details \par newMode is a ptr to the mode to be added.
     */

    void add_mode(VMode *newMode)
    {
        _modes[_NloadedModes] = newMode;
        _timeLastCallMode[_NloadedModes] = 0;
        _PeriodMode[_NloadedModes] = 0;
        _NloadedModes++;
    }

    /**
     * @brief Funtion to load a new mode. It's usually called during the setup step.
     * @details \par newMode is a ptr to the mode to be added. \par PeriodMode defines the minimum 
     * time interval beetwen consecutive execution of \par newMode.
     */

    void add_mode(VMode *newMode, float PeriodMode)
    {
        _modes[_NloadedModes] = newMode;
        _timeLastCallMode[_NloadedModes] = 0;
        _PeriodMode[_NloadedModes] = PeriodMode * 1e3;
        _NloadedModes++;
    }

    /**
     * @brief After the setup step, \par run() is recursively called inside \par loop() function.
     * @details In normal condition \par run() is the last funciton to be called and it is called inside a the 
     * loop() function (for arduino architectures) or inside an while(1) loop.
     */

    void run()
    {
        for (int i = 0; i < _NloadedModes; i++)
        {
            if (_modes[i]->isActive())
            {
                if (_PeriodMode[i] > 0)
                {
                    if (micros() < _timeLastCallMode[i])
                        _timeLastCallMode[i] = 0;

                    if (micros() - _timeLastCallMode[i] > _PeriodMode[i])
                    {
                        _modes[i]->loop();
                        _timeLastCallMode[i] = micros();
                    }
                }
                else
                {
                    _modes[i]->loop();
                }
            }
        }
    }

    /**
     * @brief This function disable every loaded modes.
     */

    void disableAll()
    {
        for (int i = 0; i < _NloadedModes; i++)
            _modes[i]->Disable();
    }

    /**
     * @brief This function enable every loaded modes.
     */

    void EnableAll()
    {
        for (int i = 0; i < _NloadedModes; i++)
            _modes[i]->Enable();
    }

    /**
     * @brief This function returns the mode with the requested name,
     * @param name is the name of the requeste mode.
     * @return Returs the pointer to the mode with the requested name. If there isn't a mode with this name returns a nullptr.
     */

    VMode *getMode(String name)
    {
        for (int i = 0; i < _NloadedModes; i++)
            if (name.equals(_modes[i]->getName()))
                return _modes[i];
        return nullptr;
    }

    /**
     * @brief This function remove the mode with the requested name,
     * @param name is the name of the requeste mode.
     */

    void removeMode(String name)
    {
        for (int i = 0; i < _NloadedModes; i++)
            if (name.equals(_modes[i]->getName()))
            {
                for (int j = i; j < _NloadedModes - 1; j++)
                {
                    _modes[j] = _modes[j + 1];
                    _PeriodMode[j] = _PeriodMode[j + 1];
                    _timeLastCallMode[j] = _timeLastCallMode[j + 1];
                }

                _NloadedModes--;
                return;
            }
    }
};

LoopManager *LoopManager::_instance = new LoopManager(20);

#endif // !
