#ifndef BLEHelper_h
#define BLEHelper_h

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "arduino.h"

class BLEHelper : public BLEServerCallbacks
{
public:
  BLEHelper();
  void setup();
  void setupDeviceInfo(BLEServer *pServer);
  void setupBatteryService(BLEServer *pServer);
  void notifyCadence(uint16_t cumulativeCrankRevolutions, uint16_t lastCrankEventTime);
  void onConnect(BLEServer *pServer);
  void onDisconnect(BLEServer *pServer);

private:
  BLECharacteristic *_measurementCharacteristics = NULL;
  void startAdvertising(BLEServer *pServer);
};

#endif
