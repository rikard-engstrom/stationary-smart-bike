#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "arduino.h"
#include "BLEHelper.hh"

#define cadenceServiceId BLEUUID((uint16_t)0x1816)

BLEHelper::BLEHelper()
{
}

void BLEHelper::setup()
{
    BLEDevice::init("Bike-7");
    BLEServer *pServer = BLEDevice::createServer();

    BLEService *bleCadence = pServer->createService(cadenceServiceId);

    // -- Feature Characteristics --------------------------------------

    // bit 0 - Wheel Revolution Data Supported
    // bit 1 - Crank Revolution Data Supported
    // bit 2 - Multiple Sensor Locations Supported
    // bit 3-16 - Reserved
    byte cscFeatureFlags[1] = {0b0000000000000010};

    _featureCharacteristics = bleCadence->createCharacteristic(BLEUUID((uint16_t)0x2A5C), BLECharacteristic::PROPERTY_READ);
    _featureCharacteristics->setValue(cscFeatureFlags, 1);

    // -- Measurement Characteristics --------------------------------------
    _measurementCharacteristics = bleCadence->createCharacteristic(BLEUUID((uint16_t)0x2A5B), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    _measurementCharacteristics->addDescriptor(new BLE2902()); // Client Characteristic Configuration

    bleCadence->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(cadenceServiceId);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
}

void BLEHelper ::notifyCadence(unsigned int cumulativeCrankRevolutions, unsigned int lastCrankEventTime)
{
    // bit 0 - Wheel Revolution Data Present
    // bit 1 - Crank Revolution Data Present
    // bit 2-7 - Reserved
    byte cscMeasurementFlags = 0b00000010;

    byte value[5] = {cscMeasurementFlags, 0, 0 , 0, 0};
    value[1] = cumulativeCrankRevolutions & 0xFF;
    value[2] = (cumulativeCrankRevolutions >> 8) & 0xFF; 
    value[3] = lastCrankEventTime & 0xFF;
    value[4] = (lastCrankEventTime >> 8) & 0xFF; 

    _measurementCharacteristics->setValue(value, 5);
    _measurementCharacteristics->notify();
}
