#include "BLEHelper.hh"

BLEHelper bleHelper;

unsigned int totalCranks = 0;
unsigned int lastEventTime = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println("Start");
    bleHelper.setup();
}

void loop()
{
    totalCranks += 1;
    bleHelper.notifyCadence(totalCranks, lastEventTime);
    lastEventTime += 682;

    /*Base Unit: org.bluetooth.unit.time.second
Represented values: M = 1, d = 0, b = -10
Unit is 1/1024 second
*/
    delay(700);
}
