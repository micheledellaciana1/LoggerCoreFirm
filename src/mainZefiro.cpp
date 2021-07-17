
#include <Arduino.h>
#include <protocol/SerialDevice.h>
#include <protocol/LoopManager.h>
#include <Zefiro2.h>
//#include <Zefiro1.h>
#include <Utility/HeaterDriver/HeaterDriverPWM.h>
#include <Utility/HeaterDriver/HeaterDriverCC.h>
#include <Utility/HeaterDriver/LedDriver.h>
#include <Utility/SHT31_ARP_B2_5kS.h>
#include <Utility/AmpMeter.h>
#include <Utility/mux/muxMAX338_8_1.h>

int16_t ADC_resolution = 14;
int16_t DAC_resolution = 12;
Zefiro2 *zef;

/*
void setup()
{
  EasyDAC::bitResolution = DAC_resolution;
  EasyADC::bitResolution = ADC_resolution;

  analogReadResolution(ADC_resolution);
  analogWriteResolution(DAC_resolution);

  float HDrefResistor = 6.98;
  analogWriteFrequency(22, 2929.687);
  HeaterDriverPWM *heaterDriver = new HeaterDriverPWM(23,
                                                      22, new EasyDAC(0, 12),
                                                      17, new EasyADC(0, 17.69),
                                                      16, new EasyADC(0, 3.3),
                                                      HDrefResistor);

  SHT31_ARP_B2_5kS *TRHSensor = new SHT31_ARP_B2_5kS(20, 21, 3.3);

  AmpMeter *ampMeter = new AmpMeter(14,
                                    new EasyADC(-1.65, 1.65),
                                    // new EasyADC(-1.6534, 1.6466),
                                    new muxMAX338_8_1(0, 1, 2, -1),
                                    new float[8]{1e-1, 1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6},
                                    1.0);

  ampMeter->minIndexUsableFromAutorange = 0;
  ampMeter->maxIndexUsableFromAutorange = 5;

  zef = new Zefiro1("Zefiro", &Serial,
                    15, new EasyADC(-12.67987, 12.69547),
                    A14, new EasyDAC(-12.75, 12.85),
                    heaterDriver,
                    ampMeter,
                    TRHSensor);

  analogReference(EXTERNAL);

  LoopManager::getInstance()->add_mode(zef->getEventManager());
  LoopManager::getInstance()->add_mode(zef, 100);

  Serial.begin(9600);
  zef->Open();
}
*/

void setup()
{
  EasyDAC::bitResolution = DAC_resolution;
  EasyADC::bitResolution = ADC_resolution;

  analogReadResolution(ADC_resolution);
  analogWriteResolution(DAC_resolution);

  float HDrefResistor = 16. / 3;
  HeaterDriverCC *heaterDriver = new HeaterDriverCC(A21, new EasyDAC(0, 3.3),
                                                    38, new EasyADC(0, 13.2),
                                                    39, new EasyADC(0, 3.3),
                                                    HDrefResistor);

  LedDriver *ledDriver = new LedDriver(5, new EasyDAC(0, 3.3), 3.2);

  SHT31_ARP_B2_5kS *TRHSensor = new SHT31_ARP_B2_5kS(35, 36, 3.3);

  AmpMeter *ampMeter = new AmpMeter(14,
                                    new EasyADC(-0.825, 0.825),
                                    new muxMAX338_8_1(2, 1, 0, -1),
                                    new float[8]{1e-2, 1e-1, 1e0, 1e1, 1e2, 1e3, 1e4, 0.5e5},
                                    0.5);
  zef = new Zefiro2("Zefiro", &Serial,
                    15, new EasyADC(-14.972, 14.988),
                    A22, new EasyDAC(-13.7823529, 13.7823529),
                    heaterDriver,
                    ledDriver,
                    ampMeter,
                    TRHSensor);

  double *pol = new double[5];
  pol[0] = -1.8798938880441764e-7;
  pol[1] = 4.505519495060779e-8;
  pol[2] = -4.534407282559641e-9;
  pol[3] = 2.4195651525937e-10;
  pol[4] = -5.010224631530787e-12;
  zef->setLeakCorreciton(pol, 5);

  analogReference(EXTERNAL);

  LoopManager::getInstance()->add_mode(zef->getEventManager());
  LoopManager::getInstance()->add_mode(zef, 10);

  Serial.begin(9600);
  zef->Open();
}

void loop()
{
  LoopManager::getInstance()->run();
}
