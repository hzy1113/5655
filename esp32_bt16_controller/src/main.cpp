#include <Arduino.h>
#include "config.h"
#include "amplifier.h"
#include "touch.h"

AmplifierControl amplifier;
TouchHandler touchHandler(&amplifier);

void setup() {
    Serial.begin(115200);
    Serial.println("========== ESP32C3 BT-16 Controller ==========");

    amplifier.begin();
    touchHandler.begin();

    Serial.println("[BOOT] Ready");
}

void loop() {
    touchHandler.update();
    delay(10);
}
