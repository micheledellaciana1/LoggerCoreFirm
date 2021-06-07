#ifndef ICARO2_H
#define ICARO2_H

#include <protocol/SerialDevice.h>
#include <protocol/SerialUtility.h>

#include <Utility/EasyDAC.h>
#include <Utility/EasyADC.h>
#include <Utility/SHT31_ARP_B2_5kS.h>
#include <Utility/TEMP235.h>
#include <Debug/Oscope.h>

class Icaro2 : public SerialDevice, public VMode
{
public:
    SHT31_ARP_B2_5kS *_TRHSensor; /// RH - Temperature sensor
    TEMP235 *_TSensor;            /// T board - Temperature sensor

    EasyDAC *DACPower;
    long averangeTimeSignals = 50000; // microsec

    float R0;
    float PT100Resistance;
    float tempBoard;
    float tempSensor;
    float RHSensor;

    Icaro2(String name, Stream *serial, float R0, EasyDAC *DAC, SHT31_ARP_B2_5kS *TemperatureRHSensor, TEMP235 *TSensor) : SerialDevice(name, serial), VMode()
    {
        this->R0 = R0;
        DACPower = DAC;
        _TRHSensor = TemperatureRHSensor;
        _TSensor = TSensor;

        _EventManager->addVEvent([this]() { this->getStream()->println(this->PT100Resistance, 9); }, "Read_PT100_Resistance");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->tempSensor, 9); }, "Read_Temp_Sensor");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->tempBoard, 9); }, "Read_Temp_Board");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->RHSensor, 9); }, "Read_RH_Sensor");

        _EventManager->addVEvent([this]() {
            String AverageTime;
            readInputLine(AverageTime, this->getStream());
            this->averangeTimeSignals = AverageTime.toInt() * 1000; }, "Set_Average_Time");

        _EventManager->addVEvent([this]() {
            String voltageHeater;
            readInputLine(voltageHeater, this->getStream());
            analogWrite(A14, DACPower->getVoltageI(voltageHeater.toFloat()));
        },
                                 "Set_Voltage_Heater");

        addOScopeEvent();
    }

    virtual ~Icaro2()
    {
    }

    void loop()
    {
        long start = micros();
        int16_t counter = 0;

        PT100Resistance = 0;
        tempSensor = 0;
        RHSensor = 0;
        tempBoard = 0;

        do
        {
            float ratio = analogRead(A10) / double(analogRead(A11));
            PT100Resistance += (ratio - 1) * R0;

            tempSensor += _TRHSensor->readTemp();
            RHSensor += _TRHSensor->readRH();
            tempBoard += _TSensor->readTemp();
            counter++;
        } while (micros() - start < averangeTimeSignals);

        PT100Resistance /= counter;
        tempSensor /= counter;
        RHSensor /= counter;
        tempBoard /= counter;
    }

    virtual void Reset()
    {
    }

    void addOScopeEvent()
    {
        _EventManager->addVEvent([this]() {
            String Channel;
            readInputLine(Channel, this->getStream());
            String NPoints;
            readInputLine(NPoints, this->getStream());

            Oscope scope = Oscope(analogRead, NPoints.toInt());
            uint16_t *_data = scope.acquireSingleChannel(Channel.toInt());

            this->getStream()->println(scope.getPeriod(), 9);
            for (int i = 0; i < scope.getArraySize(); i++)
                this->getStream()->println(EasyADC::ADC_0_3_3.getVoltagef(_data[i]), 9);
        },
                                 "ReadFastValues");
    }
};

#endif // !ICARO2
