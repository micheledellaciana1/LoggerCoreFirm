#ifndef ICARO1_H
#define ICARO1_H

#define maxVoltagePt100 3.1;
#define minVoltagePt100 0.2;

#include <protocol/SerialDevice.h>
#include <protocol/SerialUtility.h>

#include <Utility/EasyDAC.h>
#include <Utility/EasyADC.h>
#include <Utility/SHT31_ARP_B2_5kS.h>
#include <Utility/TEMP235.h>
#include <Debug/Oscope.h>

class Icaro1 : public SerialDevice, public VMode
{
public:
    SHT31_ARP_B2_5kS *TRHSensor; /// RH - Temperature sensor
    TEMP235 *TSensor;            /// T board - Temperature sensor

    EasyDAC *DACPower;
    EasyDAC *DACPt100;
    float voltageDACPt100;

    EasyADC *ADCPt100;

    long averangeTimeSignals = 100000; // microsec

    float R0;
    float gain;
    float PT100Resistance;
    float tempBoard;
    float tempSensor;
    float RHSensor;

    Icaro1(String _name, Stream *_serial, float _R0, float _gain, SHT31_ARP_B2_5kS *_TemperatureRHSensor, TEMP235 *_TSensor) : SerialDevice(_name, _serial), VMode()
    {
        R0 = _R0;
        gain = _gain;
        DACPower = &EasyDAC::DAC_0_3_3;
        analogWriteFrequency(10, 200);
        analogWrite(10, DACPower->getVoltageI(DACPower->getV_max()));

        DACPt100 = &EasyDAC::DAC_0_3_3;
        analogWrite(A14, DACPt100->getVoltageI(1.5));
        voltageDACPt100 = DACPt100->getVoltagef(DACPt100->getVoltageI(1.5));

        ADCPt100 = &EasyADC::ADC_0_3_3;

        TRHSensor = _TemperatureRHSensor;
        TSensor = _TSensor;

        _EventManager->addVEvent([this]() { this->getStream()->println(this->PT100Resistance, 9); }, "Read_PT100_Resistance");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->tempSensor, 9); }, "Read_Temp_Sensor");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->tempBoard, 9); }, "Read_Temp_Board");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->RHSensor, 9); }, "Read_RH_Sensor");

        _EventManager->addVEvent([this]() {
            String voltage;
            readInputLine(voltage, this->getStream());
            float voltagef = voltage.toFloat();
            analogWrite(A14, DACPt100->getVoltageI(voltagef));
            voltageDACPt100 = voltageDACPt100 = DACPt100->getVoltagef(DACPt100->getVoltageI(voltagef));
        },
                                 "Set_DAC_Pt100_Voltage");

        _EventManager->addVEvent([this]() {
            String AverageTime;
            readInputLine(AverageTime, this->getStream());
            this->averangeTimeSignals = AverageTime.toInt() * 1000; }, "Set_Average_Time");

        _EventManager->addVEvent([this]() {
            String voltageHeater;
            readInputLine(voltageHeater, this->getStream());
            analogWrite(10, DACPower->getVoltageI(DACPower->getV_max() - voltageHeater.toFloat()));
        },
                                 "Set_Voltage_Heater");

        addOScopeEvent();
    }

    virtual ~Icaro1()
    {
    }

    void Reset()
    {
        analogWrite(10, DACPower->getVoltageI(DACPower->getV_max()));
        analogWrite(A14, DACPt100->getVoltageI(1));
        voltageDACPt100 = DACPt100->getVoltagef(DACPt100->getVoltageI(1));
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
            PT100Resistance += MeasureResistancePt100();

            tempSensor += TRHSensor->readTemp();
            RHSensor += TRHSensor->readRH();
            tempBoard += TSensor->readTemp();
            counter++;
        } while (micros() - start < averangeTimeSignals);

        PT100Resistance /= counter;
        tempSensor /= counter;
        RHSensor /= counter;
        tempBoard /= counter;
    }

    float MeasureResistancePt100()
    {
        float voltage = ADCPt100->getVoltagef(analogRead(19));
        float Resistance = (voltage + voltageDACPt100) / (gain * voltageDACPt100) * R0;

        return Resistance;
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

#endif // !ICARO1
