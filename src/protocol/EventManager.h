
/**
 * @file EventManager.h
 * @author Michele Della Ciana
 * @date 19 Nov 2019
*/

#ifndef EVENTMANAGER
#define EVENTMANAGER

#include <Arduino.h>
#include <protocol/SerialUtility.h>
#include <protocol/VMode.h>
#include <functional>
#include <vector>

using namespace std;

class StringEvent;

class EventManager : public VMode
{
private:
    vector<function<void()>> _Events;
    vector<String> _triggerString;

public:
    EventManager(int16_t max_NEvent, String name) : VMode(name)
    {
    }

    virtual ~EventManager() {}

    void executeEvents(String ReadString)
    {
        for (int16_t i = 0; i < _Events.size(); i++)
            if (_triggerString[i].equals(ReadString))
                _Events[i]();
    }

    void addVEvent(std::function<void()> newEvent, String triggerString)
    {
        _Events.push_back(newEvent);
        _triggerString.push_back(triggerString);
    }

    void loop()
    {
        String ReadString;
        if (tryToreadInputLine(ReadString))
        {
            executeEvents(ReadString);
        }
    }
};

class StringEvent : public function<void()>
{
public:
    String _triggerString;
    StringEvent(String triggerString) : function<void()>()
    {
        _triggerString = triggerString;
    }
};

#endif