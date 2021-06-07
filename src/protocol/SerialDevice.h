#ifndef SERIAL_DEVICE
#define SERIAL_DEVICE

#include <protocol/SerialUtility.h>
#include <protocol/EventManager.h>

class SerialDevice
{
protected:
    String _name;
    Stream *_serial;

    EventManager *_EventManager;

public:
    SerialDevice(String name, Stream *serial, int16_t _NEvent = 50)
    {
        _name = name;
        _serial = serial;
        _EventManager = new EventManager(_NEvent, "EventManager_" + name);

        _EventManager->addVEvent([this]() { this->Reset(); }, "Reset");
        _EventManager->addVEvent([this]() { this->closeDevice(); }, "Close");
    }

    virtual ~SerialDevice() {}

    void Open()
    {
        pinMode(13, OUTPUT);
        digitalWrite(13, HIGH);
        EstablishConnection(_serial, _name);
        digitalWrite(13, LOW);
        pinMode(13, INPUT);
    }

    virtual void Reset()
    {
    }

    virtual void closeDevice()
    {
        Reset();
        Serial.end();
        Serial.begin(Serial.baud());

        Open();
    }

    Stream *getStream()
    {
        return _serial;
    }

    EventManager *getEventManager()
    {
        return _EventManager;
    }

    const String &getName() const
    {
        return _name;
    }
};

#endif // !SERIAL_DEVICE