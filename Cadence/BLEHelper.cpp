#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "arduino.h"
#include "BLEHelper.hh"

#define cadenceServiceId BLEUUID((uint16_t)0x1816)

BLEService *bleCadence;
BLECharacteristic *featureCharacteristics;

BLEHelper::BLEHelper()
{
}

void BLEHelper::setup()
{
    BLEDevice::init("BIKE1");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    setupDeviceInfo(pServer);
    setupBatteryService(pServer);

    bleCadence = pServer->createService(cadenceServiceId);

    // -- Feature Characteristics --------------------------------------
    featureCharacteristics = bleCadence->createCharacteristic(BLEUUID((uint16_t)0x2A5C), BLECharacteristic::PROPERTY_READ);

    // Crank Revolution Data Supported
    byte cscFeatureFlags[2] = {0b0000000000000010, 0};
    featureCharacteristics->setValue(cscFeatureFlags, 2);

    // -- Sensor Location Characteristics ------------------------------
    BLECharacteristic *sensorLocation = bleCadence->createCharacteristic(BLEUUID((uint16_t)0x2A5D), BLECharacteristic::PROPERTY_READ);
    // Right crank
    byte sensorLocationValue[1] = {6};
    sensorLocation->setValue(sensorLocationValue, 1);

    // -- SC Control Point Characteristics -----------------------------
    BLECharacteristic *scControlPoint = bleCadence->createCharacteristic(BLEUUID((uint16_t)0x2A55), BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_INDICATE);
    
    // Client Characteristic Configuration Value: 00-00
    scControlPoint->addDescriptor(new BLE2902());

    // -- Measurement Characteristics ----------------------------------
    _measurementCharacteristics = bleCadence->createCharacteristic(BLEUUID((uint16_t)0x2A5B), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    _measurementCharacteristics->addDescriptor(new BLE2902()); // Client Characteristic Configuration

    bleCadence->start();
    this->startAdvertising(pServer);
}

void BLEHelper::setupDeviceInfo(BLEServer *pServer)
{
#define deviceInformationServiceId BLEUUID((uint16_t)0x180A)
    BLEService *deviceInformationService = pServer->createService(deviceInformationServiceId);
    BLECharacteristic *firmware = deviceInformationService->createCharacteristic(BLEUUID((uint16_t)0x2A26), BLECharacteristic::PROPERTY_READ);
    firmware->setValue("1.0.2");
    BLECharacteristic *hardware = deviceInformationService->createCharacteristic(BLEUUID((uint16_t)0x2A27), BLECharacteristic::PROPERTY_READ);
    hardware->setValue("1.0.0");
    BLECharacteristic *manufacturer = deviceInformationService->createCharacteristic(BLEUUID((uint16_t)0x2A29), BLECharacteristic::PROPERTY_READ);
    manufacturer->setValue("ACME");
    deviceInformationService->start();
}

void BLEHelper::setupBatteryService(BLEServer *pServer)
{
#define batteryLevelServiceId BLEUUID((uint16_t)0x180F)
    BLEService *batteryLevelService = pServer->createService(batteryLevelServiceId);
    BLECharacteristic *batteryLevelCharacteristics = batteryLevelService->createCharacteristic(BLEUUID((uint16_t)0x2A19), BLECharacteristic::PROPERTY_READ);
    batteryLevelCharacteristics->addDescriptor(new BLE2902());
    byte value[1] = {50};
    batteryLevelCharacteristics->setValue(value, 1);
}

void BLEHelper::onConnect(BLEServer *pServer)
{
    Serial.println("onConnect");
}

void BLEHelper::onDisconnect(BLEServer *pServer)
{
    Serial.println("onDisconnect");
}

void BLEHelper::startAdvertising(BLEServer *pServer)
{
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(cadenceServiceId);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not advertise this parameter
    pAdvertising->start();
}

void BLEHelper ::notifyCadence(uint16_t cumulativeCrankRevolutions, uint16_t lastCrankEventTime)
{
    // bit 0 - Wheel Revolution Data Present
    // bit 1 - Crank Revolution Data Present
    // bit 2-7 - Reserved
    byte cscMeasurementFlags = 0b00000010;

    byte value[5] = {cscMeasurementFlags, 0, 0, 0, 0};
    value[1] = cumulativeCrankRevolutions & 0xFF;
    value[2] = (cumulativeCrankRevolutions >> 8) & 0xFF;
    value[3] = lastCrankEventTime & 0xFF;
    value[4] = (lastCrankEventTime >> 8) & 0xFF;

    _measurementCharacteristics->setValue(value, 5);
    _measurementCharacteristics->notify();
}
