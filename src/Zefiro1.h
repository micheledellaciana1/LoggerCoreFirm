#ifndef ZEFIRO_H
#define ZEFIRO_H

#include <protocol/SerialDevice.h>
#include <protocol/SerialUtility.h>

#include <Utility/EasyDAC.h>
#include <Utility/EasyADC.h>
#include <Utility/HeaterDriver/HeaterDriverPWM.h>
#include <Utility/AmpMeter.h>
#include <Utility/SHT31_ARP_B2_5kS.h>
#include <Debug/Oscope.h>

class Zefiro1 : public SerialDevice, public VMode
{
public:
    HeaterDriverPWM *_heaterDriver; /// Heater driver
    AmpMeter *_AmpMeter;            /// AmpMeter istance
    SHT31_ARP_B2_5kS *_TRHSensor;   /// RH - Temperature sensor

    int16_t _pinDACSensor;        /// pin DAC sensor
    EasyDAC *_DACSensor;          /// DAC sensor
    int16_t _pinADCVoltageSensor; /// pin ADC voltage sensor
    EasyADC *_ADCVoltageSensor;

    long averangeTimeSignals = 25000L; // microsec

    float voltageSensor;
    float currentSensor;
    float resistanceHeater;
    float tempSensor;
    float RHSensor;
    float OxygenC;

    Zefiro1(String name, Stream *serial,
            int16_t pinADCVoltageSensor, EasyADC *ADCVoltageSensor,
            int16_t pinDACSensor, EasyDAC *DACSensor,
            HeaterDriverPWM *heaterDriver,
            AmpMeter *ampMeter,
            SHT31_ARP_B2_5kS *TemperatureRHSensor) : SerialDevice(name, serial), VMode()
    {
        _pinADCVoltageSensor = pinADCVoltageSensor;
        _ADCVoltageSensor = ADCVoltageSensor;

        _pinDACSensor = pinDACSensor;
        _DACSensor = DACSensor;
        analogWrite(pinDACSensor, DACSensor->getVoltageI(0));

        _heaterDriver = heaterDriver;
        heaterDriver->setCurrentValue(0);

        _AmpMeter = ampMeter;
        _TRHSensor = TemperatureRHSensor;

        _EventManager->addVEvent([this]() { this->getStream()->println(this->voltageSensor, 12); }, "ReadVoltageSensor");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->currentSensor, 12); }, "ReadCurrentSensor");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->resistanceHeater, 12); }, "ReadHeaterResistance");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->tempSensor, 12); }, "ReadTempSensor");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->RHSensor, 12); }, "ReadRHSensor");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->OxygenC, 12); }, "ReadOxygenSensor");
        _EventManager->addVEvent([this]() { this->_AmpMeter->Autorange = false; }, "DisableCurrentSensorAutorange");
        _EventManager->addVEvent([this]() { this->_AmpMeter->Autorange = true; }, "EnableCurrentSensorAutorange");

        _EventManager->addVEvent([this]() {
            String idxAmpMeter;
            readInputLine(idxAmpMeter, this->getStream());
            this->_AmpMeter->getMux()->selectLine(idxAmpMeter.toInt()); }, "SetAmpMeterIdx");

        _EventManager->addVEvent([this]() {
            String AverageTime;
            readInputLine(AverageTime, this->getStream());
            this->averangeTimeSignals = AverageTime.toInt() * 1000; }, "SetAverageTime");

        _EventManager->addVEvent([this]() {
            String DACvalue;
            readInputLine(DACvalue, this->getStream());
            analogWrite(_pinDACSensor, _DACSensor->getVoltageI(DACvalue.toFloat())); }, "SetDACvoltagesensor");

        _EventManager->addVEvent([this]() {
                String currentValue;
                readInputLine(currentValue, this->getStream());
                this->_heaterDriver->setCurrentValue(currentValue.toFloat()); }, "Setcurrentheater");

        _EventManager->addVEvent([this]() {
            analogWrite(_heaterDriver->getPWMPin(), 0);
            delayMicroseconds(100 * 1000);

            _heaterDriver->setInMeasurementMode();
            float resistance = this->_heaterDriver->getHeaterResistanceValue();
            _heaterDriver->setInSupplyMode();
            this->getStream()->println(resistance, 12);
        },
                                 "ReadHeaterResistance_withExitation");

        addOScopeEvent();
    }

    virtual ~Zefiro1()
    {
    }

    void loop()
    {
        int16_t counter = 0;

        voltageSensor = 0;
        currentSensor = 0;
        float currentHeater = 0;
        float voltageFallHeater = 0;
        tempSensor = 0;
        RHSensor = 0;
        OxygenC = 0;

        analogWrite(_heaterDriver->getPWMPin(), 0);
        delayMicroseconds(8 * 1000);

        long start = micros();
        _AmpMeter->MeasureCurrent();
        _AmpMeter->Autorange = false;

        do
        {
            voltageSensor += _ADCVoltageSensor->getVoltagef(analogRead(_pinADCVoltageSensor));
            currentSensor += _AmpMeter->MeasureCurrent();
            tempSensor += _TRHSensor->readTemp();
            RHSensor += _TRHSensor->readRH();
            OxygenC += -1;
            counter++;
        } while (micros() - start < averangeTimeSignals);
        _AmpMeter->Autorange = true;

        _heaterDriver->setInMeasurementMode();
        currentHeater += _heaterDriver->getCurrentValue();
        voltageFallHeater += _heaterDriver->getVoltageFall();
        _heaterDriver->setInSupplyMode();

        voltageSensor /= counter;
        currentSensor /= counter;
        resistanceHeater = voltageFallHeater / currentHeater;
        tempSensor /= counter;
        RHSensor /= counter;
        OxygenC /= counter;
    }

    virtual void Reset()
    {
        analogWrite(_pinDACSensor, _DACSensor->getVoltageI(0));
        _heaterDriver->setCurrentValue(0);

        _AmpMeter->Autorange = true;
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

#endif // !ZEFIRO
