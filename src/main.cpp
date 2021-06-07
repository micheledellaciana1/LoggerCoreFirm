#include <Arduino.h>
#include <protocol/SerialDevice.h>
#include <protocol/LoopManager.h>
#include <Utility/SHT31_ARP_B2_5kS.h>
#include <Utility/TEMP235.h>
#include <Icaro1.h>

int16_t ADC_resolution = 14;
int16_t DAC_resolution = 12;
Icaro1 *ica;

void setup()
{
  EasyDAC::bitResolution = DAC_resolution;
  EasyADC::bitResolution = ADC_resolution;

  analogReadResolution(ADC_resolution);
  analogWriteResolution(DAC_resolution);

  float R0 = 1e3;
  SHT31_ARP_B2_5kS *TRHSensor = new SHT31_ARP_B2_5kS(A4, A3, 3.3);
  TEMP235 *TSensor = new TEMP235(15, &EasyADC::ADC_0_3_3);

  ica = new Icaro1("Icaro", &Serial, R0, 11, TRHSensor, TSensor);

  LoopManager::getInstance()->add_mode(ica->getEventManager());
  LoopManager::getInstance()->add_mode(ica, 10);

  Serial.begin(9600);
  ica->Open();
}

void loop()
{
  LoopManager::getInstance()->run();
}