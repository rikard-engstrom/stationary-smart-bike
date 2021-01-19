#ifndef BLEHelper_h
#define BLEHelper_h

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "arduino.h"

class BLEHelper
{
  public:
    BLEHelper();
    void setup();
    void notifyCadence(unsigned int cumulativeCrankRevolutions, unsigned int lastCrankEventTime);

  private:
    BLECharacteristic* _featureCharacteristics = NULL;
    BLECharacteristic* _measurementCharacteristics = NULL;
};

#endif
