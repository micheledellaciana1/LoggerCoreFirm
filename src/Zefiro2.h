#ifndef ZEFIRO_H
#define ZEFIRO_H

#include <protocol/SerialDevice.h>
#include <protocol/SerialUtility.h>

#include <Utility/EasyDAC.h>
#include <Utility/EasyADC.h>
#include <Utility/HeaterDriver/HeaterDriverCC.h>
#include <Utility/HeaterDriver/LedDriver.h>
#include <Utility/AmpMeter.h>
#include <Utility/SHT31_ARP_B2_5kS.h>
#include <Debug/Oscope.h>
class Zefiro2 : public SerialDevice, public VMode
{
public:
    HeaterDriverCC *_heaterDriver; /// Heater driver
    LedDriver *_ledDriver;
    AmpMeter *_AmpMeter;          /// AmpMeter istance
    SHT31_ARP_B2_5kS *_TRHSensor; /// RH - Temperature sensor

    int16_t _pinDACSensor;        /// pin DAC sensor
    EasyDAC *_DACSensor;          /// DAC sensor
    int16_t _pinADCVoltageSensor; /// pin ADC voltage sensor
    EasyADC *_ADCVoltageSensor;

    long averangeTimeSignals = 20000; // microsec

    float voltageSensor;
    float currentSensor;
    float resistanceHeater;
    float tempSensor;
    float RHSensor;
    float OxygenC;

    Zefiro2(String name, Stream *serial,
            int16_t pinADCVoltageSensor, EasyADC *ADCVoltageSensor,
            int16_t pinDACSensor, EasyDAC *DACSensor,
            HeaterDriverCC *heaterDriver,
            LedDriver *ledDriver,
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

        _ledDriver = ledDriver;
        ledDriver->setCurrentValue(0);

        _AmpMeter = ampMeter;
        _TRHSensor = TemperatureRHSensor;

        _EventManager->addVEvent([this]() { this->getStream()->println(this->voltageSensor, 9); }, "ReadVoltageSensor");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->currentSensor, 9); }, "ReadCurrentSensor");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->resistanceHeater, 9); }, "ReadHeaterResistance");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->tempSensor, 9); }, "ReadTempSensor");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->RHSensor, 9); }, "ReadRHSensor");
        _EventManager->addVEvent([this]() { this->getStream()->println(this->OxygenC, 9); }, "ReadOxygenSensor");
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
                String currentValue;
                readInputLine(currentValue, this->getStream());
                this->_ledDriver->setCurrentValue(currentValue.toFloat()); }, "SetcurrentLED");

        _EventManager->addVEvent([this]() {
            String ExitationCurrent;
            readInputLine(ExitationCurrent, this->getStream());
            this->_heaterDriver->setCurrentValue(ExitationCurrent.toFloat());
            delayMicroseconds(1000);
            float resistance = this->_heaterDriver->getHeaterResistanceValue_averanging(1);
            this->_heaterDriver->setCurrentValue(0);
            if (resistance > 1e6)
                resistance = 1e6;
            this->getStream()->println(resistance, 9);
        },
                                 "ReadHeaterResistance_withExitation");

        _EventManager->addVEvent([this]() {
            String ExitationCurrent;
            readInputLine(ExitationCurrent, this->getStream());
            this->_heaterDriver->setCurrentValue(ExitationCurrent.toFloat());
            delayMicroseconds(1000);
            float resistance = this->_heaterDriver->getHeaterResistanceValue_averanging(10000);
            this->_heaterDriver->setCurrentValue(0);
            if (resistance > 1e6)
                resistance = 1e6;
            this->getStream()->println(resistance, 9);
        },
                                 "ReadHeaterResistance_withExitation_ENHANCED");

        addOScopeEvent();
    }

    virtual ~Zefiro2()
    {
    }

    void loop()
    {
        long start = micros();
        int16_t counter = 0;

        voltageSensor = 0;
        currentSensor = 0;
        float currentHeater = 0;
        float voltageFallHeater = 0;
        tempSensor = 0;
        RHSensor = 0;
        OxygenC = 0;

        do
        {
            voltageSensor += _ADCVoltageSensor->getVoltagef(analogRead(_pinADCVoltageSensor));
            currentSensor += _AmpMeter->MeasureCurrent();
            currentHeater += _heaterDriver->getCurrentValue();
            voltageFallHeater += _heaterDriver->getVoltageFall();
            tempSensor += _TRHSensor->readTemp();
            RHSensor += _TRHSensor->readRH();
            OxygenC += -1;
            counter++;
        } while (micros() - start < averangeTimeSignals);

        voltageSensor /= counter;
        currentSensor /= counter;
        currentSensor -= CorrectionLeak(voltageSensor);

        if (currentHeater != 0)
            resistanceHeater = voltageFallHeater / currentHeater;

        //turn off if read bad heater reasistance

        if (resistanceHeater < 0)
            _heaterDriver->setCurrentValue(0);

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

    void setLeakCorreciton(double *pol, int pDim)
    {
        _pol = pol;
        _pDim = pDim;
    }

private:
    double _pDim = 0;
    double *_pol;

    double CorrectionLeak(double voltage)
    {
        double leak = 0;
        for (size_t i = 0; i < _pDim; i++)
            leak += _pol[i] * pow(voltage, i);
        return leak;
    }
};

#endif // !ZEFIRO
