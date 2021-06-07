/**
 * @file SerialUtility.h
 * @author Michele Della Ciana
 * @date 18 Nov 2019
 * @brief It contains global functions to semplify serial comunication based on Arduino.h Stream object.
 */

#ifndef SERIALUTILITY
#define SERIALUTILITY

#include <Arduino.h>

/**
 * @brief ReadInputLine waits untile it reads a line  from a Strem * ptr.
 * Results is stored in \p ReadString.
 * @param ReadString is a String object that returns the read string trimmered.
 * @param _serial is an initializated Stream ptr (i.e. a Serial object).
 */
void readInputLine(String &ReadString, Stream *_serial = &Serial);

/**
 * @brief EstablishCOnnection is a function that establish a connection with a Serial device.
 *  It wait until it reads a valid line.
 * @param _serial is a stream object (a Serial object).
 * @param _name is the name of the device to connect.
 */

void EstablishConnection(Stream *_serial, String _name);

/**
 * @brief tryToreadInputLine is a function that try to read an inputLine, (if there is a line in the Serial buffer).
 * @param ReadString is a String object that returns the read string trimmered.
 * @param _serial is an initializated Stream ptr (i.e. a Serial object).
 * @return It returns true if it finds a valid line, otherwise false.
 */
bool tryToreadInputLine(String &ReadString, Stream *_serial = &Serial);

void readInputLine(String &ReadString, Stream *_serial)
{
    bool flag = true;
    while (flag)
        if (_serial->available() > 0)
        {
            ReadString = "";
            ReadString = _serial->readStringUntil('\n');
            ReadString.trim();
            if (ReadString != "")
                flag = false;
        }
}

bool tryToreadInputLine(String &ReadString, Stream *_serial)
{
    if (_serial->available() > 0)
    {
        ReadString = "";
        ReadString = _serial->readStringUntil('\n');
        ReadString.trim();
        if (ReadString != "")
            return true;
        else
        {
            return false;
        }
    }
    return false;
}

void EstablishConnection(Stream *_serial, String _name)
{
    String readString;
    while (true)
    {
        _serial->println(_name + "_Ready");
        delay(50);
        if (tryToreadInputLine(readString, _serial))
            if (readString.equalsIgnoreCase(_name + "_Open"))
            {
                while (_serial->available() > 0)
                    _serial->read();

                _serial->println("EstablishedConnection");
                break;
            }
    }
}

#endif