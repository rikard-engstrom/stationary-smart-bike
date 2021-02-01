#include "Arduino.h"
#include "BLEHelper.hh"

BLEHelper bleHelper;

uint16_t totalCranks = 0;
uint16_t lastEventTime = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println("Start");
    bleHelper.setup();
}

void loop()
{
    // 90 RPM
    totalCranks++;
    bleHelper.notifyCadence(totalCranks, lastEventTime);
    lastEventTime += 682;
    delay(700);
}
